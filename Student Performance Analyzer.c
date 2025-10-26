#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h> 

#define MAX_STUDENTS 100
#define NUM_SUBJECTS 3
#define MAX_NAME_LENGTH 50
#define MAX_MARKS 100
#define MIN_MARKS 0
#define MIN_ROLL 0

struct Student {
    int rollNumber;
    char name[MAX_NAME_LENGTH];
    int marks[NUM_SUBJECTS];
};

long long calculateTotalMarks(int marks[]);
float calculateAverageMarks(long long totalMarks);
char calculateGrade(float averageMarks);
void displayPerformanceStars(char grade);
void displayRollNumbersRecursive(int rollNumbers[], int currentIndex, int totalStudents);
void trimWhitespace(char *string);
void sortRollNumbersAscending(int rollNumbers[], int totalStudents);
int isDuplicateRollNumber(struct Student students[], int totalEntered, int rollNumber);

int main(void) {
    int totalStudents;
    struct Student students[MAX_STUDENTS];
    int rollNumbers[MAX_STUDENTS];

    printf("Enter number of students (1-%d): ", MAX_STUDENTS);
    if (scanf("%d", &totalStudents) != 1) {
        printf("Invalid input\n");
        return 0;
    }

    if (totalStudents < 1 || totalStudents > MAX_STUDENTS) {
        printf("Invalid number of students\n");
        return 0;
    }
    while (getchar() != '\n'); 

    for (int studentIndex = 0; studentIndex < totalStudents; studentIndex++) {
        printf("\nEnter details for student %d\n", studentIndex + 1);

        while (1) {
            printf("Enter Roll no.: ");
            if (scanf("%d", &students[studentIndex].rollNumber) != 1) {
                printf("Invalid roll number input.\n");
                while (getchar() != '\n'); 
                continue;
            }

            if (students[studentIndex].rollNumber < MIN_ROLL) {
                printf("Invalid roll number! Must be non-negative.\n");
                while (getchar() != '\n'); 
                continue;
            }

            if (isDuplicateRollNumber(students, studentIndex, students[studentIndex].rollNumber)) {
                printf("Duplicate roll number is invalid! Please enter a unique roll.\n");
                while (getchar() != '\n'); 
                continue;
            }

            while (getchar() != '\n'); 
            break; 
        }

        printf("Enter Name: ");
        if (fgets(students[studentIndex].name, sizeof(students[studentIndex].name), stdin) == NULL) {
            students[studentIndex].name[0] = '\0';
        }
        trimWhitespace(students[studentIndex].name);

        for (int subjectIndex = 0; subjectIndex < NUM_SUBJECTS; subjectIndex++) {
            int validInput = 0;
            while (!validInput) {
                printf("Enter Marks%d: ", subjectIndex + 1);
                if (scanf("%d", &students[studentIndex].marks[subjectIndex]) != 1) {
                    printf("Invalid marks input.\n");
                    while (getchar() != '\n');
                    continue;
                }
                if (students[studentIndex].marks[subjectIndex] < MIN_MARKS || 
                    students[studentIndex].marks[subjectIndex] > MAX_MARKS) {
                    printf("Invalid marks! Enter a number between %d and %d\n", MIN_MARKS, MAX_MARKS);
                    continue;
                }
                validInput = 1;
            }
            while (getchar() != '\n');
        }
    }

    printf("\n--- Student Performance Report ---\n");
    for (int studentIndex = 0; studentIndex < totalStudents; studentIndex++) {
        long long totalMarks = calculateTotalMarks(students[studentIndex].marks);
        if (totalMarks > INT_MAX) {
            printf("\nError: Total marks overflow for student %d\n", students[studentIndex].rollNumber);
            continue; 
        }

        float averageMarks = calculateAverageMarks(totalMarks);
        char grade = calculateGrade(averageMarks);

        printf("\nRoll: %d\n", students[studentIndex].rollNumber);
        printf("Name: %s\n", students[studentIndex].name);
        printf("Total: %lld\n", totalMarks);
        printf("Average: %.2f\n", averageMarks);
        printf("Grade: %c\n", grade);

        if (grade == 'F') continue;

        printf("Performance: ");
        displayPerformanceStars(grade);
        printf("\n");
    }

    for (int studentIndex = 0; studentIndex < totalStudents; studentIndex++) {
        rollNumbers[studentIndex] = students[studentIndex].rollNumber;
    }
    sortRollNumbersAscending(rollNumbers, totalStudents);

    printf("\nSorted list of Roll Numbers (via recursion): ");
    displayRollNumbersRecursive(rollNumbers, 0, totalStudents);
    printf("\n");

    return 0;
}

