#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define VECTOR_SIZE 20
#define NUM_PROCESSES 4

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    long long result = 1;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);


    short vector[VECTOR_SIZE];
    srand(time(NULL) + rank);
    
    // Генерация чисел от 1 до 3 для избежания переполнения
    printf("Proc %d vector: [", rank);
    for (int i = 0; i < VECTOR_SIZE; i++) {
        vector[i] = rand() % 5 + 1;
        printf(" %d ", vector[i]);
    }
    printf("]\n");

    long long local_res = 1;
    for (int i = 0; i < VECTOR_SIZE; i++) {
        local_res *= vector[i];
    }

    long long *results = NULL;
    if (rank == 0) {
        results = malloc(NUM_PROCESSES * sizeof(long long));
    }

    MPI_Gather(&local_res, 1, MPI_LONG_LONG,
               results, 1, MPI_LONG_LONG,
               0, MPI_COMM_WORLD);

    // Вывод результатов
    if (rank == 0) {
        printf("Res multiply:\n");
        for (int i = 0; i < NUM_PROCESSES; i++) {
            printf("Процесс %d: %lld\n", i, results[i]);
            result *= results[i];
        }
        free(results);
        printf("\nResult: %lld\n", result);
    }

    MPI_Finalize();
    return 0;
}