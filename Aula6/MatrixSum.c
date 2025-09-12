#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MATRIX_SIZE 1024
#define NUM_PROCESSES 8

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);

  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  if (world_size != NUM_PROCESSES) {
      if (world_rank == 0) {
          fprintf(stderr, "Este programa deve ser executado com exatamente %d processos.\n", NUM_PROCESSES);
      }
      MPI_Abort(MPI_COMM_WORLD, 1);
  }

  // ---- Variáveis ----
  // Matrizes completas que só existem no processo mestre (rank 0)
  int* matrix_A = NULL;
  int* matrix_B = NULL;
  int* result_matrix = NULL;

  if (world_rank == 0) {
    // Aloca memória para as matrizes completas no mestre
    matrix_A = (int*)malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(int));
    matrix_B = (int*)malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(int));
    result_matrix = (int*)malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(int));
    if (matrix_A == NULL || matrix_B == NULL || result_matrix == NULL) {
        fprintf(stderr, "Falha na alocação de memória no processo mestre.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
  }

  // ---- 1. Geração de Dados (Apenas no Mestre) ----
  if (world_rank == 0) {
    srand(time(NULL));
    printf("Processo 0 (mestre) gerando as matrizes A e B de tamanho %dx%d.\n", MATRIX_SIZE, MATRIX_SIZE);
    
    // Preenche as matrizes A e B com números aleatórios (0 a 99)
    for (int i = 0; i < MATRIX_SIZE * MATRIX_SIZE; i++) {
        matrix_A[i] = rand() % 100;
        matrix_B[i] = rand() % 100;
    }
  }

  // Calcula o tamanho da fatia que cada processo irá receber
  int chunk_size = (MATRIX_SIZE * MATRIX_SIZE) / world_size;
  
  // Aloca buffers em CADA processo para receber sua parte das matrizes A, B e para guardar o resultado C
  int* sub_A = (int*)malloc(chunk_size * sizeof(int));
  int* sub_B = (int*)malloc(chunk_size * sizeof(int));
  int* sub_C = (int*)malloc(chunk_size * sizeof(int));
  if (sub_A == NULL || sub_B == NULL || sub_C == NULL) {
        fprintf(stderr, "Falha na alocação de memória dos buffers no processo %d.\n", world_rank);
        MPI_Abort(MPI_COMM_WORLD, 1);
  }

  // ---- 2. Distribuição das Matrizes (Scatter x2) ----
  // O processo 0 distribui a matriz A. Todos os processos recebem sua parte em sub_A.
  MPI_Scatter(matrix_A, chunk_size, MPI_INT, 
              sub_A, chunk_size, MPI_INT, 
              0, MPI_COMM_WORLD);

  // O processo 0 distribui a matriz B. Todos os processos recebem sua parte em sub_B.
  MPI_Scatter(matrix_B, chunk_size, MPI_INT, 
              sub_B, chunk_size, MPI_INT, 
              0, MPI_COMM_WORLD);
  
  printf("Processo %d recebeu suas fatias das matrizes A e B.\n", world_rank);

  // ---- 3. Computação Paralela ----
  // Cada processo agora soma sua fatia de A com sua fatia de B.
  for (int i = 0; i < chunk_size; i++) {
      sub_C[i] = sub_A[i] + sub_B[i];
  }
  printf("Processo %d terminou a soma local.\n", world_rank);

  // ---- 4. Coleta do Resultado (Gather) ----
  // Cada processo envia sua fatia de resultado (sub_C) para o processo 0.
  // O processo 0 reúne todas as fatias em result_matrix.
  MPI_Gather(sub_C, chunk_size, MPI_INT,
             result_matrix, chunk_size, MPI_INT,
             0, MPI_COMM_WORLD);

  // ---- Finalização e Verificação (Apenas no Mestre) ----
  if (world_rank == 0) {
      printf("\nProcesso 0 (mestre) reuniu todos os resultados.\n");
      
      // Verificação simples: checa se a soma do primeiro elemento está correta.
      printf("Verificação do primeiro elemento:\n");
      printf("  - Elemento A[0]: %d\n", matrix_A[0]);
      printf("  - Elemento B[0]: %d\n", matrix_B[0]);
      printf("  - Elemento Resultante[0]: %d\n", result_matrix[0]);
      if (result_matrix[0] == matrix_A[0] + matrix_B[0]) {
          printf("  - O cálculo da soma parece correto!\n");
      } else {
          printf("  - O cálculo da soma parece INCORRETO!\n");
      }
      
      // Libera a memória das matrizes completas alocadas no mestre
      free(matrix_A);
      free(matrix_B);
      free(result_matrix);
  }
  
  // Libera a memória dos buffers de cada processo
  free(sub_A);
  free(sub_B);
  free(sub_C);

  MPI_Finalize();
  return 0;
}