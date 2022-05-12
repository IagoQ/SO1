#include <stdio.h>
#include <pthread.h>


#define N_VISITANTES 100000
#define N_THREAD 8

int visitantes = 0;

int numero[N_THREAD];
int eligiendo[N_THREAD];

int max() {
    int max = 0;

	for (int k = 0; k < N_THREAD; k++){
        if (numero[k] > max){
            max = numero[k];
        }   
    }
    return max;
}

/* Función que bloquea la region crítica */
void lock(int i){

    /* Calcula el número de turno */
    eligiendo[i] = 1;
    asm("mfence");
    numero[i] = 1 + max();
    asm("mfence");
    eligiendo[i] = 0;

    /* Compara con todos los hilos */
    for (int j = 0; j < N_THREAD; j++){

        /* Si el hilo j está calculando su número, espera a que termine */
        while (eligiendo[j]) { /* busy waiting */ }

        /* Si el hilo j tiene más prioridad, espera a que ponga su número a cero */
        /* j tiene más prioridad si su número de turno es más bajo que el de i, */
        /* o bien si es el mismo número y además j es menor que i */
        while ((numero[j] != 0) && ((numero[j] < numero[i]) || ((numero[j] == numero[i]) && (j < i)))) { /* busy waiting */ }
    }
 }
/* Función que desbloquea la region crítica */
void unlock(int i){
    numero[i] = 0;
}

void* molinete(void *arg){
    int i = * (int*) arg;
    
    for (int j = 0; j < N_VISITANTES; j++){
        lock(i); 
        visitantes++;
        unlock(i);
    }
}

int main(){
    pthread_t m[N_THREAD];
    int arg[N_THREAD];

    for (int i = 0; i < N_THREAD; i++)
    	numero[i] = 0;

    for (int i = 0; i < N_THREAD; i++)
    	eligiendo[i] = 0;
 
    // Crea los threads
    for(int i = 0; i < N_THREAD; i++){
		arg[i] = i;
		pthread_create(&m[i], 0, molinete, &arg[i]);
	}
        
    for (int i = 0; i < N_THREAD; i++)
        pthread_join(m[i], NULL);

    printf("Hoy hubo %d visitantes!\n", visitantes);
    return 0;
}

