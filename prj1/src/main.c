#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "fork.h"

static void *philosopher_life(void *args)
{
    int philosopher_id = *(int*)args;
    printf("hi phi_id :: %d\n", philosopher_id);
    for (int i=0; i<10; i++) {
        sleep(1); // think
        fork_take(philosopher_id);
        sleep(1); // eat
        fork_put(philosopher_id);
    }
    return NULL;
}

int main(void)
{
    int philosopher_ids[PHILOSOPHERS_NUM] = {0};
    pthread_t pth_philosophers[PHILOSOPHERS_NUM] = {0};
    
    fork_init();

    for (int i=0; i<PHILOSOPHERS_NUM; i++) {
        philosopher_ids[i] = i;
        pthread_create(&pth_philosophers[i], NULL, philosopher_life, &philosopher_ids[i]);
    }

    for (int i=0; i<PHILOSOPHERS_NUM; i++) {
        pthread_join(pth_philosophers[i], NULL);
    }

    fork_deinit();
    
    return 0;
}
