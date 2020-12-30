/******************************************************************************
*   FILE: pro1.c
*   DESCRIPTION:  
*   MPI Matrix Multiply - C Version
*   In this code, the master task distributes a matrix multiply
*   operation to size-1 worker tasks.
*   AUTHOR: Amin Salem
*   LAST UPDATE: 12/22/2020
******************************************************************************/
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
static double a[64][64];
static double b[64][64];
static double c[64][64];
static void print_matrix(void)
{
int i, j;
	for (i = 0; i < 64; i++){
		for (j = 0; j < 64; j++) {
			printf( "[%d,%d] : %f\n", i, j, c[i][j]);
		}
	}
} //end print_matrix
int main(int argc, char** argv) {
//Initialize MPI environment
MPI_Init(&argc, &argv);
//Get number of processes
int size;
MPI_Comm_size(MPI_COMM_WORLD, &size);
//Get rank of processes
int rank;
MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//Error
int err;
if (size < 2 ) 
	{
	printf("Need at least two MPI tasks. Quitting...\n");
	MPI_Abort(MPI_COMM_WORLD, err);
	exit(1);
	}
MPI_Status status;
// used to determine part sent to each worker 
int dest, src, offseta, offsetb;
int i, j, k, l; //temporal
int rows;
double start_time, end_time;
rows = 64 / size;  //compute the block size
/*** master task ***/
if (rank == 0)
{
	start_time = MPI_Wtime();
	//send offset of array to other processes
	for(l = 0; l < size; l++)
	{
		offsetb = rows * l;  //start from (block size * processor id)
		offseta = rows;
		for(dest = 1; dest < size; dest++)
		{
		MPI_Send(&offseta, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
                MPI_Send(&offsetb, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
                MPI_Send(&rows, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
		offseta += rows;
                offsetb = (offsetb + rows) % 64;
		}
		offseta = rows;
                offsetb = rows * l;
			
		for(i = 0; i < offseta; i++) 
		{
                for(j = offsetb; j < offsetb + rows; j++)
			{
                        for(k = 0; k < 64; k++)
				{
				a[i][k] = i * k;
				b[k][j] = j * k;
				c[i][j] = c[i][j] + a[i][k] * b[k][j];
				}//end for k
                	} //end for j
		}// end for i
		for(src = 1; src < size; src++)
		{
                MPI_Recv(&offseta, 1, MPI_INT, src, 0, MPI_COMM_WORLD,  			&status);
                MPI_Recv(&offsetb, 1, MPI_INT, src, 0, MPI_COMM_WORLD, 				&status);
                MPI_Recv(&rows, 1, MPI_INT, src, 0, MPI_COMM_WORLD, &status);
                for(i = 0; i < rows; i++)
		{
			MPI_Recv(&c[offseta+i][offsetb], offseta, MPI_DOUBLE, 					src, 0, MPI_COMM_WORLD, &status);
                } //end for scr
		}//end for i
	} //end for l
	end_time = MPI_Wtime();          
        print_matrix();
        printf("Execution time on %2d nodes: %f\n", size, end_time-
             start_time);
} //end if (rank == 0) 
/*** worker ***/
else{  
	for(l = 0; l < size; l++)
	{
		MPI_Recv(&offseta, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		MPI_Recv(&offsetb, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		MPI_Recv(&rows, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		for(i = offseta; i < offseta+rows; i++)
		{
			for(j = offsetb; j < offsetb+rows; j++)
			{
				for(k = 0; k < 64; k++)
				{
				a[i][k] = i * k;
				b[k][j] = j * k;
				c[i][j] = c[i][j] + a[i][k]*b[k][j];
				} //end for k 
			} //end for j
		} //end for i    
		MPI_Send(&offseta, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		MPI_Send(&offsetb, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		MPI_Send(&rows, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		for(i = 0; i < rows; i++)
		{
		MPI_Send(&c[offseta+i][offsetb], offseta, MPI_DOUBLE, 0, 0, 				MPI_COMM_WORLD);
		} //end for i
	}//end for l
} // end else
//Finalize MPI environment
MPI_Finalize();
}//end main()