int isDuplicateRollNumber(struct Student students[], int totalEntered, int rollNumber) {
    for (int checkIndex = 0; checkIndex < totalEntered; checkIndex++) {
        if (students[checkIndex].rollNumber == rollNumber)
            return 1;
    }
    return 0;
}

long long calculateTotalMarks(int marks[]) {
    long long totalMarks = 0;
    for (int subjectIndex = 0; subjectIndex < NUM_SUBJECTS; subjectIndex++) {
        if (totalMarks > LLONG_MAX - marks[subjectIndex]) { 
            printf("Error: Integer overflow while calculating total marks!\n");
            return LLONG_MAX;
        }
        totalMarks += marks[subjectIndex];
    }
    return totalMarks;
}

float calculateAverageMarks(long long totalMarks) {
    return (float)totalMarks / NUM_SUBJECTS;
}

char calculateGrade(float averageMarks) {
    const int GRADE_A = 85;
    const int GRADE_B = 70;
    const int GRADE_C = 50;
    const int GRADE_D = 35;

    if (averageMarks >= GRADE_A) return 'A';
    else if (averageMarks >= GRADE_B) return 'B';
    else if (averageMarks >= GRADE_C) return 'C';
    else if (averageMarks >= GRADE_D) return 'D';
    else return 'F';
}

void displayPerformanceStars(char grade) {
    int starCount = 0;
    switch (grade) {
        case 'A': starCount = 5; break;
        case 'B': starCount = 4; break;
        case 'C': starCount = 3; break;
        case 'D': starCount = 2; break;
        default: starCount = 0; break;
    }
    for (int starIndex = 0; starIndex < starCount; starIndex++) {
        printf("*");
    }
}

void displayRollNumbersRecursive(int rollNumbers[], int currentIndex, int totalStudents) {
    if (currentIndex == totalStudents)
        return;
    printf("%d ", rollNumbers[currentIndex]);
    displayRollNumbersRecursive(rollNumbers, currentIndex + 1, totalStudents);
}

void trimWhitespace(char *string) {
    if (string == NULL)
        return;

    int startIndex = 0;
    int length = strlen(string);
    int endIndex = length - 1;

    while (endIndex >= 0 && isspace((unsigned char)string[endIndex])) {
        string[endIndex] = '\0';
        endIndex--;
    }

    while (string[startIndex] != '\0' && isspace((unsigned char)string[startIndex])) 
        startIndex++;

    if (startIndex > 0) {
        int moveIndex;
        for (moveIndex = 0; string[startIndex + moveIndex] != '\0'; moveIndex++) {
            string[moveIndex] = string[startIndex + moveIndex];
        }
        string[moveIndex] = '\0';
    }
}

void sortRollNumbersAscending(int rollNumbers[], int totalStudents) {
    for (int outerIndex = 0; outerIndex < totalStudents - 1; outerIndex++) {
        for (int innerIndex = outerIndex + 1; innerIndex < totalStudents; innerIndex++) {
            if (rollNumbers[outerIndex] > rollNumbers[innerIndex]) {
                int temp = rollNumbers[outerIndex];
                rollNumbers[outerIndex] = rollNumbers[innerIndex];
                rollNumbers[innerIndex] = temp;
            }
        }
    }
}
