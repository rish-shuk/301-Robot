#ifndef MAP_FUNCTIONS_H
#define MAP_FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define MAX_ROWS 15
#define MAX_COLS 19
#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof((arr)[0]))
    
enum OrientationState {Up, Down, Left, Right};
enum InstructionDirection {GoForward, waitForLeftTurn, waitForRightTurn, uTurn, ForwardUntilTarget, StopAtTarget};

typedef struct Instruction {
    enum InstructionDirection direction;
    int ignoreL;
    int ignoreR;
    int distanceToTarget;
    enum OrientationState expectedOrientation;
} Instruction;



struct Location
{
    int row;
    int col;
};

// MAP INITIALISATION CODE
void printMap(int map[MAX_ROWS][MAX_COLS]);
struct Location getRandomLocation(int map[MAX_ROWS][MAX_COLS]);

// Pathfinding functions
bool isValidMove(int r, int c, int rows, int cols, int map[MAX_ROWS][MAX_COLS]);
void dijkstra(int map[MAX_ROWS][MAX_COLS], struct Location startlocation, struct Location targetlocation);
void clearMap(int map[MAX_ROWS][MAX_COLS]);
void getPathInstructions(int map[MAX_ROWS][MAX_COLS], int numSteps, struct Location startLocation, struct Location targetLocation);
void checkIgnoreTurn(enum OrientationState currentRobotOrientation, int currentRow, int currentCol);
uint8_t checkPathDirection(int currentRow, int currentCol);
uint8_t getTargetOrientation(int targetRow, int targetCol); 
Instruction * processInstructionList(int index);
uint8_t instructionsListLength(); 

Instruction * findPathNewOrientation(int map[MAX_ROWS][MAX_COLS], int food_list[6][2], int i, enum OrientationState orientation);
#endif /* MAP_FUNCTIONS_H */
