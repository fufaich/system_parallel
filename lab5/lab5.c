#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define ROWS 20
#define COLS 60
#define RAND_MAX_SHORT 32767

int compare_shorts(const void *a, const void *b) {
    short arg1 = *(const short*)a;
    short arg2 = *(const short*)b;
    return (arg1 > arg2) - (arg1 < arg2);
}

int main(int argc, char **argv) {
    short matrix[ROWS][COLS];
    double start_time, end_time;

    srand(time(NULL));

    #pragma omp parallel for collapse(2)
    for(int i = 0; i < ROWS; i++) {
        for(int j = 0; j < COLS; j++) {
            matrix[i][j] = rand() % (2*RAND_MAX_SHORT + 1) - RAND_MAX_SHORT;
        }
    }

    printf("Original matrix:\n");
    for(int i = 0; i < 10; i++) {
        for(int j = 0; j < 10; j++) {
            printf("%7d", matrix[i][j]);
        }
        printf("\n");
    }

    start_time = omp_get_wtime();

    #pragma omp parallel for
    for(int col = 0; col < COLS; col++) {
        short column[ROWS];
        for(int row = 0; row < ROWS; row++) {
            column[row] = matrix[row][col];
        }
        qsort(column, ROWS, sizeof(short), compare_shorts);

        for(int row = 0; row < ROWS; row++) {
            matrix[row][col] = column[row];
        }
    }
    end_time = omp_get_wtime();

    printf("\nSorted matrix:\n");
    for(int i = 0; i < 10; i++) {
        for(int j = 0; j < 10; j++) {
            printf("%7d", matrix[i][j]);
        }
        printf("\n");
    }

    printf("\nExecution time: %.5f seconds\n", end_time - start_time);

    return 0;
}