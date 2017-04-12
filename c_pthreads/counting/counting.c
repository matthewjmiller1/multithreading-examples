#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

static pthread_mutex_t mtx;
static pthread_cond_t cv;

static uint8_t cur_count = 0;
static const uint8_t TOTAL_COUNT = 50;
static uint8_t thread_ready_count = 0;

static void *
counting_fn(void *ctx)
{
    uint8_t id = (uint8_t) ctx;

    printf("%s: starting thread %u\n", __func__, id);

    pthread_mutex_lock(&mtx);
    ++thread_ready_count;
    pthread_cond_broadcast(&cv);
    pthread_mutex_unlock(&mtx);

    pthread_mutex_lock(&mtx);
    while (cur_count <= TOTAL_COUNT) {
        if ((1 == id) && (0 != (cur_count % 2))) {
            printf("%s: (thread %u) cur_count=%u\n", __func__, id, cur_count);
        } else if ((2 == id) && (0 == (cur_count % 2))) {
            printf("%s: (thread %u) cur_count=%u\n", __func__, id, cur_count);
        }
        pthread_cond_wait(&cv, &mtx);
    }
    pthread_mutex_unlock(&mtx);

    printf("%s: finishing thread %u\n", __func__, id);
    pthread_exit(NULL);
}

int
main(int argc, char **argv)
{
    static const uint8_t THREAD_COUNT = 2;
    pthread_t threads[THREAD_COUNT];
    bool do_count_loop = true;
    uint8_t i;

    pthread_mutex_init(&mtx, NULL);
    pthread_cond_init(&cv, NULL);

    for (i = 0; i < THREAD_COUNT; ++i) {
        pthread_create(&threads[i], NULL, counting_fn,
                       (void *) (uintptr_t) (i + 1));
    }

    pthread_mutex_lock(&mtx);
    while (thread_ready_count < THREAD_COUNT) {
        pthread_cond_wait(&cv, &mtx);
    }
    pthread_mutex_unlock(&mtx);

    while (do_count_loop) {
        pthread_mutex_lock(&mtx);
        ++cur_count;
        printf("%s: cur_count=%u\n", __func__, cur_count);
        do_count_loop = (cur_count <= TOTAL_COUNT);
        pthread_cond_broadcast(&cv);
        pthread_mutex_unlock(&mtx);
    }

    for (i = 0; i < THREAD_COUNT; ++i) {
        pthread_join(threads[i], NULL);
    }

    printf("%s: %u threads finished\n", __func__, THREAD_COUNT);

    pthread_mutex_destroy(&mtx);
    pthread_cond_destroy(&cv);

    return 0;
}
