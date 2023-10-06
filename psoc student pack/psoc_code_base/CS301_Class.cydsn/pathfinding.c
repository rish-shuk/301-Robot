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
#include "map.h"

#define MAX_ROWS 15
#define MAX_COLS 19
#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof((arr)[0]))

/*int map[15][19] = {
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
{1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
{1,0,1,0,1,1,1,1,1,0,1,1,1,0,1,1,1,0,1},
{1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,1},
{1,1,1,0,1,0,1,0,1,1,1,0,1,0,1,1,1,0,1},
{1,0,0,0,1,0,1,0,0,0,1,0,1,0,0,0,0,0,1},
{1,0,1,1,1,0,1,0,1,0,1,0,1,0,1,1,1,0,1},
{1,0,1,0,1,0,0,0,1,0,1,0,1,0,1,0,0,0,1},
{1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,0,1,1,1},
{1,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,0,0,1},
{1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
{1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1},
{1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1},
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
};*/

int map[15][19] = {
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
{1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1},
{1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,0,1,0,1},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1},
{1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,0,1,0,1},
{1,0,1,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,1},
{1,0,1,1,1,0,1,0,1,0,1,0,1,0,1,1,1,0,1},
{1,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,0,0,1},
{1,0,1,1,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1},
{1,0,0,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1},
{1,1,1,1,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1},
{1,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,1},
{1,0,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1},
{1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1},
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
};



int start_pos[2] = {1,1};

int food_list[5][2]= {
{1,9},
{5,5},
{7,1},
{13,5},
{9,9}
};


// MAP INITIALISATION CODE

/*void printMap(char map[MAX_ROWS][MAX_COLS]) {
    // Print the map in a grid format
    for (int i = 0; i < MAX_ROWS; i++) {
        for (int j = 0; j < MAX_COLS; j++) {
            printf("%c", map[i][j]);
        }
        printf("\n");
    }
}*/

struct Location {
    int row;
    int col;
};

// pathfinding 
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
    while(shortestDist > 0) {    
        // make a move, check if valid and is the next step in path
        for (int i = 0; i < sizeof(moves) / sizeof(moves[0]); i++) {
            int newRow = currentRow + moves[i].row;
            int newCol = currentCol + moves[i].col;

            if (isValidMove(newRow, newCol, MAX_ROWS, MAX_COLS, map) && distances[newRow][newCol] == shortestDist - 1) {
                map[newRow][newCol] = 8; // mark optimal step
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
    // printMap(map);
}

void findPath(int map[MAX_ROWS][MAX_COLS]) {
    struct Location startLocation = {1,1};
    struct Location targetLocation = {5,5};
    dijkstra(map, startLocation, targetLocation); // find shortest path
    //map[0][0] = 7;
}

/* [] END OF FILE */