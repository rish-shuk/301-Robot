#include <stdio.h>
#include <stdlib.h>
#include "map_1.h"
#include "map_8.h"

void print_map_1() {
    // Access and print the array data
    for (int i = 0; i < NUM_ROWS; i++) {
        for (int j = 0; j < NUM_COLS; j++) {
            printf("%d", map_1[i][j]);
        }
        printf("\n");
    }
}

void print_map_8() {
    // Access and print the array data
    for (int i = 0; i < NUM_ROWS; i++) {
        for (int j = 0; j < NUM_COLS; j++) {
            printf("%d", map_8[i][j]);
        }
        printf("\n");
    }
}

int main() {
    print_map_1();
    return 0;
}