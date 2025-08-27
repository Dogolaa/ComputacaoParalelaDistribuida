#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Função para ordenar um array usando Bubble Sort
void bubble_sort(int arr[], int n) {
    int i, j, temp;
    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

int main(int argc, char** argv) {
  MPI_Init(NULL, NULL);

  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  // Verificação para garantir que há pelo menos um mestre e um escravo
  if (world_size < 2) {
      fprintf(stderr, "Must use at least two processes for this example\n");
      MPI_Abort(MPI_COMM_WORLD, 1);
  }

  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  const int MAX_NUMBERS = 100;
  int numbers[MAX_NUMBERS];
  int number_amount;
  int part_size;

  if (world_rank == 0) { // Processo mestre (Master)
    // Inicializa a semente do gerador de números aleatórios
    srand(time(NULL));

    // Escolhe a quantidade de inteiros para distribuir
    number_amount = (rand() % MAX_NUMBERS) + 1;
    
    // Inicializa o array com valores aleatórios
    printf("0 (mestre) gerou um total de %d numeros.\n", number_amount);
    for (int i = 0; i < number_amount; i++) {
        numbers[i] = rand() % 100;
    }
    
    // Distribui os dados para os escravos
    part_size = number_amount / (world_size - 1);
    
    for (int i = 1; i < world_size; i++) {
      MPI_Send(&numbers[(i-1) * part_size], part_size, MPI_INT, i, 0, MPI_COMM_WORLD);
      printf("0 (mestre) enviou %d numeros para o escravo %d\n", part_size, i);
    }
    
  } else { // Processos escravos (Slaves)
    MPI_Status status;

    // Recebe a porção de dados do mestre
    MPI_Recv(numbers, MAX_NUMBERS, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

    // Obtém a quantidade de números recebidos
    MPI_Get_count(&status, MPI_INT, &number_amount);

    printf("%d (escravo) recebeu %d numeros do mestre 0. Numeros antes da ordenacao:\n", 
           world_rank, number_amount);
    // Impressão dos números recebidos
    for (int i = 0; i < number_amount; i++) {
        printf("%d ", numbers[i]);
    }
    printf("\n");

    // Ordena o array usando Bubble Sort
    bubble_sort(numbers, number_amount);

    printf("%d (escravo) numeros apos a ordenacao:\n", world_rank);
    // Impressão dos números ordenados
    for (int i = 0; i < number_amount; i++) {
        printf("%d ", numbers[i]);
    }
    printf("\n");
  }
  
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
}