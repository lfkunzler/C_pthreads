#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t mutex_balance = PTHREAD_MUTEX_INITIALIZER;

void* debit(void* args)
{
    int i;
    float new_balance, old_balance;
    float *balance = (float*)args;
    for (i=0; i<100; i++) {
        pthread_mutex_lock(&mutex_balance);
        old_balance = *balance;
        new_balance = old_balance-100;
        printf("D: %.2f -> %.2f | real value is :: %.2f\n", old_balance, new_balance, *balance);
        *balance = new_balance;
        pthread_mutex_unlock(&mutex_balance);
    }
    return NULL;
}

void* credit(void* args)
{
    int i;
    float new_balance, old_balance;
    float *balance = (float*)args;
    for (i=0; i<100; i++) {
        while (pthread_mutex_trylock(&mutex_balance)) {
         /* busy wait */
            printf("Credit task is busy\n");
        }

        old_balance = *balance;
        new_balance = old_balance+100;
        printf("C: %.2f -> %.2f | real value is :: %.2f\n", old_balance, new_balance, *balance);
        *balance = new_balance;

        pthread_mutex_unlock(&mutex_balance);
    }
    return NULL;
}

int ex02(void)
{
    pthread_t th1, th2;
    float balance = 10000.0f;

    pthread_create(&th1, NULL, debit, &balance);
    pthread_create(&th2, NULL, credit, &balance);

    pthread_join(th1, NULL);
    pthread_join(th2, NULL);

    printf("Final balance is %.2f\n", balance);

    return 0;
}
