#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_TABLE_SIZE 2003          
#define MAX_VALUE_LENGTH 100  

int REJECT_DUPLICATES = 0; 

typedef struct CacheNode {
    int key;
    char value[MAX_VALUE_LENGTH];
    struct CacheNode *prev, *next; 
} CacheNode;


typedef struct HashEntry {
    int key;
    CacheNode *cacheNode; 
    struct HashEntry *nextValue; 
} HashEntry;

typedef struct {
    int maxCapacity; 
    int currentSize; 
    CacheNode *head; 
    CacheNode *tail; 
    HashEntry *hashMap[HASH_TABLE_SIZE];
} LRUCache;

int calculateHashIndex(int key) {
    if (key < 0) key = -key;
    return key % HASH_TABLE_SIZE; 
}

void mapInsert(LRUCache *cache, int key, CacheNode *insertNode) {
    int hashIndex = calculateHashIndex(key); 
    HashEntry *newEntry = (HashEntry*)malloc(sizeof(HashEntry));
    newEntry->key = key;
    newEntry->cacheNode = insertNode;
    newEntry->nextValue = cache->hashMap[hashIndex];
    cache->hashMap[hashIndex] = newEntry;
}

CacheNode* mapGetNode(LRUCache *cache, int key) {
    int hashIndex = calculateHashIndex(key);
    HashEntry *currentEntry = cache->hashMap[hashIndex]; 

    while (currentEntry) {
        if (currentEntry->key == key)
            return currentEntry->cacheNode;
        currentEntry = currentEntry->nextValue;
    }
    return NULL;
}

void mapDeleteEntry(LRUCache *cache, int key) {
    int hashIndex = calculateHashIndex(key);
    HashEntry *currentEntry = cache->hashMap[hashIndex];
    HashEntry *previousEntry = NULL; 

    while (currentEntry) {
        if (currentEntry->key == key) {
            if (previousEntry) previousEntry->nextValue = currentEntry->nextValue;
            else cache->hashMap[hashIndex] = currentEntry->nextValue;
            free(currentEntry);
            return;
        }
        previousEntry = currentEntry;
        currentEntry = currentEntry->nextValue;
    }
}

void addNodeToHead(LRUCache *cache, CacheNode *addNode) {
    addNode->prev = NULL;
    addNode->next = cache->head;

    if (cache->head)
        cache->head->prev = addNode;

    cache->head = addNode;

    if (cache->tail == NULL)
        cache->tail = addNode;
}

void removeExistingNode(LRUCache *cache, CacheNode *removeNode) {
    if (removeNode->prev)
        removeNode->prev->next = removeNode->next;
    else
        cache->head = removeNode->next;

    if (removeNode->next)
        removeNode->next->prev = removeNode->prev;
    else
        cache->tail = removeNode->prev;
}

void updateNodeToMRU(LRUCache *cache, CacheNode *updateNode) {
    removeExistingNode(cache, updateNode);
    addNodeToHead(cache, updateNode);
}

CacheNode* removeLRUTail(LRUCache *cache) {
    if (cache->tail == NULL) {
        printf("Nothing remove from an empty list.\n");
        return NULL;
    }
    CacheNode *leastRecentlyUsedNode = cache->tail; 
    removeExistingNode(cache, leastRecentlyUsedNode);
    return leastRecentlyUsedNode;
}

LRUCache* createLRUCache(int capacity) { 
    if (capacity <= 0 || capacity > MAX_VALUE_LENGTH) { 
        printf("Invalid capacity. Must be 1–%d.\n", MAX_VALUE_LENGTH);
        return NULL;
    }

    LRUCache *cache = (LRUCache*)malloc(sizeof(LRUCache));
    if (cache == NULL) {
        perror("Failed to allocate memory for cache");
        return NULL;
    }

    cache->maxCapacity = capacity;
    cache->currentSize = 0;
    cache->head = cache->tail = NULL;

    for (int i = 0; i < HASH_TABLE_SIZE; i++) 
        cache->hashMap[i] = NULL;

    return cache;
}

char* getCacheValue(LRUCache *cache, int key) { 
    if (!cache) return NULL;

    CacheNode *node_found = mapGetNode(cache, key); 
    if (!node_found)
        return NULL;

    updateNodeToMRU(cache, node_found); 
    return node_found->value;
}

void putCacheValue(LRUCache *cache, int key, const char *value) { 
    if (!cache) {
        printf("Cache not created.\n");
        return;
    }

    if (key < 0) {
        printf("Invalid. Key must be non-negative.\n");
        return;
    }

    if (strlen(value) == 0) {
        printf("Empty value not allowed.\n");
        return;
    }
    if (strlen(value) >= MAX_VALUE_LENGTH) {
        printf("Value  is too long. Max length is %d.\n", MAX_VALUE_LENGTH - 1);
        return;
    }

    CacheNode *existingNode = mapGetNode(cache, key); 

    if (existingNode && REJECT_DUPLICATES) {
        printf("Duplicate key not allowed.\n");
        return;
    }

    if (existingNode) {
        strcpy(existingNode->value, value);
        updateNodeToMRU(cache, existingNode);
        return;
    }

    CacheNode *newNode = (CacheNode*)malloc(sizeof(CacheNode)); 
    if (newNode == NULL) {
        perror("Failed to allocate memory for new node");
        return;
    }

    newNode->key = key;
    strcpy(newNode->value, value); 
    newNode->prev = newNode->next = NULL;

    addNodeToHead(cache, newNode);
    mapInsert(cache, key, newNode);
    cache->currentSize++;

    if (cache->currentSize > cache->maxCapacity) {
        CacheNode *lruNode = removeLRUTail(cache); 
        if (lruNode) {
            mapDeleteEntry(cache, lruNode->key);
            free(lruNode);
            cache->currentSize--;
        }
    }
}

int main() {
    char commandBuffer[50]; 
    LRUCache *cache = NULL;

    printf("LRU Cache Console Interface (Commands: createCache <cap>, put <key> <value>, get <key>, exit)\n");
    
    char input_value[MAX_VALUE_LENGTH]; 

    while (scanf("%49s", commandBuffer) != EOF) { 

        if (strcmp(commandBuffer, "createCache") == 0) {
            int capacity_input;
            if (scanf("%d", &capacity_input) == 1) {
                cache = createLRUCache(capacity_input);
            } else {
                printf("Error: Missing capacity argument for createCache.\n");
            }
        }

        else if (strcmp(commandBuffer, "put") == 0) {
            int keyInput;
            if (scanf("%d %99s", &keyInput, input_value) == 2) { 
                putCacheValue(cache, keyInput, input_value);
            } else {
                printf("Error: Missing key or value argument for put.\n");
            }
        }

        else if (strcmp(commandBuffer, "get") == 0) {
            int keyInput;
            if (scanf("%d", &keyInput) == 1) {
                char *retrieved_value = getCacheValue(cache, keyInput); 
                printf("%s\n", retrieved_value ? retrieved_value : "NULL");
            } else {
                printf("Error: Missing key argument for get.\n");
            }
        }

        else if (strcmp(commandBuffer, "exit") == 0) {
            break;
        }

        else {
            printf("Invalid command: %s\n", commandBuffer);
        }
    }
    return 0;
}