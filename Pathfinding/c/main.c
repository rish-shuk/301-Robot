#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "map2.h"

#define MAX_ROWS 15
#define MAX_COLS 19
#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof((arr)[0]))
int numSteps = 0;

void printMap(int map[MAX_ROWS][MAX_COLS]) {
    // Print the map in a grid format
    for (int i = 0; i < MAX_ROWS; i++) {
        for (int j = 0; j < MAX_COLS; j++) {
            printf("%d", map[i][j]);
        }
        printf("\n");
    }
}
void clearMap(int map[MAX_ROWS][MAX_COLS]) {
    // Print the map in a grid format
    for (int i = 0; i < MAX_ROWS; i++) {
        for (int j = 0; j < MAX_COLS; j++) {
            if(map[i][j] != 1) { map[i][j] = 0;} // clear map
        }
    }
}
struct Location {
    int row;
    int col;
};

int list_of_optimal_coordinates[285][2]; // ROW, COL

struct Location getRandomLocation(int map[MAX_ROWS][MAX_COLS]) {
    struct Location randomLocation;
    int isNotValid = 1; // true
    while (isNotValid) {
        int random_row = rand() % MAX_ROWS;
        int random_col = rand() % MAX_COLS; // get random row and column
        // check point on map to see if it's a path
        if (map[random_row][random_col] == 0) {
            randomLocation.row = random_row;
            randomLocation.col = random_col;
            isNotValid = 0; // found valid location
        }
    }
    return randomLocation;
}

struct Location moves[] = { {0, 1}, {0, -1}, {1, 0}, {-1, 0} };

bool isValidMove(int r, int c, int rows, int cols, int map[MAX_ROWS][MAX_COLS]) {
    return (r >= 0 && r < rows && c >= 0 && c < cols && map[r][c] == 0);
}

