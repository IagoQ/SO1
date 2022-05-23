#pragma once

#include <pthread.h>
#include <semaphore.h>

struct _Channel {

    int value;
    
    /* evita que otro escritor escriba antes de 
    que se lea lo que escribio el escritor acutal */
    pthread_mutex_t lock; 
    
    /* indica si se escribió algun valor */
    sem_t writer;
    
    /* indica si se leyó el valor escrito */
    sem_t reader;
    
};

typedef struct _Channel* Channel;

void channel_init(Channel c);

void chan_write(Channel c, int v);

int chan_read(Channel c);
