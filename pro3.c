/******************************************************************************
*   FILE: pro3.c
*   DESCRIPTION:  
*   MPI Integral - C Version
*   In this code, the master task distributes an integral calculation
*   operation to size-1 worker tasks.
*   AUTHOR: Amin Salem
*   LAST UPDATE: 12/22/2020
******************************************************************************/
#include<math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main(int argc, char* argv[])
{
//Initialize MPI environment
MPI_Init(&argc, &argv);
//Get number of processes
int size;
MPI_Comm_size(MPI_COMM_WORLD, &size);
//Get rank of processes
int rank;
MPI_Comm_rank(MPI_COMM_WORLD, &rank);

double start_time, end_time;
int n, i, total_size, k, partition, n_part;
float sum, result, total_result;
MPI_Status status;
start_time = MPI_Wtime();
/*** Master ***/
if (rank == 0) {
float a, b, h, result=0;
printf("Please Enter a, b, n:\n");
int unused;
unused = scanf("%f %f %d", &a, &b, &n);
h = (b - a) / n;
partition = n / (size - 1);
int offset;
	for (i = 1; i < size; i++)
	{
		offset = i * partition;
    		MPI_Send(&partition ,1, MPI_INT, i, 0, MPI_COMM_WORLD);
		MPI_Send(&a, 1, MPI_FLOAT, i, 0, MPI_COMM_WORLD);
		MPI_Send(&b, 1, MPI_FLOAT, i, 0, MPI_COMM_WORLD);
		MPI_Send(&h, 1, MPI_FLOAT, i, 0, MPI_COMM_WORLD);
	}
	for (i = 1; i < size; i++)
	{
		MPI_Recv(&result, 1, MPI_FLOAT, i, 0, MPI_COMM_WORLD, &status);
		total_result = total_result + result;
	}
printf("Result : %f \n", total_result);
end_time = MPI_Wtime();
printf("Execution time: %f\n", end_time - start_time);
}
/*** Slave ***/
else 
{
float a, b, h;
MPI_Recv(&n_part, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
MPI_Recv(&a, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
MPI_Recv(&b, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
MPI_Recv(&h, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
MPI_Comm_rank(MPI_COMM_WORLD, &rank);
float y[n_part], x[n_part];
	if(rank == 1)
	{
		x[0] = a;
		y[0] = pow(x[0], 2);
		for(i = 1; i < n_part; i++)
		{
			x[i] = a + (i * h);
			y[i] = pow(x[i], 2);
		}
		sum = 0;
		for(i = 1; i < n_part; i++)
		{
			sum = sum + y[i];
		}
		result = h / 2 * (y[0] + 2 * sum);
	}
        else if(rank == size - 1)
        {
		total_size = (rank * n_part);
		i = total_size - n_part;
		k=0;
		x[n_part - 1] = b;
		y[n_part - 1] = pow(x[n_part - 1], 2);
        	for(i = i; i < total_size - 1; i++)
		{
			x[k] = a + ( i * h);
			y[k] = pow(x[k], 2);
			k++;
		}
		sum = 0;
        	for(i = 0; i < n_part - 1; i++)
		{
			sum = sum + y[i];
		}
        	result = h / 2 * (y[n_part - 1] + 2 * sum);
        }
        else
	{
		total_size = (rank * n_part);
		i = total_size - n_part;
		k=0;
		for(i = i; i < total_size; i++)
		{
			x[k] = a + (i * h);
			y[k] = pow(x[k], 2);
			k++;
		}
		sum = 0;
		for(i = 0; i < n_part; i++)
		{
			sum = sum + y[i];
		}
		result = h / 2 * (2 * sum);
	}
        MPI_Send(&result, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
}
//Finalize MPI environment
MPI_Finalize();
return 0;
}
