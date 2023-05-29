#include <stdio.h>
#include <pthread.h>
#include <threads.h>
#include <unistd.h>
#define EXP_NUMBER  4

pthread_t t1;
pthread_t t2;

typedef struct {
    int target;
    char *name;
} param_t;

typedef struct {
    int target;
    void* (*func)(void*);
    pthread_t thread;
} experiment_t;

void* code1(void* args)
{
    int i;

    (void)args;

    for (i=0; i<3; i++) {
        printf("task thread 1 :: %d\n", i);
        sleep(1);
    }
    return NULL;
}

void* code2(void* args)
{
    int i;
    const int target = *(int*)args;
    for (i=0; i<target; i++) {
        printf("code 2 :: %d _ target :: %d\n", i, target);
        sleep(1);
    }
    return NULL;
}

void* code3(void* args)
{
    int i;
    param_t aux_param;

    aux_param = *(param_t*)args;
    for (i=0; i<aux_param.target; i++) {
        printf("Greetings from %s :: %i\n", aux_param.name, i);
        sleep(1);
    }
    return NULL;
}

int ex01(void)
{
    int i;
    param_t param1;
    int param2;
    experiment_t exp[EXP_NUMBER];

    for (i=0; i<EXP_NUMBER; i++) {
        exp[i].target = i+5;
        exp[i].func = &code2;
        pthread_create(&(exp[i].thread), NULL, exp[i].func, &(exp[i].target));
    }

    pthread_create(&t1, NULL, code1, NULL);

    param2 = 3;
    pthread_create(&t2, NULL, code2, &param2);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    param1.name = "Felipe Kunzler";
    param1.target = 4;

    pthread_create(&t1, NULL, code3, &param1);

    pthread_join(t1, NULL);

    for (i=0; i<EXP_NUMBER; i++) pthread_join(exp[i].thread, NULL);

    printf("end\n");
    return 0;
}
