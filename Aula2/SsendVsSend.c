/* hello_mpi.c */
#include <mpi.h>
#include <stdio.h>
#include <string.h>

/*
	mpicc
	mpicc -o 01_hello_world 01_hello_world.c
	mpirun
	mpirun -np 4 01_hello_world
	mpirun -np 10 --hostfile hostfile.txt -1_hello_world
*/

const int MAX_STRING = 100;

int main(void)
{

	char greeting[MAX_STRING];
	int comm_sz, rank;

	MPI_Init(NULL, NULL);

	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0)
	{
		sprintf(greeting, "Greetings from process %d of %d:", rank, comm_sz);
		MPI_Recv(greeting, MAX_STRING, MPI_CHAR, (rank - 1 + comm_sz) % comm_sz, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Ssend(greeting, strlen(greeting) + 1, MPI_CHAR, (rank + 1) % comm_sz, 0, MPI_COMM_WORLD);
	}
	else
	{
		sprintf(greeting, "Greetings from process %d of %d:", rank, comm_sz);
		MPI_Ssend(greeting, strlen(greeting) + 1, MPI_CHAR, (rank + 1) % comm_sz, 0, MPI_COMM_WORLD);
		MPI_Recv(greeting, MAX_STRING, MPI_CHAR, (rank - 1 + comm_sz) % comm_sz, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}

	printf("%s\n", greeting);

	MPI_Finalize(); // finaliza o ambiente MPI
	return 0;
}
