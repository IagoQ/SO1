#pragma once

#include <pthread.h>
#include <semaphore.h>

struct _Cond {
    
    /* cantidad de procesos esperando */
    int count;
    
    /* evita que se pierda la cuenta de procesos esperando */
    pthread_mutex_t lock;

    /* indica que ocurrió la condición */
    sem_t semaforo;
};

typedef struct _Cond* Cond;

void cond_init(Cond cond);

void cond_destroy(Cond cond);

void cond_wait(Cond cond, pthread_mutex_t* mutex);

void cond_signal(Cond cond);

void cond_broadcast(Cond cond);
