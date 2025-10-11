#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_STUDENTS 100
#define NUM_SUBJECTS 3

struct Student {
    int roll;
    char name[50];
    int marks[NUM_SUBJECTS];
};

int totalMarks(int marks[]);
float averageMarks(int total);
char getGrade(float avg);
void printStars(char grade);
void printRollnoRecursive(int rolls[], int index, int n);
void trimWhitespace(char *str);
void sortRollNumbers(int rolls[], int n);

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
    getchar(); 

    for (i = 0; i < n; i++) {
        printf("\nEnter details for student %d\n", i + 1);

        printf("Enter Roll no.: ");
        if (scanf("%d", &s[i].roll) != 1) {
            printf("Invalid roll number input.\n");
            return 0;
        }
        getchar(); 

        printf("Enter Name: ");
        if (fgets(s[i].name, sizeof(s[i].name), stdin) == NULL) {
            s[i].name[0] = '\0';
        }
        trimWhitespace(s[i].name);

        for (j = 0; j < NUM_SUBJECTS; j++) {
            printf("Enter Marks%d: ", j + 1);
            if (scanf("%d", &s[i].marks[j]) != 1) {
                printf("Invalid marks input.\n");
                return 0;
            }

            if (s[i].marks[j] < 0 || s[i].marks[j] > 100) {
                printf("Invalid marks! Please enter between 0 and 100.\n");
                j--; 
            }
        }
        getchar(); 
    }

    printf("\n--- Student Performance Report ---\n");
    for (i = 0; i < n; i++) {
        int total = totalMarks(s[i].marks);
        float avg = averageMarks(total);
        char grade = getGrade(avg);

        printf("\nRoll: %d\n", s[i].roll);
        printf("Name: %s\n", s[i].name);
        printf("Total: %d\n", total);
        printf("Average: %.2f\n", avg);
        printf("Grade: %c\n", grade);

        if (grade == 'F') {
            continue;
        }

        printf("Performance: ");
        printStars(grade);
        printf("\n");
    }

    for (i = 0; i < n; i++) {
        rolls[i] = s[i].roll;
    }
    sortRollNumbers(rolls, n);

    printf("\nList of Roll Numbers (sorted, via recursion): ");
    printRollnoRecursive(rolls, 0, n);
    printf("\n");

    return 0;
}

int totalMarks(int marks[]) {
    int total = 0;
    int i;
    for (i = 0; i < NUM_SUBJECTS; i++) {
        total += marks[i];
    }
    return total;
}

float averageMarks(int total) {
    return (float)total / NUM_SUBJECTS;
}

char getGrade(float avg) {
    if (avg >= 85)
        return 'A';
    else if (avg >= 70)
        return 'B';
    else if (avg >= 50)
        return 'C';
    else if (avg >= 35)
        return 'D';
    else
        return 'F';
}

void printStars(char grade) {
    int stars = 0;
    int k;
    switch (grade) {
        case 'A':
		    stars = 5; 
			break;
        case 'B': 
		    stars = 4; 
			break;
        case 'C': 
		    stars = 3; 
			break;
        case 'D': 
		    stars = 2; 
			break;
        default: 
		    stars = 0; 
			break;
    }

    for (k = 0; k < stars; k++) {
        printf("*");
    }
}

void printRollnoRecursive(int rolls[], int index, int n) {
    if (index == n) return;
    printf("%d ", rolls[index]);
    printRollnoRecursive(rolls, index + 1, n);
}

void trimWhitespace(char *str) {
    int start = 0, end;
    int len;

    if (str == NULL)
        return;

    len = strlen(str);
  
    end = len - 1;
    while (end >= 0 && isspace((unsigned char)str[end])) {
        str[end] = '\0';
        end--;
    }

    while (str[start] != '\0' && isspace((unsigned char)str[start])) {
        start++;
    }

    if (start > 0) {
        memmove(str, str + start, strlen(str + start) + 1);
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

