#include <pthread.h>
#include <stdio.h>
#include <stdint.h>

/*
 * TODO:
 * - Producer thread increments widgets by N, waits for all to be handled.
 * - M consumer threads consume X widgets as long as some are available.
 */

static pthread_mutex_t mtx;
static pthread_cond_t cv;

static void *
consumer_fn(void *ctx)
{
    uint8_t id = (uint8_t) ctx;

    printf("%s: starting thread %u\n", __func__, id);

    pthread_exit(NULL);
}

static void *
producer_fn(void *ctx)
{
    uint8_t id = (uint8_t) ctx;

    printf("%s: starting thread %u\n", __func__, id);

    pthread_exit(NULL);
}

int
main(int argc, char **argv)
{
    static const uint8_t PRODUCER_TREAD_COUNT = 5;
    static const uint8_t CONSUMER_TREAD_COUNT = 10;
    pthread_t producer_threads[PRODUCER_TREAD_COUNT];
    pthread_t consumer_threads[CONSUMER_TREAD_COUNT];
    uint8_t i;

    pthread_mutex_init(&mtx, NULL);
    pthread_cond_init(&cv, NULL);

    for (i = 0; i < PRODUCER_TREAD_COUNT; ++i) {
        pthread_create(&producer_threads[i], NULL, producer_fn,
                       (void *) (uintptr_t) (i + 1));
    }

    for (i = 0; i < CONSUMER_TREAD_COUNT; ++i) {
        pthread_create(&consumer_threads[i], NULL, consumer_fn,
                       (void *) (uintptr_t) (i + 1));
    }

    for (i = 0; i < PRODUCER_TREAD_COUNT; ++i) {
        pthread_join(producer_threads[i], NULL);
    }

    printf("%s: %u producer threads finished\n", __func__,
           PRODUCER_TREAD_COUNT);

    for (i = 0; i < CONSUMER_TREAD_COUNT; ++i) {
        pthread_join(consumer_threads[i], NULL);
    }

    printf("%s: %u consumer threads finished\n", __func__,
           CONSUMER_TREAD_COUNT);

    pthread_mutex_destroy(&mtx);
    pthread_cond_destroy(&cv);

    return 0;
}
