#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

sem_t tabaco, papel, fosforos, otra_vez;

int tabacoAmount = 0,papelAmount = 0,fosforosAmount = 0;
pthread_mutex_t recursosMutex;
pthread_cond_t nuevosRecursos;

void agente()
{
	for (;;)
	{
		int caso = random() % 3;
		sem_wait(&otra_vez);
		switch (caso)
		{
		case 0:
			sem_post(&tabaco);
			sem_post(&papel);
			break;
		case 1:
			sem_post(&fosforos);
			sem_post(&tabaco);
			break;
		case 2:
			sem_post(&papel);
			sem_post(&fosforos);
			break;
		}
	}
}
void fumar(int fumador)
{
	printf("Fumador␣ %d:␣Puf!␣Puf!␣Puf!\n", fumador);
	sleep(1);
}
void *fumador1(void *arg)
{
	for (;;)
	{
		pthread_mutex_lock(&recursosMutex);
		if(fosforosAmount && tabacoAmount){
			fosforosAmount--;
			tabacoAmount--;
			fumar(1);
			sem_post(&otra_vez);
		} else {
			pthread_cond_wait(&nuevosRecursos, &recursosMutex);
		}
		pthread_mutex_unlock(&recursosMutex);
	}
}
void *fumador2(void *arg)
{
	for (;;)
	{
		pthread_mutex_lock(&recursosMutex);
		if(tabacoAmount && papelAmount){
			tabacoAmount--;
			papelAmount--;
			fumar(2);
			sem_post(&otra_vez);
		} else {
			pthread_cond_wait(&nuevosRecursos, &recursosMutex);
		}
		pthread_mutex_unlock(&recursosMutex);
	}
}
void *fumador3(void *arg)
{
	for (;;)
	{
		pthread_mutex_lock(&recursosMutex);
		if(fosforosAmount && papelAmount){
			fosforosAmount--;
			papelAmount--;
			fumar(3);
			sem_post(&otra_vez);
		} else {
			pthread_cond_wait(&nuevosRecursos, &recursosMutex);
		}
		pthread_mutex_unlock(&recursosMutex);
	}
}

void *consumer1(void *arg){
	for(;;){
		sem_wait(&tabaco);
		pthread_mutex_lock(&recursosMutex);
		tabacoAmount++;
		pthread_mutex_unlock(&recursosMutex);
		pthread_cond_broadcast(&nuevosRecursos);
	}
}
void *consumer2(void *arg){
	for (;;) {
		sem_wait(&fosforos);
		pthread_mutex_lock(&recursosMutex);
		fosforosAmount++;
		pthread_mutex_unlock(&recursosMutex);
		pthread_cond_broadcast(&nuevosRecursos);
	}
}
void *consumer3(void *arg){
	for (;;) {
		sem_wait(&papel);
		pthread_mutex_lock(&recursosMutex);
		papelAmount++;
		pthread_mutex_unlock(&recursosMutex);
		pthread_cond_broadcast(&nuevosRecursos);
	}
}

int main()
{
	pthread_t s1, s2, s3;
	pthread_t c1, c2, c3;


	pthread_cond_init(&nuevosRecursos, NULL);
	pthread_mutex_init(&recursosMutex, NULL);

	sem_init(&tabaco, 0, 0);
	sem_init(&papel, 0, 0);
	sem_init(&fosforos, 0, 0);
	sem_init(&otra_vez, 0, 1);

	pthread_create(&s1, NULL, fumador1, NULL);
	pthread_create(&s2, NULL, fumador2, NULL);
	pthread_create(&s3, NULL, fumador3, NULL);

	pthread_create(&c1, NULL, consumer1, NULL);
	pthread_create(&c2, NULL, consumer2, NULL);
	pthread_create(&c3, NULL, consumer3, NULL);

	agente();
	return 0;
}
