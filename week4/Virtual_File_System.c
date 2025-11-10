#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define TOTAL_BLOCKS 1024
#define SECTOR_SIZE 512
#define MAX_FILE_SECTORS 32
#define NAME_LENGTH 50
#define INPUT_LENGTH 1024
#define DIR_DEPTH_LIMIT 128


typedef struct FileBlock {
    int sectorId;
    struct FileBlock *previous;
    struct FileBlock *next;
} FileBlock;

typedef enum { FILE_TYPE, DIR_TYPE } NodeType;

typedef struct FileNode {
    char name[NAME_LENGTH + 1];
    NodeType type;
    struct FileNode *next;
    struct FileNode *prev;
    struct FileNode *parent;
    union {
        struct { struct FileNode *firstChild; } dir;
        struct { int size; int sectors[MAX_FILE_SECTORS]; } file;
    };
} FileNode;

char virtualDisk[TOTAL_BLOCKS][SECTOR_SIZE];
FileNode *rootDir = NULL;
FileNode *currentDir = NULL;
FileBlock *freeBlockHead = NULL;
FileBlock *freeBlockTail = NULL;
int freeBlockCount = 0;

//function declaration
void initializeFileSystem();
void displayMenu();
void freeAllMemory();
void recursiveFree(FileNode *node);
void addFreeBlock(int id);
FileNode *findChild(FileNode *dir, char *name);
void createDirectory(char *name);
void createFile(char *name);
void writeFile(char *input);
void readFile(char *name);
void deleteFile(char *name);
void removeDirectory(char *name);
void listDirectory();
void changeDirectory(char *name);
void printWorkingDirectory();
void showDiskUsage();
void buildPath(char *buf, size_t size);
int getCommandCode(char *cmd);
void splitInput(char *input, char *cmd, char *arg);


int getCommandCode(char *cmd) {
    if (!strcmp(cmd, "mkdir")) return 1;
    if (!strcmp(cmd, "create")) return 2;
    if (!strcmp(cmd, "write")) return 3;
    if (!strcmp(cmd, "read")) return 4;
    if (!strcmp(cmd, "delete")) return 5;
    if (!strcmp(cmd, "rmdir")) return 6;
    if (!strcmp(cmd, "ls")) return 7;
    if (!strcmp(cmd, "cd")) return 8;
    if (!strcmp(cmd, "pwd")) return 9;
    if (!strcmp(cmd, "df")) return 10;
    if (!strcmp(cmd, "help")) return 12;
    if (!strcmp(cmd, "exit")) return 11;
    return 0;
}


void splitInput(char *input, char *cmd, char *arg) {
    int cmdLen = strcspn(input, " ");
    int safeLen = (cmdLen > NAME_LENGTH) ? NAME_LENGTH : cmdLen;
    strncpy(cmd, input, safeLen);
    cmd[safeLen] = '\0';

    char *argStart = input + cmdLen;
    while (*argStart == ' ') argStart++;
    strcpy(arg, argStart);
}

//Command menu
void displayMenu() {
    printf("\n-------- MINI VIRTUAL FILE SYSTEM COMMANDS --------n");
    printf(" mkdir <dir>          - Create a new directory\n");
    printf(" rmdir <dir>          - Remove an empty directory\n");
    printf(" create <file>        - Create a new file\n");
    printf(" write <file> \"text\" - Write text into a file\n");
    printf(" read <file>          - Display file content\n");
    printf(" delete <file>        - Delete an existing file\n");
    printf(" ls                   - List files in current directory\n");
    printf(" cd <dir>             - Change directory\n");
    printf(" cd ..                - Go up one level\n");
    printf(" pwd                  - Show current directory path\n");
    printf(" df                   - Show disk usage\n");
    printf(" help                 - Show command list again\n");
    printf(" exit                 - Exit the file system\n");
    printf("\n");
}

void initializeFileSystem() {
    memset(virtualDisk, 0, sizeof(virtualDisk));
    rootDir = malloc(sizeof(FileNode));

    strcpy(rootDir->name, "/");
    rootDir->type = DIR_TYPE;
    rootDir->parent = NULL;
    rootDir->next = rootDir->prev = rootDir;
    rootDir->dir.firstChild = NULL;
    currentDir = rootDir;

    for (int i = 0; i < TOTAL_BLOCKS; i++) {
        FileBlock *block = malloc(sizeof(FileBlock));
        block->sectorId = i;
        block->next = NULL;
        if (!freeBlockHead) {
            block->previous = NULL;
            freeBlockHead = freeBlockTail = block;
        } else {
            block->previous = freeBlockTail;
            freeBlockTail->next = block;
            freeBlockTail = block;
        }
        freeBlockCount++;
    }

    printf("File System Initialized with %d free blocks.\n", freeBlockCount);
}


FileNode *findChild(FileNode *dir, char *name) {
    FileNode *child = dir->dir.firstChild;
    if (!child) return NULL;
    FileNode *cur = child;
    do {
        if (strcmp(cur->name, name) == 0)
            return cur;
        cur = cur->next;
    } while (cur != child);
    return NULL;
}

void createDirectory(char *name) {
    if (!name || !*name) { printf("Name required.\n"); return; }
    if (findChild(currentDir, name)) { printf("Already exists.\n"); return; }

    FileNode *newDir = malloc(sizeof(FileNode));
    strcpy(newDir->name, name);
    newDir->type = DIR_TYPE;
    newDir->parent = currentDir;
    newDir->dir.firstChild = NULL;

    FileNode *head = currentDir->dir.firstChild;
    if (!head) {
        currentDir->dir.firstChild = newDir;
        newDir->next = newDir->prev = newDir;
    } else {
        FileNode *tail = head->prev;
        tail->next = newDir;
        newDir->prev = tail;
        newDir->next = head;
        head->prev = newDir;
    }
    printf("Directory '%s' created.\n", name);
}

