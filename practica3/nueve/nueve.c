#include <stdio.h>
#include <math.h>
#include "mpi.h"

//		compile: mpicc -o nueve nueve.c -lm
//		run: mpirun -n 4 nueve


int main(int argc, char** argv)
{
	int rank, size, steps, sum_p, sum_r, jump;
    MPI_Status status;

    MPI_Init(&argc, &argv);
 
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    steps = log(size) / log(2);
    sum_p = rank;
    sum_r = 0;

    for (int i = 0; i < steps; i++ ){

        jump = pow(2, i);

        if ((rank/jump) % 2){
            MPI_Send(&sum_p,1,MPI_INT,rank-jump,0,MPI_COMM_WORLD);
            MPI_Recv(&sum_r,1,MPI_INT,rank-jump,0,MPI_COMM_WORLD,&status);
            sum_p += sum_r;
        } else {
            MPI_Send(&sum_p,1,MPI_INT,rank+jump,0,MPI_COMM_WORLD);
            MPI_Recv(&sum_r,1,MPI_INT,rank+jump,0,MPI_COMM_WORLD,&status);
            sum_p += sum_r;
        }
    }
    printf("Proceso %d: suma %d\n",rank, sum_p);
    
    MPI_Finalize();

	return 0;
}