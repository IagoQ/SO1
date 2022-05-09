#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "ocho.h"

#define LECTORES 15
#define ESCRITORES 7

struct _Channel canal;

void* reader(void* arg){
	int quien = * (int*) arg;

	while(1){
		printf("Lector %d leyendo\n", quien);
        int v = chan_read(&canal);
		printf("Lei %d\n", v);
	}

	return NULL;
}

void* writer(void* arg){
	int quien = * (int*) arg;

	while(1){
		printf("Escritor %d escribiendo\n", quien);
        chan_write(&canal, quien);
		
	}

	return NULL;
}

void main() {
	
	channel_init(&canal);

	pthread_t r[LECTORES], w[ESCRITORES];
	int arg_r[LECTORES], arg_w[ESCRITORES], i, pid = 0;
    
	// crea los lectores
	for(i = 0; i < LECTORES; i++) {
		arg_r[i] = pid++;
		pthread_create(&r[i], 0, reader, &arg_r[i]);
	}

	// crea los escritores
	for(i = 0; i < ESCRITORES; i++) {
		arg_w[i] = pid++;
		pthread_create(&w[i], 0, writer, &arg_w[i]);
	}

	pthread_join(r[0], NULL);
}