#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <functions.h>

#define MAX_ROWS 19
#define MAX_COLS 15

struct Location {
    int row;
    int col;
};

struct Location getRandomLocation(int map[MAX_ROWS][MAX_COLS]) {
    struct Location randomLocation;
    int isNotValid = 1; // true
    while(isNotValid) {
        int random_row = rand() % MAX_ROWS; // get random row and column
        int random_col = rand() % MAX_COLS; 
        // check point on map to see if it's a path
        if(map[random_row][random_col] == 0) {
            randomLocation.row = random_row;
            randomLocation.col = random_col;
            isNotValid = 0; 
        }
    }
    return randomLocation;    
}