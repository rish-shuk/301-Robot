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
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define MAX_ROWS 15
#define MAX_COLS 19
#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof((arr)[0]))

// MAP INITIALISATION CODE
/*
void readMap(char *filePath, char map[MAX_ROWS][MAX_COLS]) {
    FILE *file = fopen(filePath, "r");

    if (file == NULL) {
        perror("Error opening file");
        exit(1); // Exit the program if file opening fails
    }

    // Read the file and populate the array
    for (int i = 0; i < MAX_ROWS; i++) {
        for (int j = 0; j < MAX_COLS; j++) {
            char c = fgetc(file);
            while (c != '0' && c != '1' && c != EOF) {
                c = fgetc(file);
            }
            if (c == EOF) {
                fclose(file);
                return; // Exit the function if end of file is reached
            }
            map[i][j] = c;
        }
    }

    fclose(file); // Close text file
}

void printMap(char map[MAX_ROWS][MAX_COLS]) {
    // Print the map in a grid format
    for (int i = 0; i < MAX_ROWS; i++) {
        for (int j = 0; j < MAX_COLS; j++) {
            printf("%c", map[i][j]);
        }
        printf("\n");
    }
}

struct Location {
    int row;
    int col;
};

struct Location getRandomLocation(char map[MAX_ROWS][MAX_COLS]) {
    struct Location randomLocation;
    int isNotValid = 1; // true
    while (isNotValid) {
        int random_row = rand() % MAX_ROWS;
        int random_col = rand() % MAX_COLS; // get random row and column
        // check point on map to see if it's a path
        if (map[random_row][random_col] == '0') {
            randomLocation.row = random_row;
            randomLocation.col = random_col;
            isNotValid = 0; // found valid location
        }
    }
    return randomLocation;
}

// pathfinding
struct Location moves[] = { {0, 1}, {0, -1}, {1, 0}, {-1, 0} };

bool isValidMove(int r, int c, int rows, int cols, char map[MAX_ROWS][MAX_COLS]) {
    return (r >= 0 && r < rows && c >= 0 && c < cols && map[r][c] == '0');
}

// dijkstra's algorithm- changes global map variable
void dijkstra(char map[MAX_ROWS][MAX_COLS], struct Location startlocation, struct Location targetlocation) {

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
    // retsteps can be generated from here
    //struct Location *retSteps = malloc(sizeof(struct Location) * shortestDist); // store every step of path- pointer to array
    int currentRow = targetlocation.row;
    int currentCol = targetlocation.col;
    //retSteps[shortestDist-1].row = currentRow;
    //retSteps[shortestDist-1].col = currentCol; // add targetLocation to retSteps
    //printf("retSteps[%d]: %d,%d\n", shortestDist-1, currentRow, currentCol);
    map[currentRow][currentCol] = 'X'; // mark finish location
    //printf("%d,%d\n", currentRow, currentCol); // print optimal step
    // reconstruct path by reversing paths array and taking shortest distance
    while(shortestDist > 0) {
        // make a move, check if valid and is the next step in path
        for (int i = 0; i < sizeof(moves) / sizeof(moves[0]); i++) {
            int newRow = currentRow + moves[i].row;
            int newCol = currentCol + moves[i].col;

            if (isValidMove(newRow, newCol, MAX_ROWS, MAX_COLS, map) && distances[newRow][newCol] == shortestDist - 1) {
                map[newRow][newCol] = '*'; // mark optimal step
                shortestDist--; // decrement distance
                currentRow = newRow;
                currentCol = newCol; // change current location
                //printf("retSteps[%d]: %d,%d\n", shortestDist,currentRow,currentCol); // print visited step
                //retSteps[shortestDist].row = currentRow;
                //retSteps[shortestDist].col = currentCol; // add currentRow and currentCol to retstep- stores optimal path
                if(currentRow == startlocation.row && currentCol == startlocation.col) {
                    map[currentRow][currentCol] = 'S'; // mark start location
                    break;
                }
            }
        }
    }
    printMap(map);
    //return retSteps;
}

void traverseMap(char map[MAX_ROWS][MAX_COLS]) {
    int currentRow, currentCol = 0;
    struct Location targetLocation;
    for(int i = 0; i < MAX_ROWS; i++) {
        for(int j = 0; j < MAX_COLS; j++) {
            if(map[i][j] == 'S') {
                currentRow = i;
                currentCol = j;
            } else if(map[i][j] == 'X') {
                targetLocation.row = i;
                targetLocation.col = j;
            }
    }

    bool leftOrient, rightOrient, upOrient, downOrient = false; // initialise orientation booleans- check which direction they're facing
    struct Location moves[] = { {0, 1}, {0, -1}, {1, 0}, {-1, 0} }; // initialise moves
    int visited[MAX_ROWS][MAX_COLS];
    for(int i = 0; i < MAX_ROWS; i++) {
        for(int j = 0; j < MAX_COLS; j++) {
            visited[i][j] = 0; // initialise all points as unvisited
        }
    }
        // traverse map, check for * or X until targetLocation reached
    while (currentRow != targetLocation.row && currentCol != targetLocation.col) {
        for(int i = 0; i < 4; i++) {
            int newRow = currentRow + moves[i].row;
            int newCol = currentCol + moves[i].col;

            // check next step in path and rotate and move accordingly- begin traversal
            if(map[newRow][newCol] == 'x' && visited[newRow][newCol] == 0) {
                visited[newRow][newCol] = 1; // mark new location as visited
                // move robot NEED TO CALIBRATE FOR DISTANCES AND DETERMINE ORIENTATION TO SEE IF A ROTATION IS NEEDED
                switch (i)
                {
                case 0:
                    // increment column (move right)
                    if(upOrient) {
                        // rotation clockwise
                    } else if(downOrient) {
                        // rotation anticlockwise
                    }
                    rightOrient = true;
                    // MOVE FORWARD
                    // STOP
                    break;
                case 1:
                    // decrement column (move left)
                    if(upOrient) {
                        // rotation anticlockwise
                    } else if(downOrient) {
                        // rotation clockwise
                    }
                    leftOrient = true;
                    // MOVE FORWARD
                    // STOP
                    break;
                case 2:
                    // increment row (move up)
                    if(rightOrient) {
                        // rotation anticlockwise
                    } else if(leftOrient) {
                        // rotation clockwise
                    }
                    upOrient = true;
                    // MOVE FORWARD
                    // stop
                    break;
                case 3:
                    // decrement row (move down)
                    if(rightOrient) {
                        // rotation clockwise
                    } else if(leftOrient) {
                        // rotation anticlockwise
                    }
                    downOrient = true;
                    // MOVE FORWARD
                    // STOP
                    break;
                default:
                    break;
                }
            }
        }
    }
    }
}

void findPath(char map[MAX_ROWS][MAX_COLS], char *filePath) {
    readMap(filePath, map); // read map
    dijkstra(map,getRandomLocation(map),getRandomLocation(map)); // find shortest path
}

*/
/* [] END OF FILE */