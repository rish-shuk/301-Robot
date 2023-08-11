#include <stdio.h>
#include <stdlib.h>

#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#define MAX_ROWS 15
#define MAX_COLS 19

struct Location {
    int row;
    int col;
};

struct Location getRandomLocation(int map[MAX_ROWS][MAX_COLS]);

#endif // FUNCTIONS_H