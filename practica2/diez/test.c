#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "diez.h"

#define E 6
#define S 1

pthread_mutex_t lock;
struct _Cond cv;

void* espera(void* arg){
  int quien = * (int*) arg;

  while(1){
    pthread_mutex_lock(&lock);
    cond_wait(&cv, &lock);
    printf("Recibi la señal, hilo: %d\n", quien);
    pthread_mutex_unlock(&lock);
  }

}


void* senala(void* arg){
  int quien = * (int*) arg;

  while(1){
    
    if(quien%2){
      cond_broadcast(&cv);
    } else{
      cond_signal(&cv);
    }

  }
}

int main(){
  pthread_mutex_init(&lock, NULL);

  cond_init(&cv);

  pthread_t r[E], w[S];
	int arg_r[E], arg_w[S], i, pid = 0;
    
	// hilos que esperan
	for(i = 0; i < E; i++) {
		arg_r[i] = pid++;
		pthread_create(&r[i], 0, espera, &arg_r[i]);
	}

	// hilos que mandan señales
	for(i = 0; i < S; i++) {
		arg_w[i] = pid++;
		pthread_create(&w[i], 0, senala, &arg_w[i]);
	}

	pthread_join(r[0], NULL);
}