void removeDirectory(char *name) {
    FileNode *target = findChild(currentDir, name);

    if (!target) 
    { printf("Directory not found.\n"); 
        return; 
    }
    if (target->type != DIR_TYPE) 
    { printf("It is not a directory.\n"); 
        return; 
    }
    if (target->dir.firstChild) 
    { printf("Directory is not empty.\n"); 
        return; 
    }

    FileNode *head = currentDir->dir.firstChild;
    if (target == head && target->next == target) {
        currentDir->dir.firstChild = NULL;
    } 
    else {
        if (target == head)
        currentDir->dir.firstChild = target->next;
        target->prev->next = target->next;
        target->next->prev = target->prev;
    }

    free(target);
    printf("Directory removed.\n");
}

void createFile(char *name) {
    if (!name || !*name) 
    { printf("Name required.\n"); 
        return; 
    }
    if (findChild(currentDir, name))
    { printf("Directory already exists.\n");
        return; 
    }

    FileNode *newFile = malloc(sizeof(FileNode));
    strcpy(newFile->name, name);
    newFile->type = FILE_TYPE;
    newFile->parent = currentDir;
    newFile->file.size = 0;
    for (int i = 0; i < MAX_FILE_SECTORS; i++) 
    newFile->file.sectors[i] = -1;

    FileNode *head = currentDir->dir.firstChild;
    if (!head) {
        currentDir->dir.firstChild = newFile;
        newFile->next = newFile->prev = newFile;
    } else {
        FileNode *tail = head->prev;
        tail->next = newFile;
        newFile->prev = tail;
        newFile->next = head;
        head->prev = newFile;
    }
    printf("File named '%s' created.\n", name);
}

void listDirectory() {
    FileNode *head = currentDir->dir.firstChild;
    if (!head) 
    { printf("(empty)\n"); 
        return; 
    }
    FileNode *cur = head;
    do {
        printf("%s%s  ", cur->name, cur->type == DIR_TYPE ? "/" : "");
        cur = cur->next;
    } 
    while (cur != head);
    printf("\n");
}

void changeDirectory(char *name) 
{
    if (strcmp(name, "..") == 0) 
    {
        if (currentDir->parent)
            currentDir = currentDir->parent;
        else
            printf("Already at root.\n");
        return;
    }

    FileNode *target = findChild(currentDir, name);
    if (!target || target->type != DIR_TYPE) {
        printf("Directory not found.\n");
        return;
    }

    currentDir = target;
}

void buildPath(char *buf, size_t size) {
    if (currentDir == rootDir) 
    { snprintf(buf, size, "/"); 
        return; 
    }
    FileNode *stack[DIR_DEPTH_LIMIT];
    int depth = 0;
    FileNode *node = currentDir;
    while (node != rootDir && depth < DIR_DEPTH_LIMIT) {
        stack[depth++] = node;
        node = node->parent;
    }
    char *ptr = buf;
    for (int i = depth - 1; i >= 0; i--) {
        ptr += snprintf(ptr, size - (ptr - buf), "/%s", stack[i]->name);
    }
}

void printWorkingDirectory() {
    char path[INPUT_LENGTH];
    buildPath(path, sizeof(path));
    printf("Current Directory: %s\n", path);
}

void showDiskUsage() {
    int used = TOTAL_BLOCKS - freeBlockCount;
    printf("Total Blocks: %d | Used: %d | Free: %d | Usage: %.2f%%\n", TOTAL_BLOCKS, used, freeBlockCount, (float)used * 100 / TOTAL_BLOCKS);
}

void recursiveFree(FileNode *node) {
    if (!node) return;
    if (node->type == DIR_TYPE && node->dir.firstChild) {
        FileNode *child = node->dir.firstChild;
        child->prev->next = NULL;
        while (child) {
            FileNode *next = child->next;
            recursiveFree(child);
            child = next;
        }
    }
    free(node);
}

void freeAllMemory() {
    recursiveFree(rootDir);
    FileBlock *blk = freeBlockHead;
    while (blk) {
        FileBlock *next = blk->next;
        free(blk);
        blk = next;
    }
    printf("Memory released successfully.\n");
}

//main function
int main() {
    printf("------MINI VIRTUAL FILE SYSTEM------\n");
    initializeFileSystem();
    displayMenu();

    char cmd[NAME_LENGTH + 1], arg[INPUT_LENGTH], input[INPUT_LENGTH], path[INPUT_LENGTH];
    int code;

    do {
        buildPath(path, sizeof(path));
        printf("%s> ", path);

        if (!fgets(input, sizeof(input), stdin)) 
        { code = 11; printf("\n"); 
            continue; 
        }

        if (!strchr(input, '\n')) {
            printf("Input too long.\n");
            int c; 
            while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }

        input[strcspn(input, "\n")] = '\0';
        if (input[0] == '\0') continue;

        splitInput(input, cmd, arg);
        code = getCommandCode(cmd);

        switch (code) {
            case 1: createDirectory(arg); break;
            case 2: createFile(arg); break;
            case 6: removeDirectory(arg); break;
            case 7: listDirectory(); break;
            case 8: changeDirectory(arg); break;
            case 9: printWorkingDirectory(); break;
            case 10: showDiskUsage(); break;
            case 12: displayMenu(); break;
            case 11: printf("Exiting File System...\n"); break;
            default: printf("Unknown command: %s (use 'help' to list commands)\n", cmd);
        }

    } while (code != 11);

    freeAllMemory();
    printf("Program exited successfully.\n");
    return 0;
}