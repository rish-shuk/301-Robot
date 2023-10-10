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

enum InstructionDirection {GoForward, waitForLeftTurn, waitForRightTurn, uTurn, ForwardUntilTarget, StopAtTarget, Skip};
typedef struct Instruction {
    enum InstructionDirection direction;
    int ignoreL;
    int ignoreR;
} Instruction;

enum OrientationState {Up, Down, Left, Right};

struct Location
{
    int row;
    int col;
};

// MAP INITIALISATION CODE
void printMap(int map[MAX_ROWS][MAX_COLS]);
struct Location getRandomLocation(int map[MAX_ROWS][MAX_COLS]);

// Pathfinding functions
bool isValidMove(int r, int c, int rows, int cols, char map[MAX_ROWS][MAX_COLS]);
void dijkstra(int map[MAX_ROWS][MAX_COLS], struct Location startlocation, struct Location targetlocation);
void clearMap(int map[MAX_ROWS][MAX_COLS]);
Instruction* findPath(int map[MAX_ROWS][MAX_COLS], int food_list[5][2]);
void checkIgnoreTurn(enum OrientationState currentRobotOrientation, int currentRow, int currentCol);
int checkPathDirection(int currentRow, int currentCol);
int getTargetOrientation(int targetRow, int targetCol); 
void processInstructionList(int index);
int instructionsListLength(); 

#endif /* MAP_FUNCTIONS_H */
