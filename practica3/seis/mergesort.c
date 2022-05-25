#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define N 200000

void merge(int arr[], long long l, long long m, long long r) {
  int leftLen = m - l + 1;
  int rightLen = r - m;

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

  // add leftover numbers
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

// Divide the array into two subarrays, sort them and merge them
void mergeSort(int arr[], long long l, long long r) {
  if (l < r) {
    // m is the point where the array is divided into two subarrays
    long long m = l + (r - l) / 2;

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


    // Merge the sorted subarrays
    merge(arr, l, m, r);
  }
}

int main(int argc, char **argv) {
  int arr[N];

  for (long long i = 0; i < N; i++) {
    arr[i] = rand() % 500000;
  }

#pragma omp parallel
  {
#pragma omp single
    { mergeSort(arr, 0, N - 1); }
  }

  for (long long i = 0; i < N - 1; i++) {
    if (arr[i] > arr[i + 1]) {
      perror("No esta ordenado");
      exit(1);
    }
  }

  return 0;
}
