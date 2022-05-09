#pragma once

#include <pthread.h>
#include <semaphore.h>

struct _Cond {
    int count;
    pthread_mutex_t lock;
    sem_t semaforo;
};

typedef struct _Cond* Cond;

void cond_init(Cond cond);

void cond_destroy(Cond cond);

void cond_wait(Cond cond, pthread_mutex_t* mutex);

void cond_signal(Cond cond);

void cond_broadcast(Cond cond);