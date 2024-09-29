#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_CACHE_SIZE 1000000

int collatz_with_cache(int num);
int collatz(int num);

int cache[MAX_CACHE_SIZE] = {0};

int collatz_with_cache(int num) {
    if (num <= 1 || num > MAX_CACHE_SIZE) {
        return collatz(num);
    }

    if (cache[num] != 0) {
        return cache[num];
    }

    // Calculate steps
    int steps;
    if (num == 1) {
        steps = 0;
    } else if (num % 2 == 0) {
        steps = 1 + collatz_with_cache(num / 2);
    } else {
        steps = 1 + collatz_with_cache(3 * num + 1);
    }

    cache[num] = steps;

    return steps;
}


int collatz(int num) {
    int steps = 0;
    while (num != 1) {
        if (num % 2 == 0) { num = floor(num / 2); }
        else {num = 3 * num +1; }
        steps++;
    }
    return steps;
}

int rd_num(int min, int max) {
    return (rand() % (max - min + 1) + min);
}


void calculate_steps(int testValue, int minValue,  int maxValue) {
    for (int i = 0; i < testValue; i++) {
        int num = rd_num(minValue, maxValue);
        printf("Without cache - Value: %d - Steps: %d\n", num, collatz(num));
        printf("With cache - Value: %d - Steps: %d\n", num, collatz_with_cache(num));
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {  // argc should be 4: program name + 3 arguments
        printf("Usage: ./program_out <testing_value> <min> <max>\n");
        return 1;
    }

    calculate_steps( atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));

    return 0;
}