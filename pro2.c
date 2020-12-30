/******************************************************************************
*   FILE: pro2.c
*   DESCRIPTION:  
*   MPI Integral - C Version
*   Serial Integral calculation.
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
double start_time, end_time;
float a, b, h, result;
int n, i;
start_time = MPI_Wtime();
printf("Please Enter a, b ,n:\n");
int unused;
unused = scanf("%f%f%d",&a,&b,&n);
h = (b - a) / n;
float y[n],x[n], sum;
y[0] = pow(a, 2);
y[n - 1] = pow(b, 2);
for(i = 1; i < n - 1; i++)
	{
	x[i] = a + i * h;
	y[i] = pow(x[i], 2);
	}
	sum = 0;
for(i = 1; i < n - 1; i++)
	{
	sum = sum + y[i];
	}
result = h / 2 * (y[0]  + (2 * sum) + y[n-1]);
printf("Result : %f \n", result);
end_time = MPI_Wtime();
//Finalize MPI environment
MPI_Finalize();
printf("Execution time: %f\n", end_time-start_time);
return 0;
}
