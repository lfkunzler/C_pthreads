# C_pthreads
C with pthread exercises

## EX 04:
In this exercise we have 8 threads, 4 for depositing random values and 4 to withdrawal operations.
The max withdrawal is 20.0

## EX 05:
Using pthread_cond_t
- pthread_cond_signal to sinalize to one thread about something that it was waiting for
- pthread_cond_broadcast to sinalize to all threads that were waiting for

## EX 06:
Prod line simulator with 3 stages
- 1st stage: 5 threads producing A
- 2nd stage: 5 threads consuming A and generating B
- 3rd stage: 5 threads consuming B and generating C

Instructions: Keep 3 counters, c_A, c_B and c_C

## PRJ1
This project meant to put in practice the concept of monitors solving the problem of the philosophers dinner.

## EX 07:
This exercise meant to study the issue of the readers and writers.
I also did a quick review on basic operations with linked lists.

## Notes about callbacks
```
#include <stdio.h>

int sum(int a, int b)
{
    return a+b;
}

int sub(int a, int b)
{
    return a-b;
}

int calc(int (*f)(int, int), int a, int b)
{
    return f(a, b);
}

int main(void)
{
    printf("sum 15 + 16 == %d\n", calc(sum, 15, 16));
    printf("sub 15 - 16 == %d\n", calc(sub, 15, 16));

    return 0;
}
```

## EX 08
Playing with signal()