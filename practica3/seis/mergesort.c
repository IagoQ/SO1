#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define N 200000

// recibe un array con dos partes ordenadas y mergea en una sola
void merge(int arr[], long long l, long long m, long long r) {
  int leftLen = m - l + 1;
  int rightLen = r - m;

  // copia los sub arrays en un buffer para no pisarlos al mergear
  int leftArr[leftLen], rigthArr[rightLen];
  for (int i = 0; i < leftLen; i++)
    leftArr[i] = arr[l + i];
  for (int j = 0; j < rightLen; j++)
    rigthArr[j] = arr[m + 1 + j];

  long long i, j, k;
  i = 0;
  j = 0;
  k = l;

  // MERGE
  while (i < leftLen && j < rightLen) {
    if (leftArr[i] <= rigthArr[j]) {
      arr[k] = leftArr[i];
      i++;
    } else {
      arr[k] = rigthArr[j];
      j++;
    }
    k++;
  }

  // agregar los sobrantes al final
  // estas condiciones son ser mutuamente exclusivas
  while (i < leftLen) {
    arr[k] = leftArr[i];
    i++;
    k++;
  }

  while (j < rightLen) {
    arr[k] = rigthArr[j];
    j++;
    k++;
  }
}

void mergeSort(int arr[], long long l, long long r) {
  if (l < r) {
    // m es el punto medio donde se divide el array en dos subarrays
    long long m = l + (r - l) / 2;

    // si el size del array es suficientemente chico, el costo de levantar los threads no se llega a amortizar
    // por lo tanto lo hacemos sincronicamente
    if (r-l < 100) {
      mergeSort(arr, l, m);

      mergeSort(arr, m + 1, r);

    } else {
      #pragma omp task shared(arr)
      mergeSort(arr, l, m);

      #pragma omp task shared(arr)
      mergeSort(arr, m + 1, r);

      #pragma omp taskwait
    }


    // mergear los subarrays
    merge(arr, l, m, r);
  }
}

int main(int argc, char **argv) {
  int arr[N];

  for (long long i = 0; i < N; i++) {
    arr[i] = rand();
  }

#pragma omp parallel
  {
#pragma omp single
    { 
      mergeSort(arr, 0, N - 1); 
    }
  }

  for (long long i = 0; i < N - 1; i++) {
    if (arr[i] > arr[i + 1]) {
      perror("No esta ordenado");
      exit(1);
    }
  }

  return 0;
}
