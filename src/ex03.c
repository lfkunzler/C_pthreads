#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t mtx_balance = PTHREAD_MUTEX_INITIALIZER;

double interest_receive(const double *balance, float percent)
{
    double value;

    pthread_mutex_lock(&mtx_balance);
    printf("locked interest_receive\n");
    value = *balance * (percent/100.0f);
    pthread_mutex_unlock(&mtx_balance);

    return value;
}

void *task_debit(void *args)
{
    int i;
    double current_balance;
    double *balance = (double*)args;

    for (i=0; i<20; i++) {
        pthread_mutex_lock(&mtx_balance);
        printf("locked task_debit\n");
        current_balance = *balance;
        current_balance += 100;
        *balance = current_balance;
        pthread_mutex_unlock(&mtx_balance);
        printf("unlocked task_debit\n");
        sleep(1);
    }
    return NULL;
}

void *task_interest(void *args)
{
    int i;
    double *balance = (double*)args;

    for (i=0; i<10; i++) {
        pthread_mutex_lock(&mtx_balance);
        printf("locked task_interest\n");
        *balance += interest_receive(balance, 5);
        pthread_mutex_unlock(&mtx_balance);
        printf("unlocked task_interest\n");
        sleep(2);
    }
    return NULL;
}

void mtx_init_recursive(pthread_mutex_t *mtx)
{
    pthread_mutexattr_t mtx_attr;

    pthread_mutexattr_init(&mtx_attr);
    pthread_mutexattr_settype(&mtx_attr, PTHREAD_MUTEX_RECURSIVE_NP);
    pthread_mutex_init(mtx, &mtx_attr);
    pthread_mutexattr_destroy(&mtx_attr);
}

void ex03(void)
{
    pthread_t t1, t2;
    double balance = 10000.00;

    mtx_init_recursive(&mtx_balance);

    pthread_create(&t1, NULL, task_debit, &balance);
    pthread_create(&t2, NULL, task_interest, &balance);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
}
