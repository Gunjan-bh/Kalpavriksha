#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MIN_MATRIX_SIZE 2
#define MAX_MATRIX_SIZE 10
#define MAX_PIXEL_VALUE 255
#define FILTER_SIZE 3

int getMatrixSize();
void fillMatrixRandomly(int *mat, int n);
void showMatrix(int *mat, int n, const char *msg);
void rotate90Clockwise(int *mat, int n);
void apply3x3Filter(int *mat, int n);

// Main Function 
int main() {
    int n = getMatrixSize();

    int myMatrix[n][n];
    int *ptr = &myMatrix[0][0];

    fillMatrixRandomly(ptr, n);
    showMatrix(ptr, n, "\nOriginal Random Matrix:");

    rotate90Clockwise(ptr, n);
    showMatrix(ptr, n, "\nMatrix after 90° Clockwise Rotation:");

    apply3x3Filter(ptr, n);
    showMatrix(ptr, n, "\nMatrix after 3x3 Smoothing Filter:");

    return 0;
}

// Ask user for matrix size 
int getMatrixSize() {
    int size;
    printf("Enter matrix size (2-10): ");
    while (1) {
        if (scanf("%d", &size) != 1) {
            printf("Invalid input! Enter a number: ");
            while (getchar() != '\n'); // clear input buffer
            continue;
        }
        if (size < 2 || size > 10) {
            printf("Size must be 2-10. Try again: ");
            continue;
        }
        break;
    }
    return size;
}

// Fill matrix with values 0-255
void fillMatrixRandomly(int *mat, int n) {
    srand(time(NULL));
    for (int i = 0; i < n * n; i++) {
        *(mat + i) = rand() % 256;
    }
}

// Print matrix
void showMatrix(int *mat, int n, const char *msg) {
    printf("%s\n", msg);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%4d", *(mat + i * n + j));
        }
        printf("\n");
    }
}

// Rotate matrix 90° clockwise
void rotate90Clockwise(int *mat, int n) {
    // Transpose
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            int temp = *(mat + i * n + j);
            *(mat + i * n + j) = *(mat + j * n + i);
            *(mat + j * n + i) = temp;
        }
    }

    // Reverse each row
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n / 2; j++) {
            int temp = *(mat + i * n + j);
            *(mat + i * n + j) = *(mat + i * n + (n - j - 1));
            *(mat + i * n + (n - j - 1)) = temp;
        }
    }
}

// Apply 3x3 smoothing filter
void apply3x3Filter(int *mat, int n) {
    int tempMat[n * n];

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int total = 0, count = 0;
            for (int di = -1; di <= 1; di++) {
                for (int dj = -1; dj <= 1; dj++) {
                    int ni = i + di;
                    int nj = j + dj;
                    if (ni >= 0 && ni < n && nj >= 0 && nj < n) {
                        total += *(mat + ni * n + nj);
                        count++;
                    }
                }
            }
            *(tempMat + i * n + j) = total / count;
        }
    }

    // Copy back
    for (int i = 0; i < n * n; i++) {
        *(mat + i) = *(tempMat + i);
    }
}
