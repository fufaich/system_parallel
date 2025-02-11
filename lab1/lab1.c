#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define M 5
#define N 4

int main(int argc, char *argv[]) {
   int rank, size;
   
   MPI_Init(&argc, &argv);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);

   int gather_vector[N][M];

   if (rank == 0) {
       MPI_Comm_size(MPI_COMM_WORLD, &size);
       printf("Total size %d\n", size);
       int cnt = 0;
       for (int i = 0; i < N; i++) {
           for (int j = 0; j < M; j++) {
               gather_vector[i][j] = cnt++;
           }
       }

       printf("Root generate vector:\n");
       for (int i = 0; i < N; i++) {
           for (int j = 0; j < M; j++) {
               printf(" %d ", gather_vector[i][j]);
           }
           printf("\n");
       }
   }

   MPI_Bcast(gather_vector, N * M, MPI_INT, 0, MPI_COMM_WORLD);

   if (rank != 0) {
       printf("Process %d received message:\n", rank);
       for (int i = 0; i < N; i++) {
           for (int j = 0; j < M; j++) {
               printf(" %d ", gather_vector[i][j]);
           }
           printf("\n");
       }
   }

   MPI_Barrier(MPI_COMM_WORLD);
   MPI_Finalize();
   return 0;
}