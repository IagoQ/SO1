#include <omp.h>
#include <stdio.h>
#include <math.h>

int main(int argc, char **argv) {

  int divisores = 0;
  int number = 12;

#pragma omp parallel for reduction(+: divisores) // reduction(operator: reduction-variable)
  for (int i = 1; i < (int)sqrt(number); i++) {
    if (number % i == 0) {
      divisores = divisores + 1; // there is a race condition if reduction(+:
    }
  }

  printf("divisores: %d\n", divisores);

  return 0;
}