void dijkstra(int map[MAX_ROWS][MAX_COLS], struct Location startlocation, struct Location targetlocation) {

    int visitedMap[MAX_ROWS][MAX_COLS];
    for (int i = 0; i < MAX_ROWS; i++) {
        for (int j = 0; j < MAX_COLS; j++) {
            visitedMap[i][j] = 1; // fill visited array with 1s- mark as unvisited
        }
    }

    struct PriorityQueue {
        struct Location location;
        int distance;
    };

    struct PriorityQueue pq[MAX_ROWS * MAX_COLS]; // initialise priority queue
    int pqSize = 0;

    // Initialize distances and paths arrays
    int distances[MAX_ROWS][MAX_COLS]; // array to record distance of each location
    //struct Location paths[MAX_ROWS * MAX_COLS]; // 1d array to record path- each element is a location

    for (int i = 0; i < MAX_ROWS; i++) {
        for (int j = 0; j < MAX_COLS; j++) {
            distances[i][j] = 1000; // initialise distances as infinity
        }
    }

    int stepNum = 0;
    distances[startlocation.row][startlocation.col] = 0; // initialise distance of startLoc to 0
    //paths[stepNum] = startlocation; // add start to path array
    stepNum++; // increment stepNum

    pq[pqSize].location = startlocation; // add startLocation to Priority queue
    pq[pqSize].distance = 0; // add startlocation distance to priority queue
    pqSize++; // increment pqSize- pq is not empty

    while (pqSize > 0) {
        int idx = 0;
        for (int i = 1; i < pqSize; i++) {
            if (pq[i].distance < pq[idx].distance) {
                idx = i; // find the shortest distance location 
            }
        }
        struct PriorityQueue current = pq[idx]; // record current location
        pq[idx] = pq[pqSize - 1];
        pqSize--; // decrement pqSize

        int row = current.location.row;
        int col = current.location.col; // get current location details

        if (visitedMap[row][col] == 1) {
            visitedMap[row][col] = 0; // mark as visited if unvisited

            if (row == targetlocation.row && col == targetlocation.col) {
                break; // if reached target destination, end
            }
            // make a move, check if valid
            for (int i = 0; i < sizeof(moves) / sizeof(moves[0]); i++) {
                int newRow = row + moves[i].row;
                int newCol = col + moves[i].col;

                if (isValidMove(newRow, newCol, MAX_ROWS, MAX_COLS, map) && visitedMap[newRow][newCol] == 1) {
                    int newDistance = distances[row][col] + 1;
                    // take the shortest distance between new and existing
                    if (newDistance < distances[newRow][newCol]) {
                        distances[newRow][newCol] = newDistance; // take new distance if less than existing, otherwise do nothing
                        //paths[stepNum] = (struct Location){newRow, newCol}; // add location to path array
                        
                        pq[pqSize].location.row = newRow; // update location
                        pq[pqSize].location.col = newCol; // update location
                        pq[pqSize].distance = newDistance; // update distance
                        
                        stepNum++;
                        pqSize++;
                    }
                }
            }
        }
    }

    // PATH RECONSTRUCTION
    // initialise maximum distance- will be decremented
    int shortestDist = distances[targetlocation.row][targetlocation.col];
    numSteps = shortestDist; // save number of steps
    // retsteps can be generated from here
    struct Location *retSteps = malloc(sizeof(struct Location) * shortestDist); // store every step of path- pointer to array
    int currentRow = targetlocation.row;
    int currentCol = targetlocation.col;
    retSteps[shortestDist-1].row = currentRow; 
    retSteps[shortestDist-1].col = currentCol; // add targetLocation to retSteps
    //printf("retSteps[%d]: %d,%d\n", shortestDist-1, currentRow, currentCol);
    map[currentRow][currentCol] = 9; // mark finish location
    //printf("%d,%d\n", currentRow, currentCol); // print optimal step
    // reconstruct path by reversing paths array and taking shortest distance
    int pathIndex = 0;

    while(shortestDist > 0) {    
        // make a move, check if valid and is the next step in path
        for (int i = 0; i < sizeof(moves) / sizeof(moves[0]); i++) {
            int newRow = currentRow + moves[i].row;
            int newCol = currentCol + moves[i].col;

            if (isValidMove(newRow, newCol, MAX_ROWS, MAX_COLS, map) && distances[newRow][newCol] == shortestDist - 1) {
                map[newRow][newCol] = 8; // mark optimal step
                list_of_optimal_coordinates[pathIndex][0] = newRow;
                list_of_optimal_coordinates[pathIndex][1] = newCol;
                pathIndex++;
                // reverse array somewhere

                shortestDist--; // decrement distance
                currentRow = newRow; 
                currentCol = newCol; // change current location
                //printf("retSteps[%d]: %d,%d\n", shortestDist,currentRow,currentCol); // print visited step
                retSteps[shortestDist].row = currentRow; 
                retSteps[shortestDist].col = currentCol; // add currentRow and currentCol to retstep- stores optimal path
                if(currentRow == startlocation.row && currentCol == startlocation.col) {
                    map[currentRow][currentCol] = 2; // mark start location
                    break;
                }   
            }
        }
    }
    //printMap(map);
    //return retSteps;
}

enum InstructionDirection {Forward, TurnLeft, TurnRight, uTurn, ForwardUntilTarget, StopAtTarget};
enum OrientationState {Up, Down, Left, Right};
enum OrientationState previousRobotOrientation, currentRobotOrientation = Down; // initialize 
enum InstructionDirection Instructions[285];
struct Instructions {
    enum InstructionDirection direction;
    int ignoreL;
    int ignoreR;
};

