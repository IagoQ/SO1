
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct rwmutex{
  sem_t writeSem;

  pthread_mutex_t mutex;

  pthread_cond_t writersFinished;

  int readers;
  int writers;
};

void rwmutex_init(struct rwmutex *mutex);

void rwmutex_RLock(struct rwmutex *mutex);

void rwmutex_RUnlock(struct rwmutex *mutex);

void rwmutex_RWLock(struct rwmutex *mutex);

void rwmutex_RWUnlock(struct rwmutex *mutex);

