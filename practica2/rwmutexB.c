#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#define M 50
#define N 5    
#define ARRLEN 10240

struct rwmutex{
  sem_t writeSem;

  pthread_mutex_t mutex;

  pthread_cond_t writersFinished;

  int readers;
  int writers;
};


void rwmutex_init(struct rwmutex *mutex){
  mutex->readers = 0;
  mutex->writers = 0;
  
  // mutex->writersFinished = PTHREAD_COND_INITIALIZER;;
  pthread_mutex_init(&(mutex->mutex),NULL);
  sem_post(&(mutex->writeSem));
}

void rwmutex_RLock(struct rwmutex *mutex){

	pthread_mutex_lock(&(mutex->mutex));

  while(mutex->writers > 0){
    pthread_cond_wait(&(mutex->writersFinished), &(mutex->mutex));
  }

	if(mutex->readers == 0){
		sem_wait(&(mutex->writeSem));
	} 

	mutex->readers++; 

	pthread_mutex_unlock(&(mutex->mutex));
}

void rwmutex_RUnlock(struct rwmutex *mutex){

	pthread_mutex_lock(&(mutex->mutex));

	mutex->readers--; 

	if(mutex->readers == 0){
		sem_post(&(mutex->writeSem));
	}

	pthread_mutex_unlock(&(mutex->mutex));
}

void rwmutex_RWLock(struct rwmutex *mutex){
	pthread_mutex_lock(&(mutex->mutex));
  mutex->writers++;
	pthread_mutex_unlock(&(mutex->mutex));

	sem_wait(&(mutex->writeSem));
}

void rwmutex_RWUnlock(struct rwmutex *mutex){
	pthread_mutex_lock(&(mutex->mutex));
  mutex->writers--;

  if(mutex->writers == 0){
    pthread_cond_broadcast(&(mutex->writersFinished));
  }
	sem_post(&(mutex->writeSem));

	pthread_mutex_unlock(&(mutex->mutex));
}


int arr[ARRLEN];
struct rwmutex rwm;

void * escritor(void *arg){ 
	int i;
	int num = arg - (void*)0;
	while (1) {
    sleep(random() % 3);
    rwmutex_RWLock(&rwm);
		printf("Escritor %d escribiendo\n", num);
    for (i = 0; i < ARRLEN; i++){
      arr[i] = num;
    }
    rwmutex_RWUnlock(&rwm);
  }
	return NULL;
}

void * lector(void *arg){
	int v, i;
	int num = arg - (void*)0;
	while (1) {
		sleep(random() % 3);
    rwmutex_RLock(&rwm);
    // dada la cantidad de lectores y la duracion artificial de la lectura
    // si el mutex no le da preferencia a los writes nunca habria un write
    sleep(1);
		v = arr[0];
		for (i = 1; i < ARRLEN; i++) {
			if (arr[i] != v)
				break;
		}
    rwmutex_RUnlock(&rwm);
		if (i < ARRLEN){
			printf("Lector %d, error de lectura\n", num);
      exit(1);
    } else{
			printf("Lector %d, dato %d\n", num, v);
    }
	}
	return NULL;
}

int main(){
	pthread_t lectores[M], escritores[N];
  rwmutex_init(&rwm);
	int i;
	for (i = 0; i < M; i++)
		pthread_create(&lectores[i], NULL, lector, i + (void*)0);
	for (i = 0; i < N; i++)
		pthread_create(&escritores[i], NULL, escritor, i + (void*)0);
	pthread_join(lectores[0], NULL);
	/* Espera para siempre */
	return 0;
}
