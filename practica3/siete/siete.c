#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <assert.h>

#define N 12

// Creates an array of integers
int* init_array(int n) {
  int* array = malloc(sizeof(int) * n);
  assert(array != NULL);

  for (int i = 0; i < n; i++) {
    array[i] = i;
  }

  return array;
}

// Computes the sum of an array of integers
int compute_add(int* array, int n) {
  int sum = 0;

  for (int i = 0; i < n; i++) {
    sum += array[i];
  }
  return sum;
}

int main(int argc, char** argv) {
    
  // Array size
  //int n = 40;

  MPI_Init(&argc, &argv);

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // Number of elements per process (habria que dividir bien del total)
  // int n_per_proc = 40;
  int n_per_proc = N / size;

  // Create the array of numbers on the root process
  int* array = NULL;
  if (rank == 0) {
    array = init_array(N);
  }

  // For each process, create a buffer that will contain a subset of the array
  int* sub_array;
  int leftovers = N % size;
  if (!(rank == size)) {
     sub_array = malloc(sizeof(int) * n_per_proc);
  } else {
     sub_array = malloc(sizeof(int) * n_per_proc + leftovers);
  }
  assert(sub_array != NULL);

  // cantidad de elementos por proc
  int numbercounts[size];
  int i;
  for (i = 0; i < size - 1; i++) {
    numbercounts[i] = n_per_proc;
  }
  numbercounts[i] = n_per_proc + leftovers;

  // offset del array para cada proc
  int displs[size];
  displs[0] = 0;
  for (i = 0; i < size - 1; i++) {
    displs[i+1] = displs[i] + numbercounts[i];
  }

  // Scatter the array from the root process to all processes 
  // int MPI_Scatterv(void *sendbuf, int *sendcounts, int *displs,
  // MPI_Datatype sendtype, void *recvbuf, int recvcount,
  // MPI_Datatype recvtype, int root, MPI_Comm comm)
  MPI_Scatterv(array, numbercounts,displs, MPI_INT, sub_array, numbercounts[rank], MPI_INT, 0, MPI_COMM_WORLD);
  // Computes the sum of the subset
  int sub_add = compute_add(sub_array, n_per_proc);

  // Gather all partial sums to the root process
  int *sub_adds = NULL;
  if (rank == 0) {
    sub_adds = malloc(sizeof(int) * size);
    assert(sub_adds != NULL);
  }
  MPI_Gather(&sub_add, 1, MPI_INT, sub_adds, 1, MPI_INT, 0, MPI_COMM_WORLD);

  // Computes the total sum of all numbers
  if (rank == 0) {
    int add = compute_add(sub_adds, size);
    printf("La suma es: %d\n", add);
    // Computes the sum using the full array for comparison
    int original_add = compute_add(array, n_per_proc * size);
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
