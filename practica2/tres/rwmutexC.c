#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define M 2
#define N 2
#define ARRLEN 10240

struct linked_sem
{
  sem_t s;
  int type; // 0 = reader, 1 writer
  int subscribers;
  struct linked_sem *next;
};

struct rwmutex
{
  pthread_mutex_t mutex;
  struct linked_sem *current;
  struct linked_sem *last;
};

void rwmutex_init(struct rwmutex *mutex)
{
  pthread_mutex_init(&(mutex->mutex), NULL);
  mutex->current = NULL;
  mutex->last = NULL;
}

void rwmutex_RLock(struct rwmutex *mutex)
{
  pthread_mutex_lock(&(mutex->mutex));

  if (mutex->current == NULL)
  {
    struct linked_sem *c = malloc(sizeof(struct linked_sem));
    c->type = 0;
    c->subscribers = 1;
    c->next = NULL;

    printf("reader got lock: 0 \n");

    mutex->current = c;
    mutex->last = c;

    pthread_mutex_unlock(&(mutex->mutex));
    return;
  }

  // if a reader has the lock and there isnt anyone else waiting
  if (mutex->current == mutex->last && mutex->current->type == 0) {
    mutex->current->subscribers++;
    pthread_mutex_unlock(&(mutex->mutex));
    return;
  }

  // if last is a reader add it to the same "bag" and wait on it
  if (mutex->last->type == 0) {
    mutex->last->subscribers++;
    struct linked_sem *tmp = mutex->last;
    pthread_mutex_unlock(&(mutex->mutex));
    printf("reader waiting lock \n");
    sem_wait(&(tmp->s));
    printf("reader got lock \n");
    return;
  }

  // else add it to the queue and wait on the sem
  struct linked_sem *c = malloc(sizeof(struct linked_sem));
  c->type = 0;
  c->subscribers = 1;

  mutex->last->next = c;
  mutex->last = c;

  pthread_mutex_unlock(&(mutex->mutex));

  sem_wait(&(c->s));

}

void rwmutex_RUnlock(struct rwmutex *mutex)
{

  pthread_mutex_lock(&(mutex->mutex));

  if (mutex->current->type != 0)
  {
    printf("Wrong type for unlock, not reader\n");
    exit(1);
  }

  mutex->current->subscribers--;
  if (mutex->current->subscribers <= 0)
  {
    struct linked_sem *tmp = mutex->current;
    mutex->current = tmp->next;
    free(tmp);

    if (mutex->current != NULL)
    {
      // if its a writer unlock all subscribers
      if (mutex->current->type == 0) {
        for (int i = 0; i < mutex->current->subscribers; i++) {
          sem_post(&(mutex->current->s));
        }
      } else {
        sem_post(&(mutex->current->s));
      }
    }
  }
  pthread_mutex_unlock(&(mutex->mutex));
}

void rwmutex_RWLock(struct rwmutex *mutex)
{
  pthread_mutex_lock(&(mutex->mutex));

  if (mutex->current == NULL)
  {
    struct linked_sem *c = malloc(sizeof(struct linked_sem));
    c->type = 1;
    c->next = NULL;

    mutex->current = c;
    mutex->last = c;

    pthread_mutex_unlock(&(mutex->mutex));
    return;
  }

  // else add it to the queue and wait on the sem
  struct linked_sem *c = malloc(sizeof(struct linked_sem));
  c->type = 1;
  c->next = NULL;

  mutex->last->next = c;
  mutex->last = c;

  pthread_mutex_unlock(&(mutex->mutex));
  sem_wait(&(c->s));
}

void rwmutex_RWUnlock(struct rwmutex *mutex)
{
  pthread_mutex_lock(&(mutex->mutex));

  if (mutex->current->type != 1)
  {
    printf("Wrong type for unlock, not Writer\n");
    exit(1);
  }

  struct linked_sem *tmp = mutex->current;
  mutex->current = tmp->next;
  free(tmp);


  // if the next is a reader unlock all consecutive readers
  if (mutex->current != NULL)
  {
      sem_post(&(mutex->current->s));
    // if its a writer unlock all subscribers
    if (mutex->current->type == 0)
    {
      for (int i = 0; i < mutex->current->subscribers; i++)
      {
        sem_post(&(mutex->current->s));
      }
    }
    else
    {
      sem_post(&(mutex->current->s));
    }
  }

  pthread_mutex_unlock(&(mutex->mutex));
}

int arr[ARRLEN];
struct rwmutex rwm;

void *escritor(void *arg)
{
  int i;
  int num = arg - (void *)0;
  while (1)
  {
    // sleep(random() % 2);
    rwmutex_RWLock(&rwm);
    printf("Escritor %d escribiendo\n", num);
    for (i = 0; i < ARRLEN; i++)
    {
      arr[i] = num;
    }
    rwmutex_RWUnlock(&rwm);
  }
  return NULL;
}

void *lector(void *arg)
{
  int v, i;
  int num = arg - (void *)0;
  while (1)
  {
    // sleep(random() % 2);
    rwmutex_RLock(&rwm);
    v = arr[0];
    for (i = 1; i < ARRLEN; i++)
    {
      if (arr[i] != v)
        break;
    }
    if (i < ARRLEN)
    {
      printf("Lector %d, error de lectura\n", num);
      exit(1);
    }
    else
    {
      printf("Lector %d, dato %d\n", num, v);
    }
    rwmutex_RUnlock(&rwm);
  }
  return NULL;
}

int main()
{
  pthread_t lectores[M], escritores[N];
  int i;
  for (i = 0; i < M; i++)
    pthread_create(&lectores[i], NULL, lector, i + (void *)0);
  for (i = 0; i < N; i++)
    pthread_create(&escritores[i], NULL, escritor, i + (void *)0);
  pthread_join(lectores[0], NULL);
  return 0;
}
