#include <stdio.h>
#include <stdlib.h>

int** matrix_mp(int m, int p, int (*mat1)[m], int (*mat2)[p]) {
    int n,q;
    n = sizeof(mat1[0])/sizeof(mat1[0][0]);
    q = sizeof(mat2[0])/sizeof(mat2[0][0]);

    if (n != p) return 0;

    int** result;
    result = malloc(sizeof(int*) * m);
    for (int i=0; i<m; i++) result[i] = malloc(sizeof(int) * q);
    
    for (int i=0; i<m; i++) {
        for (int j=0; j<q; j++) {
            for (int k=0; k<p; k++) {
                result[i][j] = result[i][j] + (mat1[i][k]*mat2[k][j]);
            }
        }
    }

    return result;
}

int main() {
    int mat1[2][3] = {{1,2,3},{4,5,6}};
    int mat2[3][2] = {{7,8},{9,10},{11,12}};
    int** mat3= matrix_mp(sizeof(mat1)/sizeof(mat1[0]), sizeof(mat1)/sizeof(mat1[0]), mat1, mat2);
    for (int i=0; i<2; i++) {
        for (int j=0; j<2; j++) {
            printf("%d ",mat3[i][j]);
        }
        printf("\n");
    }
    return 0;
}