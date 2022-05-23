#include <math.h>
#include <omp.h>
#include <stdio.h>

int main(int argc, char **argv) {

  while(1){
    long long number = 40000000000000;
    scanf("%lld", &number);
    long long target = sqrt(number);
    int divisores = 0;

    #pragma omp parallel for reduction(+: divisores) // reduction(operator: reduction-variable)
    for (int i = 2; i < target; i++) {
      if (number % i == 0) {
        divisores = divisores + 1;
      }
    }

    if(divisores == 0){
      printf("primo \n");
    } else {
      printf("no primo \n");
    }
  }

  return 0;
}
