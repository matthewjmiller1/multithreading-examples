#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

struct common_ctx_t_ {
    uint8_t printer_tid;
    uint8_t cur_count;
    uint8_t total_count;
    bool cur_count_handled;
    bool do_debug;

    pthread_mutex_t mtx;
    pthread_cond_t cv;
};
typedef struct common_ctx_t_ common_ctx_t;

struct thread_ctx_t_ {
    uint8_t thread_id;
    common_ctx_t *common_ctx;
};
typedef struct thread_ctx_t_ thread_ctx_t;

static void *
counting_fn(void *arg)
{
    thread_ctx_t *thread_ctx = (thread_ctx_t *) arg;
    common_ctx_t *ctx = thread_ctx->common_ctx;
    const uint8_t id = thread_ctx->thread_id;

    if (ctx->do_debug) {
        printf("%s: starting thread %u\n", __func__, id);
    }

    pthread_mutex_lock(&(ctx->mtx));
    while (true) {
        if (id == ctx->printer_tid) {
            printf("%s: (thread %u) cur_count=%u\n", __func__, id,
                   ctx->cur_count);
            ctx->cur_count_handled = true;
            pthread_cond_signal(&(ctx->cv));
        }
        if (ctx->cur_count >= ctx->total_count) {
            break;
        }
        pthread_cond_wait(&(ctx->cv), &(ctx->mtx));
    }
    pthread_mutex_unlock(&(ctx->mtx));

    if (ctx->do_debug) {
        printf("%s: finishing thread %u\n", __func__, id);
    }
    pthread_exit(NULL);
}

int
main(int argc, char **argv)
{
    static const uint8_t THREAD_COUNT = 5;
    static const uint8_t TOTAL_COUNT = 50;
    static const bool DO_DEBUG = false;

    common_ctx_t ctx = {0};
    pthread_t threads[THREAD_COUNT];
    thread_ctx_t *thread_ctx[THREAD_COUNT] = {0};
    uint8_t *tid;
    uint8_t i;

    ctx.total_count = TOTAL_COUNT;
    ctx.do_debug = DO_DEBUG;

    pthread_mutex_init(&(ctx.mtx), NULL);
    pthread_cond_init(&(ctx.cv), NULL);

    for (i = 0; i < THREAD_COUNT; ++i) {
        thread_ctx[i] = malloc(sizeof(*thread_ctx[i]));
        if (NULL == thread_ctx[i]) {
            printf("%s: allocation failure\n", __func__);
            goto done;
        }
        thread_ctx[i]->thread_id = (i + 1);
        thread_ctx[i]->common_ctx = &ctx;
        pthread_create(&threads[i], NULL, counting_fn, thread_ctx[i]);
    }

    tid = &(ctx.printer_tid);
    pthread_mutex_lock(&(ctx.mtx));
    *tid = 0;
    for (ctx.cur_count = 1; ctx.cur_count <= ctx.total_count;
         ++(ctx.cur_count)) {
        *tid = (*tid >= THREAD_COUNT) ?  1 : (*tid + 1);
        ctx.cur_count_handled = false;
        printf("%s: cur_count=%u\n", __func__, ctx.cur_count);
        pthread_cond_broadcast(&(ctx.cv));
        while (!ctx.cur_count_handled) {
            pthread_cond_wait(&(ctx.cv), &(ctx.mtx));
        }
    }
    pthread_mutex_unlock(&(ctx.mtx));

done:

    for (i = 0; i < THREAD_COUNT; ++i) {
        if (NULL != thread_ctx[i]) {
            pthread_join(threads[i], NULL);
            free(thread_ctx[i]);
            thread_ctx[i] = NULL;
        }
    }

    printf("%s: %u threads finished\n", __func__, THREAD_COUNT);

    pthread_mutex_destroy(&(ctx.mtx));
    pthread_cond_destroy(&(ctx.cv));

    return 0;
}
