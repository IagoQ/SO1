#include <math.h>
#include <omp.h>
#include <stdio.h>
#include "timing.h"

int isPrime(unsigned long number){
	int prime = 1;
	unsigned long target = sqrt(number);
	
	#pragma omp parallel for
	for (int i = 2; i < target; i++) {
		if (number % i == 0) {
			prime = 0;
		}
	}
	
	return prime;
}

int isPrimeSeq(unsigned long number){
	unsigned long target = sqrt(number);
	
	for (int i = 2; i < target; i++) {
		if (number % i == 0) {
			return 0;
		}
	}
	
	return 1;
}

int main(int argc, char **argv) {
	unsigned long number = 18446744073709551557;

	printf("Calculo concurrente \n");
	int prime = TIME(isPrime(number), NULL);

	if (prime) {
		printf("primo \n");
	} else {
		printf("no primo \n");
	}


	printf("Calculo secuencial \n");
	prime = TIME(isPrimeSeq(number), NULL);
	
	if (prime) {
		printf("primo \n");
	} else {
		printf("no primo \n");
	}
	
	return 0;
}

