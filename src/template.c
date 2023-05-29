#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define THREAD_NUMBER 4

void* task_debitX(void* args)
{
    double *balance = (double*)args;
    (void)balance;

    return NULL;
}

void* task_creditX(void* args)
{
    double *balance = (double*)args;
    (void)balance;

    return NULL;
}

void exXY(void)
{
    int i;
    double balance;
    pthread_t th_c[THREAD_NUMBER], th_d[THREAD_NUMBER];

    balance = 0.0;

    for (i = 0; i < THREAD_NUMBER; i++) {
        pthread_create(&th_c[i], NULL, task_creditX, &balance);
        pthread_create(&th_d[i], NULL, task_debitX, &balance);
    }

    for (i = 0; i < THREAD_NUMBER; i++) {
        pthread_join(th_c[i], NULL);
        pthread_join(th_d[i], NULL);
    }
}
