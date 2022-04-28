#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>

#define N_VISITANTES 100000
#define N_THREAD 3

int visitantes = 0;

int numero[N_THREAD];
bool eligiendo[N_THREAD];


/* Función que bloquea la region crítica */
void lock(int i){
    int max = 0;

    /* Calcula el número de turno */
    eligiendo[i] = true;
    asm("mfence");
    for (int k = 0; k < N_THREAD; k++){
        if (numero[k] > max){
            max = numero[k];
        }   
    }

    numero[i] = 1+ max;

    asm("mfence");
    eligiendo[i] = false;
    // asm("mfence");

    /* Compara con todos los hilos */
    for (int j = 0; j < N_THREAD; j++){

        /* Si el hilo j está calculando su número, espera a que termine */
        while (eligiendo[j]) { /* busy waiting */ }
    	// asm("mfence");


        /* Si el hilo j tiene más prioridad, espera a que ponga su número a cero */
        /* j tiene más prioridad si su número de turno es más bajo que el de i, */
        /* o bien si es el mismo número y además j es menor que i */
        while ((numero[j] != 0) && ((numero[j] < numero[i]) || ((numero[j] == numero[i]) && (j < i)))) {
	       	/* busy waiting */ }
    }
 }
/* Función que desbloquea la region crítica */
void unlock(int i){
    numero[i] = 0;
}

void* molinete(void *arg){
    int i = (int) arg;
    
    for (int j = 0; j < N_VISITANTES; j++){
        lock(i); 
        visitantes++;
        unlock(i);
    }
}

int main(){
    pthread_t m[N_THREAD];

    for (int i = 0; i < N_THREAD; i++)
    	numero[i] = 0;

    for (int i = 0; i < N_THREAD; i++)
    	eligiendo[i] = false;
 

    for (int i = 0; i < N_THREAD; i++)
        pthread_create(&m[i], NULL, molinete, (void *)i);
        
    for (int i = 0; i < N_THREAD; i++)
        pthread_join(m[i], NULL);

    printf("Hoy hubo %d visitantes!\n", visitantes);
    return 0;
}

