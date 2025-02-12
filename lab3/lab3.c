#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int compare_shorts(const void *a, const void *b) {
    short arg1 = *(const short*)a;
    short arg2 = *(const short*)b;
    return (arg1 > arg2) - (arg1 < arg2);
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    short *matrix = NULL;
    short *transposed_flat = NULL;
    int *sendcounts = NULL;
    int *displs = NULL;

    // Генерация матрицы в процессе 0
    if (rank == 0) {
        matrix = malloc(4 * 9 * sizeof(short));
        srand(time(NULL));
        
        printf("Original 4x9 matrix:\n");
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 9; j++) {
                matrix[i*9 + j] = rand() % 100 - 50;
                printf("%4d", matrix[i*9 + j]);
            }
            printf("\n");
        }

        // Транспонирование матрицы
        transposed_flat = malloc(9 * 4 * sizeof(short));
        for (int i = 0; i < 9; i++) 
            for (int j = 0; j < 4; j++) 
                transposed_flat[i*4 + j] = matrix[j*9 + i];

        // Расчет параметров распределения
        sendcounts = malloc(size * sizeof(int));
        displs = malloc(size * sizeof(int));
        
        int num_rows_base = 9 / size;
        int remainder = 9 % size;
        int current_displ = 0;
        
        for (int i = 0; i < size; i++) {
            int num_rows = num_rows_base + (i < remainder ? 1 : 0);
            sendcounts[i] = num_rows * 4;
            displs[i] = current_displ;
            current_displ += sendcounts[i];
        }
    }

    // Расчет количества получаемых данных для текущего процесса
    int num_rows_base = 9 / size;
    int remainder = 9 % size;
    int num_rows = (rank < remainder) ? num_rows_base + 1 : num_rows_base;
    int recvcount = num_rows * 4;
    short *recvbuf = malloc(recvcount * sizeof(short));

    // Распределение данных
    MPI_Scatterv(
        transposed_flat, sendcounts, displs, MPI_SHORT,
        recvbuf, recvcount, MPI_SHORT,
        0, MPI_COMM_WORLD
    );

    // Сортировка полученных столбцов
    for (int i = 0; i < num_rows; i++) 
        qsort(recvbuf + i*4, 4, sizeof(short), compare_shorts);

    // Сбор результатов
    short *gathered_transposed = NULL;
    if (rank == 0)
        gathered_transposed = malloc(9 * 4 * sizeof(short));
    
    MPI_Gatherv(
        recvbuf, recvcount, MPI_SHORT,
        gathered_transposed, sendcounts, displs, MPI_SHORT,
        0, MPI_COMM_WORLD
    );

    // Вывод результатов в процессе 0
    if (rank == 0) {
        short sorted_matrix[4][9];
        for (int i = 0; i < 9; i++)
            for (int j = 0; j < 4; j++)
                sorted_matrix[j][i] = gathered_transposed[i*4 + j];

        printf("\nSorted matrix (columns ascending):\n");
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 9; j++)
                printf("%4d", sorted_matrix[i][j]);
            printf("\n");
        }

        free(matrix);
        free(transposed_flat);
        free(gathered_transposed);
        free(sendcounts);
        free(displs);
    }

    free(recvbuf);
    MPI_Finalize();
    return 0;
}