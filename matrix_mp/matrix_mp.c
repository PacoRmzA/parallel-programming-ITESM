//Francisco José Ramírez Aldaco A01634262

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include <string.h>

int threads;

int** matrix_mp(int m, int n, int p, int q, int** mat1, int** mat2) {
    if (n != p) return 0;

    int** result;
    result = malloc(sizeof(int*) * m);
    #pragma omp parallel for num_threads(threads)
    for (int i=0; i<m; i++) result[i] = malloc(sizeof(int*) * q);

    #pragma omp parallel for num_threads(threads)
    for (int i=0; i<m; i++) {
        for (int j=0; j<q; j++) {
            result[i][j] = 0;
            for (int k=0; k<p; k++) {
                result[i][j] += (mat1[i][k]*mat2[k][j]);
            }
        }
    }
    return result;
}

// Function to generate a random matrix (credit: PAOK-2001)
int** generate_random_matrix(int row, int col){
    int **returnMat = (int **)malloc(row * sizeof(int *));
    #pragma omp parallel for num_threads(threads)
    for (int i = 0; i < row; i++) {
        returnMat[i] = (int *)malloc(col * sizeof(int));
    }

    #pragma omp parallel for num_threads(threads)
    for (int i = 0; i < row; i++){
        for (int j = 0; j < col; j++){
            returnMat[i][j] = rand() % 10;
        }
    }

    return returnMat;
}

void print_mats(int** mat1, int** mat2, int** mat3, int row_a, int col_a_row_b, int col_b) {
    for (int i=0; i<row_a; i++) {
        for (int j=0; j<col_a_row_b; j++) {
            printf("%d ", mat1[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    for (int i=0; i<col_a_row_b; i++) {
        for (int j=0; j<col_b; j++) {
            printf("%d ", mat2[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    for (int i=0; i<row_a; i++) {
        for (int j=0; j<col_b; j++) {
            printf("%d ", mat3[i][j]);
        }
        printf("\n");
    }
}

void free_mats(int** mat1, int** mat2, int** mat3, int row_a, int col_a_row_b) {
    #pragma omp parallel for num_threads(threads) 
    for (int i=0; i<row_a; i++) {
        free(mat1[i]);
        free(mat3[i]);
    }
    #pragma omp parallel for num_threads(threads) 
    for (int i=0; i<col_a_row_b; i++) {
        free(mat2[i]);
    }
    free(mat1);
    free(mat2);
    free(mat3);
}

int main(int argc, char* argv[]) {
    srand(time(NULL));
    int row_a, col_a_row_b, col_b;
    int doPrint = 0;

    if (argc == 6) {
        threads = strtol(argv[1], NULL, 10);
        row_a = strtol(argv[2], NULL, 10);
        col_a_row_b = strtol(argv[3], NULL, 10);
        col_b = strtol(argv[4], NULL, 10);
        doPrint = strcmp(argv[5], "y") == 0;
        omp_set_num_threads(threads);
    }
    else {
        printf("Execution format is ./matrix_mp [# of threads] [row_a] [col_a/row_b] [col_b] [print? (y/n)]\n");
        return 0;
    }
    int** mat1 = generate_random_matrix(row_a,col_a_row_b);
    int** mat2 = generate_random_matrix(col_a_row_b,col_b);
    int** mat3 = matrix_mp(row_a, col_a_row_b, col_a_row_b, col_b, mat1, mat2);
    if (doPrint == 1) print_mats(mat1, mat2, mat3, row_a, col_a_row_b, col_b);
    free_mats(mat1, mat2, mat3, row_a, col_a_row_b);
    return 0;
}