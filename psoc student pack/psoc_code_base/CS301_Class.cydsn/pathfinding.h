/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
 */

#ifndef MAP_FUNCTIONS_H
#define MAP_FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define MAX_ROWS 15
#define MAX_COLS 19
#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof((arr)[0]))

struct Location
{
    int row;
    int col;
};

// MAP INITIALISATION CODE
//void readMap(char *filePath, char map[MAX_ROWS][MAX_COLS]);
//void printMap(char map[MAX_ROWS][MAX_COLS]);
//struct Location getRandomLocation(char map[MAX_ROWS][MAX_COLS]);

// Pathfinding functions
bool isValidMove(int r, int c, int rows, int cols, int map[MAX_ROWS][MAX_COLS]);
void dijkstra(int map[MAX_ROWS][MAX_COLS], struct Location startlocation, struct Location targetlocation);
void findPath();

// ASTAR
typedef struct {
    int x;
    int y;
} Position;

typedef struct {
    Position parent, position;
    int f, g, h;
} Node;

typedef struct {
    int dx;
    int dy;
} Moves;

bool is_valid_Position(int x, int y);
bool is_walkable(int map[MAX_ROWS][MAX_COLS], int x, int y);
int heuristic(Position a, Position b);
void reconstruct_path(int map[MAX_ROWS][MAX_COLS], Position came_from[MAX_ROWS][MAX_COLS], Position current, Position start);
void astar(int map[MAX_ROWS][MAX_COLS], Position start, Position end);

#endif /* MAP_FUNCTIONS_H */
