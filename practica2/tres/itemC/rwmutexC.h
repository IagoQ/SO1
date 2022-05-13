
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct linked_sem{
  sem_t s;
  int type; // 0 = reader, 1 writer
  int subscribers;
  struct linked_sem *next;
};

struct rwmutex{
  pthread_mutex_t mutex;
  struct linked_sem *current;
  struct linked_sem *last;
};

void rwmutex_init(struct rwmutex *mutex);

void rwmutex_RLock(struct rwmutex *mutex);

void rwmutex_RUnlock(struct rwmutex *mutex);

void rwmutex_RWLock(struct rwmutex *mutex);

void rwmutex_RWUnlock(struct rwmutex *mutex);

