#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

static pthread_mutex_t mtx;
static pthread_cond_t cv;

static uint8_t printer_tid = 0;
static uint8_t cur_count = 0;
static bool cur_count_handled = false;
static const uint8_t TOTAL_COUNT = 50;

static const bool DO_DEBUG = false;

static void *
counting_fn(void *ctx)
{
    uint8_t id = (uint8_t) ctx;

    if (DO_DEBUG) {
        printf("%s: starting thread %u\n", __func__, id);
    }

    pthread_mutex_lock(&mtx);
    while (true) {
        if (id == printer_tid) {
            printf("%s: (thread %u) cur_count=%u\n", __func__, id, cur_count);
            cur_count_handled = true;
            pthread_cond_signal(&cv);
        }
        if (cur_count >= TOTAL_COUNT) {
            break;
        }
        pthread_cond_wait(&cv, &mtx);
    }
    pthread_mutex_unlock(&mtx);

    if (DO_DEBUG) {
        printf("%s: finishing thread %u\n", __func__, id);
    }
    pthread_exit(NULL);
}

int
main(int argc, char **argv)
{
    static const uint8_t THREAD_COUNT = 5;
    pthread_t threads[THREAD_COUNT];
    uint8_t i;

    pthread_mutex_init(&mtx, NULL);
    pthread_cond_init(&cv, NULL);

    for (i = 0; i < THREAD_COUNT; ++i) {
        pthread_create(&threads[i], NULL, counting_fn,
                       (void *) (uintptr_t) (i + 1));
    }

    pthread_mutex_lock(&mtx);
    printer_tid = 0;
    for (cur_count = 1; cur_count <= TOTAL_COUNT; ++cur_count) {
        printer_tid = (printer_tid >= THREAD_COUNT) ? 1 : (printer_tid + 1);
        cur_count_handled = false;
        printf("%s: cur_count=%u\n", __func__, cur_count);
        pthread_cond_broadcast(&cv);
        while (!cur_count_handled) {
            pthread_cond_wait(&cv, &mtx);
        }
    }
    pthread_mutex_unlock(&mtx);

    for (i = 0; i < THREAD_COUNT; ++i) {
        pthread_join(threads[i], NULL);
    }

    printf("%s: %u threads finished\n", __func__, THREAD_COUNT);

    pthread_mutex_destroy(&mtx);
    pthread_cond_destroy(&cv);

    return 0;
}
