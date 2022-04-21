#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#define M 5
#define N 5
#define ARRLEN 10240

struct rwmutex{
sem_t writeSem;
pthread_mutex_t readersMutex;
int readers;
};

void rwmutex_RLock(struct rwmutex *mutex){
	pthread_mutex_lock(&(mutex->readersMutex));
	if(mutex->readers == 0){
		sem_wait(&(mutex->writeSem));
	} 
	mutex->readers++; 
	pthread_mutex_unlock(&(mutex->readersMutex));
}

void rwmutex_Runlock(struct rwmutex *mutex){
	pthread_mutex_lock(&(mutex->readersMutex));
	mutex->readers--; 
	if(mutex->readers == 0){
		sem_post(&(mutex->writeSem));
	}
	pthread_mutex_unlock(&(mutex->readersMutex));
}

void rwmutex_RWLock(struct rwmutex *mutex){
	sem_wait(&(mutex->writeSem));
}

void rwmutex_RWunlock(struct rwmutex *mutex){
	sem_post(&(mutex->writeSem));
}


int arr[ARRLEN];
void * escritor(void *arg){ 
	int i;
	int num = arg - (void*)0;
	while (1) {sleep(random() % 3);
		printf("Escritor %d escribiendo\n", num);
	for (i = 0; i < ARRLEN; i++)
		arr[i] = num;
	}
	return NULL;
}
void * lector(void *arg){
	int v, i;
	int num = arg - (void*)0;
	while (1) {
		sleep(random() % 3);
		v = arr[0];
		for (i = 1; i < ARRLEN; i++) {
			if (arr[i] != v)
				break;
		}
		if (i < ARRLEN)
			printf("Lector %d, error de lectura\n", num);
		else
			printf("Lector %d, dato %d\n", num, v);
	}
	return NULL;
}
int main(){
	pthread_t lectores[M], escritores[N];
	int i;
	for (i = 0; i < M; i++)
		pthread_create(&lectores[i], NULL, lector, i + (void*)0);
	for (i = 0; i < N; i++)
		pthread_create(&escritores[i], NULL, escritor, i + (void*)0);
	pthread_join(lectores[0], NULL);
	/* Espera para siempre */
	return 0;
}
