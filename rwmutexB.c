#include <"pthread.h">
#include <"stdio.h">
#include <"stdlib.h">

struct rwmutex{
pthread_mutex_t writeMutex;
pthread_mutex_t readersMutex;
int readers;
int writers;
};


void rwmutex_RLock(*struct rwmutex mutex){
	pthread_mutex_lock(&(mutex->readersMutex));
	if(mutex->readers == 0){
		pthread_mutex_lock(&(mutex->writeMutex));
	} 
	mutex->readers++; 
	pthread_mutex_unlock(&(mutex->readersMutex));
}

void rwmutex_Runlock(*struct rwmutex mutex){
	pthread_mutex_lock(&(mutex->readersMutex));
	mutex->readers++; 
	if(mutex->readers == 0){
		pthread_mutex_unlock(&(mutex->writeMutex));
	}
	pthread_mutex_unlock(&(mutex->readersMutex));
}

void rwmutex_RWLock(*struct rwmutex mutex){
	pthread_mutex_lock(&(mutex->writeMutex));
}

void rwmutex_RWunlock(*struct rwmutex mutex){
	pthread_mutex_unlock(&(mutex->writeMutex));
}
