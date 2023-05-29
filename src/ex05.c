#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define THREAD_C_NUMBER 1
#define THREAD_D_NUMBER 4

pthread_mutex_t mtx_balance3 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_balance = PTHREAD_COND_INITIALIZER;

/* 1 thread x 10 loops x 80 = 800 */
void* task_credit3(void* args)
{
    int i;
    double *balance = (double*)args;

    for (i=0; i<10; i++) {
        pthread_mutex_lock(&mtx_balance3);
        *balance += 80;
        pthread_mutex_unlock(&mtx_balance3);
        /*
         * we need a broadcast, to comunicate all waiting threads
         * pthread_cond_signal(&cond_balance);
         */
        pthread_cond_broadcast(&cond_balance);
        printf("%ld will put the money!\n", pthread_self());
        sleep(1);
    }

    return NULL;
}

/* 4 thread x 10 loops x 20 = 800 */
void* task_debit3(void* args)
{
    int i;
    double *balance = (double*)args;

    for (i=0; i<10; i++) {
        pthread_mutex_lock(&mtx_balance3);

        while (*balance < 20)
            pthread_cond_wait(&cond_balance, &mtx_balance3);

        *balance -= 20;
        pthread_mutex_unlock(&mtx_balance3);
        printf("%ld will get the money!\n", pthread_self());
    }

    return NULL;
}

void ex05(void)
{
    int i;
    double balance;
    pthread_t th_c[THREAD_C_NUMBER], th_d[THREAD_D_NUMBER];

    balance = 0.0;

    for (i = 0; i < THREAD_C_NUMBER; i++) {
        pthread_create(&th_c[i], NULL, task_credit3, &balance);
    }

    for (i = 0; i < THREAD_D_NUMBER; i++) {
        pthread_create(&th_d[i], NULL, task_debit3, &balance);
    }

    for (i = 0; i < THREAD_C_NUMBER; i++) {
        pthread_join(th_c[i], NULL);
    }

    for (i = 0; i < THREAD_D_NUMBER; i++) {
        pthread_join(th_d[i], NULL);
    }
    printf("final balance :: %.2f\n", balance);
}
