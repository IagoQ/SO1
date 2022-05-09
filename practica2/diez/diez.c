#include "diez.h"

void cond_init(Cond cond){
    
    cond->count = 0;
    pthread_mutex_init(&cond->lock, NULL);
    sem_init(&cond->semaforo, 0, 0);
}

void cond_destroy(Cond cond){

    pthread_mutex_destroy(&cond->lock);
    sem_destroy(&cond->semaforo);
}

void cond_wait(Cond cond, pthread_mutex_t* mutex){
        
    pthread_mutex_unlock(mutex);
    
    pthread_mutex_lock(&cond->lock); // alguien esta esperando
    (cond->count)++;
    pthread_mutex_unlock(&cond->lock);

    sem_wait(&cond->semaforo);

    pthread_mutex_lock(mutex);
}

void cond_signal(Cond cond){
        
    sem_post(&cond->semaforo);

    pthread_mutex_lock(&cond->lock); 
    (cond->count)--;
    pthread_mutex_unlock(&cond->lock);
}

void cond_broadcast(Cond cond){
        
    for(int i = 0; i < cond->count; i++){
        sem_post(&cond->semaforo);

        pthread_mutex_lock(&cond->lock);
        (cond->count)--;
        pthread_mutex_unlock(&cond->lock);
    }
    
}