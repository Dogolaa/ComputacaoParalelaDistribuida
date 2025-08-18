#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    const int PING_PONG_LIMIT = 10;

    // Inicializa o ambiente MPI
    MPI_Init(NULL, NULL);
    // Descobre o rank e o tamanho do mundo
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Garante que haja pelo menos 2 processos
    if (world_size < 2) {
        fprintf(stderr, "World size must be greater than one for %s\n", argv[0]);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int ping_pong_count = 0;
    
    // Calcula o rank do próximo processo no anel
    int next_rank = (world_rank + 1) % world_size;
    // Calcula o rank do processo anterior no anel
    // Adicionamos world_size antes do módulo para lidar com o caso do rank 0
    int prev_rank = (world_rank - 1 + world_size) % world_size;

    // Apenas o processo com rank 0 inicia a bola
    if (world_rank == 0) {
        printf("Processo %d está iniciando o ping-pong com %d processos.\n", world_rank, world_size);
        ping_pong_count++;
        MPI_Send(&ping_pong_count, 1, MPI_INT, next_rank, 0, MPI_COMM_WORLD);
        printf("Processo %d enviou e incrementou ping_pong_count %d para o processo %d.\n", world_rank, ping_pong_count, next_rank);
    }

    // Loop principal para receber e enviar a mensagem
    while (ping_pong_count < PING_PONG_LIMIT) {
        // Recebe a mensagem do processo anterior
        MPI_Recv(&ping_pong_count, 1, MPI_INT, prev_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Processo %d recebeu ping_pong_count %d do processo %d.\n", world_rank, ping_pong_count, prev_rank);

        // Se o limite foi atingido, não envia mais e encerra
        if (ping_pong_count >= PING_PONG_LIMIT) {
            break;
        }

        // Incrementa o contador e envia para o próximo processo
        ping_pong_count++;
        MPI_Send(&ping_pong_count, 1, MPI_INT, next_rank, 0, MPI_COMM_WORLD);
        printf("Processo %d enviou e incrementou ping_pong_count %d para o processo %d.\n", world_rank, ping_pong_count, next_rank);
    }

    // Finaliza o ambiente MPI
    MPI_Finalize();
}