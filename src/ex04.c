#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#define THREAD_NUMBER   8

pthread_mutex_t mtx_balance2 = PTHREAD_MUTEX_INITIALIZER;

inline double scale_range(double range_min, double range_max, double value)
{
    return range_min + (range_max-range_min)*value;
}

void* task_credit2(void* args)
{
    int i;
    double random_number, credit_value;
    double* balance = (double*)args;
    struct drand48_data drand_buf;

    srand48_r(time(NULL), &drand_buf);

    for (i=0; i<10; i++) {
        drand48_r(&drand_buf, &random_number);
        credit_value = scale_range(10.0, 30.0, random_number);

        pthread_mutex_lock(&mtx_balance2);
        *balance += credit_value;
        printf("+ %.2f in the account\n", credit_value);
        pthread_mutex_unlock(&mtx_balance2);
        usleep(100);
    }
    return NULL;
}

void* task_debit2(void* args)
{
    int i;
    double debit_value;
    double* balance = (double*)args;
    for (i=0; i<10; i++) {
        pthread_mutex_lock(&mtx_balance2);
        debit_value = *balance > 20.0 ? 20.0 : *balance;
        *balance -= debit_value;
        printf("- %.2f in the account\n", debit_value);
        pthread_mutex_unlock(&mtx_balance2);
        usleep(100);
    }
    return NULL;
}

void ex04(void)
{
    int i;
    pthread_t th[THREAD_NUMBER];
    double balance = 0.0;

    for (i=0; i<THREAD_NUMBER/2; i++) {
        pthread_create(&th[i], NULL, task_debit2, &balance);
    }

    for (i=THREAD_NUMBER/2; i<THREAD_NUMBER; i++) {
        pthread_create(&th[i], NULL, task_credit2, &balance);
    }

    for (i=0; i<THREAD_NUMBER; i++) pthread_join(th[i], NULL);

    printf("final balance: %f\n", balance);

}
