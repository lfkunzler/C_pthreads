/*
 * This exercise means to practice the usage of signals
 * https://www.man7.org/linux/man-pages/man7/signal.7.html
 * Signal() is the "old way" to handle with signals
 * nowadays we use sigaction
 * Using Signal() we can define how it will be handled, with:
 *  - SIG_IGN (ignore)
 *  - SIG_DFL (default behavior)
 *  - function pointer to the handler
 * signals are able to wake up the app from sleep()
 * signals are meant to be used with processes so using them in the context of
 *  threads can be a bit tricky
*/

#include <stdio.h>
#include <execinfo.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#define THREAD_NUM 3

static void capture_ctrlc(int signal)
{
    printf("signal %d | <C-c> pressed! thread id :: %ld\n", signal, pthread_self());
}

static void capture_own_signal(int signal)
{
    printf("signal %d | Own Signal | id :: %ld\n", signal, pthread_self());
}

static void capture_mem_error(int signal)
{
    printf("signal %d | SEG FAULT! thread id :: %ld\n", signal, pthread_self());
    void *callstack[128];
    int frames = backtrace(callstack, sizeof(callstack) / sizeof(void *));
    char **strs = backtrace_symbols(callstack, frames);
    
    if (strs == NULL) {
        perror("backtrace_symbols");
        exit(EXIT_FAILURE);
    }

    printf("Invalid access to memory, printing callstack:\n");
    for (int i = 0; i < frames; i++) {
        printf("%s\n", strs[i]);
    }

    free(strs);
    exit(EXIT_FAILURE);
}

int sigaction_init(struct sigaction* action, void (*f)(int signal), int signal)
{
    if (action == NULL) return -1;
    
    action->sa_handler = f;
    sigemptyset(&(action->sa_mask));
    action->sa_flags = SA_RESTART; // if signal occurs while read(), write() or open(), function will restart
    if (sigaction(signal, action, NULL) < 0) {
        printf("unable to set sigaction\n");
        return -2;
    } 

    return 0;
}

static void* pth_fn(void* args)
{
    (void)args;

    for (int i = 0; i < 10; i++) {
        printf("thread sleeping | id :: %ld\n", pthread_self());
        sleep(1);
        printf("thread woke up  | id :: %ld\n", pthread_self());
    }

    return NULL;
}

int ex08(void)
{
    if (signal(SIGINT, capture_ctrlc) == SIG_ERR) return -1;
    if (signal(SIGUSR1, capture_own_signal) == SIG_ERR) return -2;
    if (signal(SIGSEGV, capture_mem_error) == SIG_ERR) return -3;

    printf("will sleep\n");
    sleep(10);
    printf("woke up\n");
    printf("press something:\n");
    getchar();

    kill(getpid(), SIGUSR1); // send a signal to the app
    // int* pInv = NULL;
    // *pInv = 10; // generates a segfault

    // restablish the default behavior of signals:
    if (signal(SIGINT, SIG_DFL) == SIG_ERR) return -1;
    if (signal(SIGUSR1, SIG_DFL) == SIG_ERR) return -2;
    if (signal(SIGSEGV, SIG_DFL) == SIG_ERR) return -3;

    // using sigaction()
    struct sigaction act_1, act_2, act_3;

    sigaction_init(&act_1, capture_ctrlc, SIGINT);
    sigaction_init(&act_2, capture_own_signal, SIGUSR1);
    sigaction_init(&act_3, capture_mem_error, SIGSEGV);

    printf("will sleep\n");
    sleep(10);
    printf("woke up\n");
    printf("press something:\n");
    getchar();

    kill(getpid(), SIGUSR1); // send a signal to the app
    // int* pInv = NULL;
    // *pInv = 10;

    printf("MAIN task id :: %ld\n", pthread_self());
    pthread_t pth[THREAD_NUM];
    for (int i = 0; i < THREAD_NUM; i++) {
        // threads will inherit behavior related to signals of main() at this point
        // any change after thread creation will not be applied to others
        pthread_create(pth+i, NULL, pth_fn, NULL);
    }
    // by default, signals are handled by main thread
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    pthread_sigmask(SIG_BLOCK, &mask, NULL); // main() thread will not handle SIGINT

    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_join(pth[i], NULL);
    }

    return 0;
}