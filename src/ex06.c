#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define THREAD_NUMBER 5

unsigned int counter_A = 0;
pthread_mutex_t mtx_counter_A = PTHREAD_MUTEX_INITIALIZER;

unsigned int counter_B = 0;
pthread_mutex_t mtx_counter_B = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t counter_cond_AB = PTHREAD_COND_INITIALIZER;

unsigned int counter_C = 0;
pthread_mutex_t mtx_counter_C = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t counter_cond_BC = PTHREAD_COND_INITIALIZER;


void* task_1st(void* args)
{
    int i;
    (void)args;

    for (i=0; i<10; i++) {
        pthread_mutex_lock(&mtx_counter_A);
        counter_A++;
        pthread_cond_broadcast(&counter_cond_AB);
        pthread_mutex_unlock(&mtx_counter_A);
        sleep(1);
    }

    return NULL;
}

void* task_2nd(void* args)
{
    int i;
    (void)args;

    for (i=0; i<10; i++) {
        pthread_mutex_lock(&mtx_counter_A);

        while (!counter_A) {
            pthread_cond_wait(&counter_cond_AB, &mtx_counter_A);
        }
        counter_A--;

        pthread_mutex_unlock(&mtx_counter_A);

        sleep(1);

        pthread_mutex_lock(&mtx_counter_B);
        counter_B++;
        pthread_cond_broadcast(&counter_cond_BC);
        pthread_mutex_unlock(&mtx_counter_B);
    }

    return NULL;
}

void* task_3rd(void* args)
{
    int i;
    (void)args;

    for (i=0; i<10; i++) {
        pthread_mutex_lock(&mtx_counter_B);
        while (!counter_B) pthread_cond_wait(&counter_cond_BC, &mtx_counter_B);
        counter_B--;
        pthread_mutex_unlock(&mtx_counter_B);

        sleep(1);

        pthread_mutex_lock(&mtx_counter_C);
        counter_C++;
        pthread_mutex_unlock(&mtx_counter_C);
    }
    return NULL;
}

void ex06(void)
{
    int i;
    pthread_t th_1st[THREAD_NUMBER], th_2nd[THREAD_NUMBER], th_3rd[THREAD_NUMBER];

    for (i = 0; i < THREAD_NUMBER; i++) {
        pthread_create(&th_1st[i], NULL, task_1st, NULL);
        pthread_create(&th_2nd[i], NULL, task_2nd, NULL);
        pthread_create(&th_3rd[i], NULL, task_3rd, NULL);
    }

    for (i = 0; i < THREAD_NUMBER; i++) {
        pthread_join(th_1st[i], NULL);
        pthread_join(th_2nd[i], NULL);
        pthread_join(th_3rd[i], NULL);
    }

    printf("counter A :: %d\n", counter_A);
    printf("counter B :: %d\n", counter_B);
    printf("counter C :: %d\n", counter_C);
}
