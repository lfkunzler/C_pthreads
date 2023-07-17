#include "fork.h"
#include <pthread.h>
#include <stdio.h>

typedef enum
{
    ST_THINKING = 0,
    ST_HUNGRY,
    ST_EATING,
} state_e;

#define LEFT(f) (f + PHILOSOPHERS_NUM - 1) % PHILOSOPHERS_NUM
#define RIGHT(f) (f + 1) % PHILOSOPHERS_NUM

static pthread_mutex_t mtx_forks = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cnd_forks[PHILOSOPHERS_NUM];
static state_e st_phi[PHILOSOPHERS_NUM];

static void print_table(void)
{
    printf("|");
    for (int f = 0; f < PHILOSOPHERS_NUM; f++) {
        switch (st_phi[f]) {
        case ST_THINKING:
            printf(" T |");
            break;
        case ST_HUNGRY:
            printf(" H |");
            break;
        case ST_EATING:
            printf(" E |");
            break;
        }
    }
    printf("\n");
}

static void try_to_eat(int phi_id)
{
    const unsigned int left = LEFT(phi_id);
    const unsigned int right = RIGHT(phi_id);

    if (ST_HUNGRY == st_phi[phi_id] && 
            ST_EATING != st_phi[left] && ST_EATING != st_phi[right]) {
        st_phi[phi_id] = ST_EATING;
        pthread_cond_signal(&cnd_forks[phi_id]);
    }
}

void fork_init(void)
{
    pthread_mutex_init(&mtx_forks, NULL);
    for (int i = 0; i < PHILOSOPHERS_NUM; i++) {
        st_phi[i] = ST_THINKING;
        pthread_cond_init(&cnd_forks[i], NULL);
    }
    print_table();
}

void fork_deinit(void)
{
    pthread_mutex_destroy(&mtx_forks);
    for (int i = 0; i < PHILOSOPHERS_NUM; i++) {
        pthread_cond_destroy(&cnd_forks[i]);
    }
    print_table();
}

void fork_take(int phi_id)
{
    pthread_mutex_lock(&mtx_forks);

    printf("%d is hungry\n", phi_id);
    print_table();
    st_phi[phi_id] = ST_HUNGRY;
    try_to_eat(phi_id);

    while (ST_HUNGRY == st_phi[phi_id])
        pthread_cond_wait(&cnd_forks[phi_id], &mtx_forks);

    pthread_mutex_unlock(&mtx_forks);
}

void fork_put(int phi_id)
{
    const unsigned int left = LEFT(phi_id);
    const unsigned int right = RIGHT(phi_id);

    pthread_mutex_lock(&mtx_forks);

    printf(":: %d will drop the fork\n", phi_id);
    st_phi[phi_id] = ST_THINKING;

    try_to_eat(left);
    try_to_eat(right);

    pthread_mutex_unlock(&mtx_forks);
}
