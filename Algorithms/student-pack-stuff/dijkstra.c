#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define MAX_ROWS 15
#define MAX_COLS 19

struct Location {
    int row;
    int col;
};

struct PriorityQueue {
    struct Location location;
    int distance;
};

struct Location moves[] = {{0,1}, {0,-1},{1,0},{-1,0}};

bool isValidMove(int r, int c, int rows, int cols, int map[MAX_ROWS][MAX_COLS]) {
    return (r >= 0 && r < rows && c >= 0 && c < cols && map[r][c] == 0);
};

struct Location get_random_location(int rows, int cols, int map[MAX_ROWS][MAX_COLS]) {
    int isNotValid = 1; // true
    while(isNotValid) {
        int random_row = rand() % MAX_ROWS; // get random row and column
        int random_col = rand() % MAX_COLS; 
        // check point on map to see if it's a path
        if(map[random_row][random_col] == 0) {
            isNotValid = 0; 
        }
    }

    
}

void dijkstra(int map[MAX_ROWS][MAX_COLS], int rows, int cols, struct Location startLocation, struct Location targetLocation) {

}