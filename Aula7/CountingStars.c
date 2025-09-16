#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_PROCESSES 8
#define LUMINOSITY_THRESHOLD 233

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
  
  if (world_rank == 0) {
      if (argc < 2) {
          fprintf(stderr, "Uso: mpirun -np %d %s <nome_do_arquivo.pgm>\n", NUM_PROCESSES, argv[0]);
          MPI_Abort(MPI_COMM_WORLD, 1);
      }
  }

  unsigned char* full_image_data = NULL;
  int total_pixels = 0;

  if (world_rank == 0) {
    int image_width, image_height, max_val;
    char buffer[100];
    
    FILE *image_file = fopen(argv[1], "r"); 
    if (image_file == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo da imagem %s.\n", argv[1]);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    
    fgets(buffer, 100, image_file);
    
    do {
      fgets(buffer, 100, image_file);
    } while (buffer[0] == '#');

    sscanf(buffer, "%d %d", &image_width, &image_height);
    fscanf(image_file, "%d", &max_val);

    printf("Processo Mestre (0): Lendo imagem '%s' (%dx%d).\n", argv[1], image_width, image_height);

    total_pixels = image_width * image_height;
    
    full_image_data = (unsigned char*)malloc(total_pixels * sizeof(unsigned char));
    if (full_image_data == NULL) {
        fprintf(stderr, "Falha na alocação de memória para a imagem completa.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    for (int i = 0; i < total_pixels; i++) {
        int pixel_val;
        fscanf(image_file, "%d", &pixel_val);
        full_image_data[i] = (unsigned char)pixel_val;
    }
    fclose(image_file);
  }
  

  MPI_Bcast(&total_pixels, 1, MPI_INT, 0, MPI_COMM_WORLD);


  int chunk_size = total_pixels / world_size;
  
  unsigned char* sub_image_data = (unsigned char*)malloc(chunk_size * sizeof(unsigned char));
   if (sub_image_data == NULL) {
        fprintf(stderr, "Falha na alocação de memória no processo %d.\n", world_rank);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

  MPI_Scatter(full_image_data, chunk_size, MPI_UNSIGNED_CHAR, 
              sub_image_data, chunk_size, MPI_UNSIGNED_CHAR, 
              0, MPI_COMM_WORLD);


  int local_star_count = 0;
  for (int i = 0; i < chunk_size; i++) {
      if (sub_image_data[i] > LUMINOSITY_THRESHOLD) {
          local_star_count++;
      }
  }
  printf("Processo %d encontrou %d estrelas em sua fatia.\n", world_rank, local_star_count);


  int total_star_count = 0;
  MPI_Reduce(&local_star_count, &total_star_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);


  if (world_rank == 0) {
      printf("\n--------------------------------------------------\n");
      printf("Processo Mestre (0) reuniu todos os resultados.\n");
      printf("Total de estrelas encontradas na imagem: %d\n", total_star_count);
      printf("--------------------------------------------------\n");
      
      free(full_image_data);
  }
  
  free(sub_image_data);

  MPI_Finalize();
  return 0;
}