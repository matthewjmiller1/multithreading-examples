#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

/*
 * TODO:
 * - Producer thread increments widgets by N, waits for all to be handled.
 * - M consumer threads consume X widgets as long as some are available.
 */

static pthread_mutex_t mtx;
static pthread_cond_t cv;

static uint8_t cur_count = 0;
static uint8_t total_count = 0;
static const uint8_t TOTAL_COUNT_THRESHOLD = 100;

static void *
consumer_fn(void *ctx)
{
    uint8_t id = (uint8_t) ctx;

    printf("%s: starting thread %u\n", __func__, id);

    pthread_mutex_lock(&mtx);
    while ((total_count <= TOTAL_COUNT_THRESHOLD) || (0 != cur_count)) {
        while ((0 == cur_count) && (total_count <= TOTAL_COUNT_THRESHOLD)) {
            pthread_cond_wait(&cv, &mtx);
        }

        if (0 != cur_count) {
            --cur_count;
            printf("%s: thread %u consumed one, cur_count=%u (total=%u)\n",
                   __func__, id, cur_count, total_count);
            pthread_cond_broadcast(&cv);
            pthread_mutex_unlock(&mtx);
            pthread_mutex_lock(&mtx);
        }
    }
    pthread_mutex_unlock(&mtx);

    printf("%s: finishing thread %u\n", __func__, id);

    pthread_exit(NULL);
}

static void *
producer_fn(void *ctx)
{
    uint8_t id = (uint8_t) ctx;
    uint8_t count;
    static const uint8_t MAX_PER_RND = 10;

    printf("%s: starting thread %u\n", __func__, id);

    pthread_mutex_lock(&mtx);
    while (total_count <= TOTAL_COUNT_THRESHOLD) {
        if (0 == cur_count) {
            count = (rand() % MAX_PER_RND);
            cur_count += count;
            total_count += cur_count;

            printf("%s: thread %u produced %u (total=%u)\n", __func__, id,
                   count, total_count);

            pthread_cond_broadcast(&cv);
        }

        while (cur_count > 0) {
            pthread_cond_wait(&cv, &mtx);
        }
    }
    pthread_mutex_unlock(&mtx);

    printf("%s: finishing thread %u\n", __func__, id);

    pthread_exit(NULL);
}

int
main(int argc, char **argv)
{
    static const uint8_t PRODUCER_THREAD_COUNT = 5;
    static const uint8_t CONSUMER_THREAD_COUNT = 10;
    pthread_t producer_threads[PRODUCER_THREAD_COUNT];
    pthread_t consumer_threads[CONSUMER_THREAD_COUNT];
    uint8_t i;

    srand(42);

    pthread_mutex_init(&mtx, NULL);
    pthread_cond_init(&cv, NULL);

    for (i = 0; i < PRODUCER_THREAD_COUNT; ++i) {
        pthread_create(&producer_threads[i], NULL, producer_fn,
                       (void *) (uintptr_t) (i + 1));
    }

    for (i = 0; i < CONSUMER_THREAD_COUNT; ++i) {
        pthread_create(&consumer_threads[i], NULL, consumer_fn,
                       (void *) (uintptr_t) (i + 1));
    }

    for (i = 0; i < PRODUCER_THREAD_COUNT; ++i) {
        pthread_join(producer_threads[i], NULL);
    }

    printf("%s: %u producer threads finished\n", __func__,
           PRODUCER_THREAD_COUNT);

    for (i = 0; i < CONSUMER_THREAD_COUNT; ++i) {
        pthread_join(consumer_threads[i], NULL);
    }

    printf("%s: %u consumer threads finished\n", __func__,
           CONSUMER_THREAD_COUNT);

    printf("%s: total=%u, cur_count=%u\n", __func__, total_count, cur_count);

    pthread_mutex_destroy(&mtx);
    pthread_cond_destroy(&cv);

    return 0;
}
