#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Definindo o tamanho da matriz e o número de processos para facilitar a leitura
#define MATRIX_SIZE 1024
#define NUM_PROCESSES 8

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);

  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  // Verificação para garantir que o programa seja executado com exatamente 8 processos
  if (world_size != NUM_PROCESSES) {
      if (world_rank == 0) {
          fprintf(stderr, "Este programa deve ser executado com exatamente %d processos.\n", NUM_PROCESSES);
      }
      MPI_Abort(MPI_COMM_WORLD, 1);
  }

  // ---- Variáveis ----
  int K; // O escalar que será multiplicado
  
  // O processo mestre (rank 0) aloca a matriz completa
  int* full_matrix = NULL;
  int* result_matrix = NULL;

  if (world_rank == 0) {
    // Aloca memória para a matriz completa e a matriz de resultado
    full_matrix = (int*)malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(int));
    result_matrix = (int*)malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(int));
    if (full_matrix == NULL || result_matrix == NULL) {
        fprintf(stderr, "Falha na alocação de memória no processo mestre.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
  }

  // ---- Lógica do Mestre (Rank 0): Geração de Dados ----
  if (world_rank == 0) {
    srand(time(NULL));
    K = (rand() % 10) + 1; // K será um número aleatório de 1 a 10

    printf("Processo 0 (mestre) gerou a matriz %dx%d e o escalar K = %d.\n", MATRIX_SIZE, MATRIX_SIZE, K);
    
    // Preenche a matriz com números aleatórios (0 a 99)
    for (int i = 0; i < MATRIX_SIZE * MATRIX_SIZE; i++) {
        full_matrix[i] = rand() % 100;
    }
  }

  // ---- Etapa 1: MPI_Bcast ----
  // O processo 0 envia o valor de K para todos os outros processos.
  // Todos os processos devem chamar esta função.
  MPI_Bcast(&K, 1, MPI_INT, 0, MPI_COMM_WORLD);
  // Após esta linha, todos os processos (0 a 7) terão o mesmo valor para a variável 'K'.


  // ---- Etapa 2: MPI_Scatter ----
  // O processo 0 divide 'full_matrix' em 'world_size' partes e envia uma parte para cada processo.
  
  // Calcula o tamanho da fatia que cada processo irá receber
  int chunk_size = (MATRIX_SIZE * MATRIX_SIZE) / world_size;
  
  // Aloca um buffer em CADA processo para receber sua parte da matriz
  int* sub_matrix = (int*)malloc(chunk_size * sizeof(int));
   if (sub_matrix == NULL) {
        fprintf(stderr, "Falha na alocação de memória no processo %d.\n", world_rank);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

  // Distribui a matriz
  // O processo 0 envia os dados de 'full_matrix'
  // Todos os outros processos recebem seus respectivos pedaços em 'sub_matrix'
  MPI_Scatter(full_matrix, chunk_size, MPI_INT, 
              sub_matrix, chunk_size, MPI_INT, 
              0, MPI_COMM_WORLD);

  // ---- Computação Paralela ----
  // Cada processo agora trabalha em sua própria parte da matriz ('sub_matrix').
  printf("Processo %d recebeu K=%d e sua fatia da matriz. Iniciando a multiplicação.\n", world_rank, K);
  for (int i = 0; i < chunk_size; i++) {
      sub_matrix[i] = sub_matrix[i] * K;
  }
  printf("Processo %d terminou a multiplicação.\n", world_rank);

  // ---- Etapa 3: MPI_Gather ----
  // Cada processo envia o conteúdo do seu 'sub_matrix' de volta para o processo 0.
  // O processo 0 reúne todas as partes em 'result_matrix'.
  MPI_Gather(sub_matrix, chunk_size, MPI_INT,
             result_matrix, chunk_size, MPI_INT,
             0, MPI_COMM_WORLD);

  // ---- Finalização e Verificação (Apenas no Mestre) ----
  if (world_rank == 0) {
.      printf("\nProcesso 0 (mestre) reuniu todos os resultados.\n");
      
      // Verificação simples: imprime o primeiro elemento da matriz original e da resultante
      // para ver se a multiplicação está correta.
      printf("Verificação:\n");
      printf("  - Elemento original [0]: %d\n", full_matrix[0]);
      printf("  - K: %d\n", K);
      printf("  - Elemento resultante [0]: %d\n", result_matrix[0]);
      if (result_matrix[0] == full_matrix[0] * K) {
          printf("  - O cálculo parece correto!\n");
      } else {
          printf("  - O cálculo parece INCORRETO!\n");
      }
      
      // Libera a memória alocada apenas no mestre
      free(full_matrix);
      free(result_matrix);
  }
  
  // Libera a memória do buffer de cada processo
  free(sub_matrix);

  MPI_Finalize();
  return 0;
}