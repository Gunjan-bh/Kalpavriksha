#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h> 

#define MAX_STUDENTS 100
#define NUM_SUBJECTS 3

struct Student {
    int roll;
    char name[50];
    int marks[NUM_SUBJECTS];
};

long long totalMarks(int marks[]);
float averageMarks(long long total);
char getGrade(float avg);
void printStars(char grade);
void printRollnoRecursive(int rolls[], int index, int n);
void trimWhitespace(char *str);
void sortRollNumbers(int rolls[], int n);
int isDuplicateRoll(struct Student s[], int count, int roll);

int main(void) {
    int n, i, j;
    struct Student s[MAX_STUDENTS];
    int rolls[MAX_STUDENTS];

    printf("Enter number of students (1-100): ");
    if (scanf("%d", &n) != 1) {
        printf("Invalid input\n");
        return 0;
    }

    if (n < 1 || n > 100) {
        printf("Invalid number of students\n");
        return 0;
    }
    while (getchar() != '\n'); 

    for (i = 0; i < n; i++) {
        printf("\nEnter details for student %d\n", i + 1);

        while (1) {
            printf("Enter Roll no.: ");
            if (scanf("%d", &s[i].roll) != 1) {
                printf("Invalid roll number input.\n");
                while (getchar() != '\n'); 
                continue;
            }

            if (s[i].roll < 0) {
                printf("Invalid roll number! Must be non-negative.\n");
                while (getchar() != '\n'); 
                continue;
            }

            if (isDuplicateRoll(s, i, s[i].roll)) {
                printf("Duplicate roll number is invalid! Please enter a unique roll.\n");
                while (getchar() != '\n'); 
                continue;
            }

            while (getchar() != '\n'); 
            break; 
        }

        printf("Enter Name: ");
        if (fgets(s[i].name, sizeof(s[i].name), stdin) == NULL) {
            s[i].name[0] = '\0';
        }
        trimWhitespace(s[i].name);

        for (j = 0; j < NUM_SUBJECTS; j++) {
            int valid = 0;
            while (!valid) {
                printf("Enter Marks%d: ", j + 1);
                if (scanf("%d", &s[i].marks[j]) != 1) {
                    printf("Invalid marks input.\n");
                    while (getchar() != '\n');
                    continue;
                }
                if (s[i].marks[j] < 0 || s[i].marks[j] > 100) {
                    printf("Invalid marks! Enter a positive number\n");
                    continue;
                }
                valid = 1;
            }
            while (getchar() != '\n');
        }
    }

    printf("\n--- Student Performance Report ---\n");
    for (i = 0; i < n; i++) {
        long long total = totalMarks(s[i].marks);
        if (total > INT_MAX) {
            printf("\nError: Total marks overflow for student %d\n", s[i].roll);
            continue; 
        }

        float avg = averageMarks(total);
        char grade = getGrade(avg);

        printf("\nRoll: %d\n", s[i].roll);
        printf("Name: %s\n", s[i].name);
        printf("Total: %lld\n", total);
        printf("Average: %.2f\n", avg);
        printf("Grade: %c\n", grade);

        if (grade == 'F') continue;

        printf("Performance: ");
        printStars(grade);
        printf("\n");
    }

    for (i = 0; i < n; i++) {
        rolls[i] = s[i].roll;
    }
    sortRollNumbers(rolls, n);

    printf("\nSorted list of Roll Numbers (via recursion): ");
    printRollnoRecursive(rolls, 0, n);
    printf("\n");

    return 0;
}

int isDuplicateRoll(struct Student s[], int count, int roll) {
    int i;
    for (i = 0; i < count; i++) {
        if (s[i].roll == roll)
            return 1;
    }
    return 0;
}

long long totalMarks(int marks[]) {
    long long total = 0;
    int i;
    for (i = 0; i < NUM_SUBJECTS; i++) {
        if (total > LLONG_MAX - marks[i]) { 
            printf("Error: Integer overflow while calculating total marks!\n");
            return LLONG_MAX;
        }
        total += marks[i];
    }
    return total;
}

float averageMarks(long long total) {
    return (float)total / NUM_SUBJECTS;
}

char getGrade(float avg) {
    if (avg >= 85) return 'A';
    else if (avg >= 70) return 'B';
    else if (avg >= 50) return 'C';
    else if (avg >= 35) return 'D';
    else return 'F';
}

void printStars(char grade) {
    int stars = 0;
    int k;
    switch (grade) {
        case 'A': stars = 5; break;
        case 'B': stars = 4; break;
        case 'C': stars = 3; break;
        case 'D': stars = 2; break;
        default: stars = 0; break;
    }
    for (k = 0; k < stars; k++) {
        printf("*");
    }
}

void printRollnoRecursive(int rolls[], int index, int n) {
    if (index == n)
	return;
    printf("%d ", rolls[index]);
    printRollnoRecursive(rolls, index + 1, n);
}

void trimWhitespace(char *str) {
    int start = 0;
	int end, len;
    if (str == NULL)
	return;

    len = strlen(str);
    end = len - 1;
    while (end >= 0 && isspace((unsigned char)str[end])) {
        str[end] = '\0';
        end--;
    }
    while (str[start] != '\0' && isspace((unsigned char)str[start])) 
	start++;

    if (start > 0) {
        int i;
        for (i = 0; str[start + i] != '\0'; i++) {
            str[i] = str[start + i];
        }
        str[i] = '\0';
    }
}

void sortRollNumbers(int rolls[], int n) {
    int i, j, temp;
    for (i = 0; i < n - 1; i++) {
        for (j = i + 1; j < n; j++) {
            if (rolls[i] > rolls[j]) {
                temp = rolls[i];
                rolls[i] = rolls[j];
                rolls[j] = temp;
            }
        }
    }
}
