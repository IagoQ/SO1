#include "ocho.h"

void channel_init(Channel c){
    // inicializa el mutex
    pthread_mutex_init(&c->lock, NULL);

    // inicializa los semaforos
    sem_init(&c->reader, 0, 0);
    sem_init(&c->writer, 0, 0);
    sem_init(&c->extra, 0, 0);
}

void chan_write(Channel c, int v){

    sem_wait(&c->extra); // espero a que aparezca algun lector
    
    pthread_mutex_lock(&c->lock);
    c->value = v;
    sem_post(&c->writer); // ya escribi
    sem_wait(&c->reader); // espero que lo lean
    pthread_mutex_unlock(&c->lock);
}

int chan_read(Channel c){

    sem_post(&c->extra); // aparce un lector
    
    sem_wait(&c->writer); // espero a que escriban
       
    int v = c->value;
    sem_post(&c->reader); // ya lei
    return v;
}