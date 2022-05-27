#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <assert.h>

// Tamaño del array
#define N 3363

// Crea un array de enteros
int* init_array(int n) {
  int* array = malloc(sizeof(int) * n);
  assert(array != NULL);

  for (int i = 0; i < n; i++) {
    array[i] = i;
  }

  return array;
}

// Calcula la suma de un array de enteros
int compute_add(int* array, int n) {
  int sum = 0;

  for (int i = 0; i < n; i++) {
    sum += array[i];
  }
  return sum;
}

int main(int argc, char** argv) {

  MPI_Init(&argc, &argv);

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // Número de elementos por proceso
  int n_per_proc = N / size;

  // Crear el array de números en el proceso raíz
  int* array = NULL;
  if (rank == 0) {
    array = init_array(N);
  }

  // Para cada proceso, crea un buffer que contendrá una parte del array
  int* sub_array;
  int leftovers = N % size;
  if (!(rank == (size - 1))) {
     sub_array = malloc(sizeof(int) * n_per_proc);
  } else {
     sub_array = malloc(sizeof(int) * (n_per_proc + leftovers));
  }
  assert(sub_array != NULL);

  // Cantidad de elementos por proceso
  int numbercounts[size];
  int i;
  for (i = 0; i < size - 1; i++) {
    numbercounts[i] = n_per_proc;
  }
  numbercounts[i] = n_per_proc + leftovers;

  // offset del array para cada proceso
  int displs[size];
  displs[0] = 0;
  for (i = 0; i < size - 1; i++) {
    displs[i+1] = displs[i] + numbercounts[i];
  }

  // Divide y distribuye el array desde el proceso raíz a todos los procesos
  // int MPI_Scatterv(void *sendbuf, int *sendcounts, int *displs,
  // MPI_Datatype sendtype, void *recvbuf, int recvcount,
  // MPI_Datatype recvtype, int root, MPI_Comm comm)
  MPI_Scatterv(array, numbercounts, displs, MPI_INT, sub_array, numbercounts[rank], MPI_INT, 0, MPI_COMM_WORLD);

  // Calcula la suma del subconjunto 
  int sub_add = compute_add(sub_array, numbercounts[rank]);

  // Reune todas las sumas parciales en el proceso raíz
  int *sub_adds = NULL;
  if (rank == 0) {
    sub_adds = malloc(sizeof(int) * size);
    assert(sub_adds != NULL);
  }
  MPI_Gather(&sub_add, 1, MPI_INT, sub_adds, 1, MPI_INT, 0, MPI_COMM_WORLD);

  // Calcula la suma total de todos los números
  if (rank == 0) {
    int add = compute_add(sub_adds, size);
    printf("La suma es: %d\n", add);
    // Calcula la suma usando el array completo para comparar
    int original_add = compute_add(array, N);
    printf("La suma usando el array completo es: %d\n", original_add);
  }


  if (rank == 0) {
    free(array);
    free(sub_adds);
  }
  free(sub_array);

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
}
