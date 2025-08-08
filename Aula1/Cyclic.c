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

int main(void){
	
	char greeting[MAX_STRING];
	int comm_sz, my_rank;
					
	MPI_Init(NULL, NULL);
	
	MPI_Comm_size(MPI_COMM_WORLD,&comm_sz);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	
	
		// Create message
		sprintf(greeting, "Greetings from process %d of %d:", my_rank,comm_sz);
		// Send message to process 0
		MPI_Send(greeting, strlen(greeting) + 1, MPI_CHAR, (my_rank + 1) % comm_sz, 0, MPI_COMM_WORLD);
		MPI_Recv(greeting, MAX_STRING, MPI_CHAR, (my_rank - 1 + comm_sz)%comm_sz, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	printf("%s\n", greeting);
	
	MPI_Finalize();                       // finaliza o ambiente MPI
    	return 0;
}
