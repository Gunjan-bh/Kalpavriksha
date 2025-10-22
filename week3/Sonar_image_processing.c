#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MIN_MATRIX_SIZE 2
#define MAX_MATRIX_SIZE 10
#define MAX_PIXEL_VALUE 255
#define FILTER_SIZE 3

// Function declarations
void generateRandomMatrix(int size, int matrix[][size]);
void printMatrix(int size, int matrix[][size]);
void rotateMatrixClockwise90(int size, int matrix[][size]);
void applySmoothingFilter(int size, int matrix[][size]);

int main() {
    int size;

    printf("Enter the size of the matrix (%d %d): ", MIN_MATRIX_SIZE, MAX_MATRIX_SIZE);

    // Validate matrix size input
    while (1) {
        if (scanf("%d", &size) == 1 && size >= MIN_MATRIX_SIZE && size <= MAX_MATRIX_SIZE) {
            // Clear buffer
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            break;
        }
        printf("Invalid input. Please enter a number between %d and %d: ", MIN_MATRIX_SIZE, MAX_MATRIX_SIZE);
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    }

    int matrix[size][size];

    // Generate and display the matrix
    generateRandomMatrix(size, matrix);
    printf("\nOriginal Randomly Generated Matrix:\n");
    printMatrix(size, matrix);

    // Rotate the matrix 90 degrees clockwise
    rotateMatrixClockwise90(size, matrix);
    printf("\nMatrix after 90� Clockwise Rotation:\n");
    printMatrix(size, matrix);

    // Apply 3x3 smoothing filter
    applySmoothingFilter(size, matrix);
    printf("\nMatrix after Applying %dx%d Smoothing Filter:\n", FILTER_SIZE, FILTER_SIZE);
    printMatrix(size, matrix);

    return 0;
}

/* 
 * Generates a random matrix of given size with values between 0 and MAX_PIXEL_VALUE.
 */
void generateRandomMatrix(int size, int matrix[][size]) {
    srand((unsigned int)time(NULL));

    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size; col++) {
            matrix[row][col] = rand() % (MAX_PIXEL_VALUE + 1);
        }
    }
}

/*
 * Displays the matrix in a formatted way.
 */
void printMatrix(int size, int matrix[][size]) {
    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size; col++) {
            printf("%-4d ", matrix[row][col]);
        }
        printf("\n");
    }
}

/*
 * Rotates the given square matrix 90� clockwise.
 */
void rotateMatrixClockwise90(int size, int matrix[][size]) {
    // Transpose the matrix
    for (int row = 0; row < size; row++) {
        for (int col = row; col < size; col++) {
            int temp = matrix[row][col];
            matrix[row][col] = matrix[col][row];
            matrix[col][row] = temp;
        }
    }

    // Reverse each row
    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size / 2; col++) {
            int temp = matrix[row][col];
            matrix[row][col] = matrix[row][size - 1 - col];
            matrix[row][size - 1 - col] = temp;
        }
    }
}

/*
 * Applies a 3x3 (FILTER_SIZE x FILTER_SIZE) smoothing filter to the matrix.
 * Each element is replaced with the average of its neighboring elements.
 */
void applySmoothingFilter(int size, int matrix[][size]) {
    int previousRowCopy[size];
    int currentRowCopy[size];

    memcpy(previousRowCopy, matrix, size * sizeof(int));

    for (int row = 0; row < size; row++) {
        memcpy(currentRowCopy, matrix[row], size * sizeof(int));

        for (int col = 0; col < size; col++) {
            int sum = 0;
            int count = 0;

            // Apply filter window
            for (int rowOffset = -1; rowOffset <= 1; rowOffset++) {
                for (int colOffset = -1; colOffset <= 1; colOffset++) {
                    int neighborRow = row + rowOffset;
                    int neighborCol = col + colOffset;

                    if (neighborRow >= 0 && neighborRow < size && neighborCol >= 0 && neighborCol < size) {
                        if (neighborRow == row) {
                            sum += currentRowCopy[neighborCol];
                        } else if (neighborRow == row - 1) {
                            sum += previousRowCopy[neighborCol];
                        } else {
                            sum += matrix[neighborRow][neighborCol];
                        }
                        count++;
                    }
                }
            }

            matrix[row][col] = sum / count;
        }

        memcpy(previousRowCopy, currentRowCopy, size * sizeof(int));
    }
}