void checkIgnoreTurn(enum OrientationState currentRobotOrientation, int currentRow, int currentCol);
int checkPathDirection(int currentRow, int currentCol);
int getTargetOrientation(int targetRow, int targetCol); 
int ignoreR = 0, ignoreL = 0;
struct Instructions instructionsList[285]; // list to store instructions
// return a list of instructions for robot to execute
void getPathInstructions(int map[MAX_ROWS][MAX_COLS], int numSteps, struct Location startLocation, struct Location targetLocation);
void getPathInstructions(int map[MAX_ROWS][MAX_COLS], int numSteps, struct Location startLocation, struct Location targetLocation) {
    int currentRow = startLocation.row; 
    int currentCol = startLocation.col; // initialise with start
    int listIndex = 0;
    
    // given path, traverse it by calculating number of turns/ turns to skip between each junction
    while(numSteps >= 0) {
        checkIgnoreTurn(currentRobotOrientation, currentRow, currentCol); // if a 0 is adjacent to path, need to update ignoreL/ ignoreR counts- check zeroes
        map[currentRow][currentCol] = 0; // clear visited map location (for when we need to u-turn)
        previousRobotOrientation = currentRobotOrientation;
        enum InstructionDirection newDirection;
        int nextStep = checkPathDirection(currentRow,currentCol);  // check all four sides for next step in path
        int targetOrientation = getTargetOrientation(targetLocation.row, targetLocation.col);
        if(currentRow == targetLocation.row && currentCol == targetLocation.col) {
            newDirection = ForwardUntilTarget; // if next step is the target, call forward until target
            printf("Reached target\n");
            nextStep = 5;
        }
        switch (nextStep) {
            case 0: // next step is up
                currentRobotOrientation = Up;
                if(previousRobotOrientation == Left) {
                    newDirection = TurnRight;
                    printf("Right Turn\n") ; // need right turn
                    ignoreL = 0;
                    ignoreR = 0; // reset ignoreL/ ignoreR
                } 
                if(previousRobotOrientation == Right) {
                    newDirection = TurnLeft;
                    printf("Left Turn\n") ; // need a right turn
                    ignoreL = 0;
                    ignoreR = 0; // reset ignoreL/ ignoreR
                }
                if(previousRobotOrientation == Up) {
                    // check if we only need to go forward to reach target
                    if(targetOrientation == nextStep && targetLocation.col == currentCol) {
                        newDirection = ForwardUntilTarget;
                        printf("Forward until target\n");
                    } else {
                        newDirection = Forward;
                        printf("Forward; ignore %dL, ignore %dR\n",  ignoreL, ignoreR);
                    }
                }
                if(previousRobotOrientation == Down) {
                    newDirection = uTurn;
                    printf("U-turn\n");
                }
                currentRow--; // jump to new location
                break;
            case 1: // next step is down
                currentRobotOrientation = Down;
                if(previousRobotOrientation == Right) {
                    newDirection = TurnRight;
                    printf("Right Turn\n") ; // need Right turn
                    ignoreL = 0;
                    ignoreR = 0; // reset ignoreL/ ignoreR
                } 
                if(previousRobotOrientation == Left) {
                    newDirection = TurnLeft;
                    printf("Left Turn\n") ; // need Left turn
                    ignoreL = 0;
                    ignoreR = 0; // reset ignoreL/ ignoreR
                }
                if(previousRobotOrientation == Down) {
                    // check if we only need to go forward to reach target
                    if(targetOrientation == nextStep && targetLocation.col == currentCol) {
                        newDirection = ForwardUntilTarget;
                        printf("Forward until target\n");
                    } else {
                        newDirection = Forward;
                        printf("Forward; ignore %dL, ignore %dR\n",  ignoreL, ignoreR);
                    }                    
                }
                if(previousRobotOrientation == Up) {
                    newDirection = uTurn;
                    ignoreL = 0;
                    ignoreR = 0;
                    printf("U-turn\n");
                }
                currentRow++;
                break;
            case 2: // next step is left
                currentRobotOrientation = Left;
                if(previousRobotOrientation == Up) {
                    newDirection = TurnLeft;
                    printf("Left Turn\n") ; // need left turn
                    ignoreL = 0;
                    ignoreR = 0; // reset ignoreL/ ignoreR
                } 
                if(previousRobotOrientation == Down) {
                    newDirection = TurnRight;
                    printf("Right Turn\n") ; // need a right turn
                    ignoreL = 0;
                    ignoreR = 0; // reset ignoreL/ ignoreR
                }
                if(previousRobotOrientation == Left) {
                    // check if we only need to go forward to reach target
                    if(targetOrientation == nextStep && targetLocation.row == currentRow) {
                        newDirection = ForwardUntilTarget;
                        printf("Forward until target\n");
                    } else {
                        newDirection = Forward;
                        printf("Forward; ignore %dL, ignore %dR\n",  ignoreL, ignoreR);
                    }
                }
                if(previousRobotOrientation == Right) {
                    newDirection = uTurn;
                    ignoreL = 0;
                    ignoreR = 0;
                    printf("U-turn\n");
                }
                currentCol--;
                break;
            case 3: // next step is right
                currentRobotOrientation = Right;
                if(previousRobotOrientation == Up) {
                    newDirection = TurnRight;
                    printf("Right Turn\n") ; // need a right turn
                    ignoreL = 0;
                    ignoreR = 0; // reset ignoreL/ ignoreR
                }
                if(previousRobotOrientation == Down) {
                    newDirection = TurnLeft;
                    printf("Left Turn\n") ; // need left turn
                    ignoreL = 0;
                    ignoreR = 0; // reset ignoreL/ ignoreR
                }
                if(previousRobotOrientation == Right) {
                    // check if we only need to go forward to reach target
                    if(targetOrientation == nextStep && targetLocation.row == currentRow) {
                        newDirection = ForwardUntilTarget;
                        printf("Forward until target\n");
                    } else {
                        newDirection = Forward;
                        printf("Forward; ignore %dL, ignore %dR\n",  ignoreL, ignoreR);
                    }
                }
                if(previousRobotOrientation == Left) {
                    newDirection = uTurn;
                    ignoreL = 0;
                    ignoreR = 0;
                    printf("U-turn\n");
                }
                currentCol++;
                break;
            default:
                break;
        }
        
        // add new direction to list
        instructionsList[listIndex].direction = newDirection;
        instructionsList[listIndex].ignoreL = ignoreL;
        instructionsList[listIndex].ignoreR = ignoreR;
        listIndex++; // increment instruction list index
        numSteps--; // decrement numSteps
    }
}

