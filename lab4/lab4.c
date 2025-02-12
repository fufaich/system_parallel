#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define SIZE 7
#define RAND_RANGE 100

int main(int argc, char **argv) {
    short a[SIZE], b[SIZE], c[SIZE];
    

    srand(time(NULL));
    

    #pragma omp parallel for
    for(int i = 0; i < SIZE; i++) {
        b[i] = rand() % RAND_RANGE - RAND_RANGE/2;
        c[i] = rand() % RAND_RANGE - RAND_RANGE/2;
    }

    printf("Vector b:\t");
    for(int i = 0; i < SIZE; i++) printf("%6hd", b[i]);
    
    printf("\nVector c:\t");
    for(int i = 0; i < SIZE; i++) printf("%6hd", c[i]);
    
    #pragma omp parallel for
    for(int i = 0; i < SIZE; i++) {
        a[i] = b[i] + c[i];
    }

    printf("\nResult a:\t");
    for(int i = 0; i < SIZE; i++) printf("%6hd", a[i]);
    printf("\n");

    return 0;
}