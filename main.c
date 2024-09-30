#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include <string.h>

#define MAX_CACHE_SIZE 1000000000

typedef struct Node {
    int key;
    int value;
    struct Node* prev;
    struct Node* next;
} Node;

typedef struct {
    int capacity;
    int size;
    Node* head;
    Node* tail;
    Node** map;
    int hits;
    int misses;
} FIFOCache;

FIFOCache* createCache(int capacity) {
    FIFOCache* cache = (FIFOCache*)malloc(sizeof(FIFOCache));
    cache->capacity = capacity;
    cache->size = 0;
    cache->head = NULL;
    cache->tail = NULL;
    cache->map = (Node**)calloc(MAX_CACHE_SIZE, sizeof(Node*));
    cache->hits = 0;
    cache->misses = 0;
    return cache;
}

void addToTail(FIFOCache* cache, Node* node) {
    node->next = NULL;
    node->prev = cache->tail;
    if (cache->tail) {
        cache->tail->next = node;
    }
    cache->tail = node;
    if (!cache->head) {
        cache->head = node;
    }
}

int get(FIFOCache* cache, int key, int* wasCacheHit) {
    if (cache->map[key]) {
        cache->hits++;
        *wasCacheHit = 1; // Cache hit
        return cache->map[key]->value;
    }
    cache->misses++;
    *wasCacheHit = 0; // Cache miss
    return -1;
}

void put(FIFOCache* cache, int key, int value) {
    if (cache->map[key]) {
        // Update existing node
        cache->map[key]->value = value;
    } else {
        Node* newNode = (Node*)malloc(sizeof(Node));
        newNode->key = key;
        newNode->value = value;
        cache->map[key] = newNode;
        addToTail(cache, newNode);
        cache->size++;
        if (cache->size > cache->capacity) {
            // Remove the oldest (head) node
            Node* oldHead = cache->head;
            cache->head = oldHead->next;
            if (cache->head) {
                cache->head->prev = NULL;
            } else {
                cache->tail = NULL;
            }
            cache->map[oldHead->key] = NULL;
            free(oldHead);
            cache->size--;
        }
    }
}

float getCacheHitRate(FIFOCache* cache) {
    int total = cache->hits + cache->misses;
    if (total == 0) return 0.0f;
    return (float)cache->hits / total * 100.0f;
}

int collatz(int num) {
    int steps = 0;
    while (num != 1) {
        if (num % 2 == 0) { num = num / 2; }
        else { num = 3 * num + 1; }
        steps++;
    }
    return steps;
}

FIFOCache* cache;

int collatz_with_cache(int num, int* wasCacheHit) {
    if (num <= 1 || num > MAX_CACHE_SIZE) {
        *wasCacheHit = 0; // No caching for these numbers
        return collatz(num);
    }

    int cachedResult = get(cache, num, wasCacheHit);
    if (*wasCacheHit) {
        return cachedResult;
    }

    int steps = collatz(num);
    put(cache, num, steps);
    return steps;
}

int rd_num(int min, int max) {
    return (rand() % (max - min + 1) + min);
}

void calculate_steps(int testValue, int minValue, int maxValue, int useCache) {
    clock_t start, end;
    double cpu_time_used;

    start = clock();

    printf("Random_Number,Steps,Cache_Hit_Rate\n");
    for (int i = 0; i < testValue; i++) {
        int num = rd_num(minValue, maxValue);
        int steps;
        if (useCache) {
            int wasCacheHit;
            steps = collatz_with_cache(num, &wasCacheHit);
            printf("%d,%d,%.2f\n", num, steps, getCacheHitRate(cache));
        } else {
            steps = collatz(num);
            printf("%d,%d,N/A\n", num, steps);
        }
    }

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("\nTotal execution time: %.2f seconds\n", cpu_time_used);
    printf("Mean time per number: %.6f seconds\n", cpu_time_used / testValue);
    if (useCache) {
        printf("Final cache hit rate: %.2f%%\n", getCacheHitRate(cache));
    }
}

// void wait_for_space() {
//     printf("\nPress space to continue calculating with FIFO caching implemented...");
//     int c;
//     while ((c = getchar()) != ' ' && c != EOF) {
//         // Consume all characters until space or EOF is encountered
//     }
//     printf("\n");
// }

#include <stdio.h>
#include <stdlib.h>  // for atoi, srand
#include <string.h>  // for strcmp
#include <time.h>    // for time

int main(int argc, char *argv[]) {
    if (argc != 6) {
        printf("Usage: ./program_out <testing_value> <min> <max> <cache_mode> <cache_size>\n");
        return 1;
    }

    int testValue = atoi(argv[1]);
    int minValue = atoi(argv[2]);
    int maxValue = atoi(argv[3]);
    char *cacheMode = argv[4];
    int cacheSize = atoi(argv[5]);

    srand(time(NULL));

    if (strcmp(cacheMode, "none") == 0) {
        printf("Without Cache:\n");
        calculate_steps(testValue, minValue, maxValue, 0);
    }
    else if (strcmp(cacheMode, "FIFO") == 0) {
        printf("With FIFO Cache (size %d):\n", cacheSize);
        cache = createCache(cacheSize);
        calculate_steps(testValue, minValue, maxValue, 1);
    }

    // wait_for_space();

    // Free the cache memory
    if (cache != NULL) {
        for (int i = 0; i < MAX_CACHE_SIZE; i++) {
            if (cache->map[i]) {
                free(cache->map[i]);
            }
        }
        free(cache->map);
        free(cache);
    }

    return 0;
}