int checkPathDirection(int currentRow, int currentCol) {
    // map is cleared beforehand, so no need to check- this code will allow for 180deg turns
    if (map[currentRow - 1][currentCol] == 8 || map[currentRow - 1][currentCol] == 9) {
        return 0; // up 
    }
    if (map[currentRow + 1][currentCol] == 8 || map[currentRow + 1][currentCol] == 9) {
        return 1; // down
    }
    if (map[currentRow][currentCol - 1] == 8 || map[currentRow][currentCol - 1] == 9) {
        return 2; // left
    }
    if (map[currentRow][currentCol + 1] == 8 || map[currentRow][currentCol + 1] == 9) {
        return 3; // right
    }
    return 5; // default- will never reach this
}

void checkIgnoreTurn(enum OrientationState robotOrientation, int currentRow, int currentCol) {
    // UP - 0 DOWN - 1 LEFT - 2 RIGHT - 3
    if (map[currentRow - 1][currentCol] == 0) { // if the row above is a 0
        switch (robotOrientation) {
            case Left:
                ignoreR++;
                break;
            case Right:
                ignoreL++;
                break;
            default:
                break; // ignore when going up and down
        }
    }
    if (map[currentRow + 1][currentCol] == 0) { // if row below is a 0
        switch (robotOrientation) {
            case Left:
                ignoreL++;
                break;
            case Right:
                ignoreR++;
                break;
            default:
                break;
        }
    }
    if (map[currentRow][currentCol - 1] == 0) { 
        switch (robotOrientation) {
            case Up:
                ignoreL++;
                break;
            case Down:
                ignoreR++;
                break;
            default:
                break;
        }
    }
    if (map[currentRow][currentCol + 1] == 0) {
        switch (robotOrientation) {
            case Up:
                ignoreR++;
                break;
            case Down:
                ignoreL++;
                break;
            default:
                break;
        }
    }
}

int getTargetOrientation(int targetRow, int targetCol) {
    if(map[targetRow + 1][targetCol] == 8) {
        return 0; // robot will be travelling up
    }
    if(map[targetRow - 1][targetCol] == 9) {
        return 1; // robot will be travelling down
    }
    if(map[targetRow][targetCol - 1] == 9) {
        return 2; // robot will be travelling left
    }
    if(map[targetRow][targetCol + 1] == 9) {
        return 3; // robot will be travelling right
    }
    return 5; // placeholder
}

int main() {
    struct Location startLocation; 
    startLocation.row = 1;
    startLocation.col = 1;
    struct Location targetLocation; // generate random start and target location
    targetLocation.row = 3;
    targetLocation.col = 3;
    printf("\n");
    printf("Start location: %d , %d\n", startLocation.row, startLocation.col);
    printf("Target location: %d , %d\n", targetLocation.row, targetLocation.col); // print start and target location
    dijkstra(map, startLocation, targetLocation); // find shortest path
    // get list of coordinates of path
    printMap(map);
    getPathInstructions(map, numSteps, startLocation, targetLocation); // edit list of instructions
    clearMap(map);
    printf("\n");
    startLocation.row = 3;
    startLocation.col = 3;
    targetLocation.row = 1;
    targetLocation.col = 1;
    dijkstra(map, startLocation, targetLocation);
    printMap(map);
    getPathInstructions(map, numSteps, startLocation, targetLocation);

    // can add some logic to skip multiple forward calls- only check the last one

    return 0;
}
