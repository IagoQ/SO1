#include <pthread.h>
#include <stdio.h>

#define N_VISITANTES 100000

int visitantes = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *molinete(void *arg) {
  int i;

  for (i = 0; i < N_VISITANTES; i++) {
    pthread_mutex_lock(&lock); // Bloquea la region critica
    visitantes++;
    pthread_mutex_unlock(&lock); // Libera la region critica
  }
  return 0;
}

int main() {
  pthread_t m1, m2;

  pthread_create(&m1, NULL, molinete, NULL);
  pthread_create(&m2, NULL, molinete, NULL);
  pthread_join(m1, NULL);
  pthread_join(m2, NULL);

  printf("Hoy hubo %d visitantes!\n", visitantes);
  return 0;
}
