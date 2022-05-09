#pragma once


#include <pthread.h>
#include <semaphore.h>

struct _Channel {
    int value;
    pthread_mutex_t lock;
    sem_t reader;
    sem_t writer;
    sem_t extra;
};

typedef struct _Channel* Channel;

void channel_init(Channel c);

void chan_write(Channel c, int v);

int chan_read(Channel c);