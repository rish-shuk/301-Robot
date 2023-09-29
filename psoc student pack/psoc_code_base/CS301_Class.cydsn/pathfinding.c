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
#include "arraylist.h"

#define MAX_ROWS 15
#define MAX_COLS 19
#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof((arr)[0]))


// MAP INITIALISATION CODE

/*void printMap(char map[MAX_ROWS][MAX_COLS]) {
    // Print the map in a grid format
    for (int i = 0; i < MAX_ROWS; i++) {
        for (int j = 0; j < MAX_COLS; j++) {
            printf("%c", map[i][j]);
        }
        printf("\n");
    }
}

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
} */

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
}

void findPath() {
    struct Location startLocation = {1,1};
    struct Location targetLocation = {1,5};
    dijkstra(map, startLocation, targetLocation); // find shortest path
}

// ==--==--==--==--==--==--==--==--==--==--==--==--==
// ==--==--==--==--==--==--==--==--==--==--==--==--==
// ==--==--==--==-- A-STAR ALGORITHM --==--==--==--==
// ==--==--==--==--==--==--==--==--==--==--==--==--==
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


// checks if the x, y coordinates are within the map and does not go out of bounds
bool is_valid_Position(int x, int y) {
    return x >= 0 && x < MAX_COLS && y >= 0 && y < MAX_ROWS;
}

// checks if the current map coordinates are walkable (meaning at map[y][x] == 0)
bool is_walkable(int map[MAX_ROWS][MAX_COLS], int x, int y) {
    // 0 = Path
    // 1 = Obstacle
    return is_valid_Position(x, y) && map[y][x] == 0;
}

// manhatten distance between two points
int heuristic(Position a, Position b) {
    return (abs(a.x - b.x) + abs(a.y - b.y)) * 10;
}

// Reconstructs the path from the target node back to the start node and modifies the map array to show that path.
void reconstruct_path(int map[MAX_ROWS][MAX_COLS], Position came_from[MAX_ROWS][MAX_COLS], Position current, Position start) {
    // printf("========\n");
    // for (int y = 0; y < MAX_ROWS; y++) {
    //     for (int x = 0; x < MAX_COLS; x++) {
    //         printf("[%02d, %02d]", came_from[y][x].x, came_from[y][x].y);
    //     }
    //     printf("\n");
    // }
    // printf("========\n");

    int crashindicator = 0;
    // we should be at the end
    map[current.y][current.x] = 9; // 9 IS END OF APTH

    while (current.x != -1 && current.y != -1) {
        current = came_from[current.y][current.x];
        map[current.y][current.x] = 8; // 8 IS MARK OF PATH
        if (current.x == 0 && current.y == 0) {
            crashindicator++;
        }
        if (crashindicator > 3) {
            //printf("INFINTIE LOOP OCCURED: 0,0 EVERYWHERE - CRASH\n");
            break;
        }
        // we should be at the start
        if (current.x == start.x && current.y == start.y) {
            map[current.y][current.x] = 2; // 2 IS START OF PATH
        }
    }
    // for (int i = 0; i < MAX_ROWS; i++) {
    //     for (int j = 0; j < MAX_COLS; j++) {
    //         printf("%c", map[i][j]);
    //     }
    //     printf("\n");
    // } 
}

// Astar algorithm. Uses two parameters, G and H, to determine the best path to the target node.
void astar(int map[MAX_ROWS][MAX_COLS], Position start, Position end) {
    // OPEN - the set of nodes to be evalutated
    // CLOSED - the set of nodes already evaluated
    arraylist* open_list = arraylist_create();
    bool closed_list[MAX_ROWS][MAX_COLS] = {false};
    Position came_from[MAX_ROWS][MAX_COLS] = {0};
    Node *nodes[MAX_ROWS][MAX_COLS] = {0};

    for (int i = 0; i < MAX_ROWS; i++) {
        for (int j = 0; j < MAX_COLS; j++) {
            nodes[i][j] = (Node*)malloc(sizeof(Node));
            nodes[i][j]->f = 0;
            nodes[i][j]->g = 0;
            nodes[i][j]->h = 0;
            nodes[i][j]->parent.x = -1;
            nodes[i][j]->parent.y = -1;
            nodes[i][j]->position.x = -1;
            nodes[i][j]->position.y = -1;
        }
    }

    // Create moves list
    Moves moves_list[4] = {
        {.dx = 0, .dy = -1}, // up
        {.dx = 0, .dy = 1}, // down
        {.dx = -1, .dy = 0}, // left
        {.dx = 1, .dy = 0}, // right
    };

    // add the start node to the open list
    // f = g + h
    // g = distance from current node to the start node
    // h = distance from current node to the target node.
    Node start_node = {
        .parent = {.x = -1, .y = -1},
        .position = {.x = start.x, .y = start.y},
        .f = 0,
        .g = 0,
        .h = heuristic(start, end)
    };

    nodes[start.y][start.x] = &start_node;
    arraylist_add(open_list, nodes[start.y][start.x]);
    came_from[start.y][start.x] = start_node.parent;

    // loop
    while (true) {
        Position current = {-1, -1};
        Node* currentNode;
        int nodeIndex = 0;
        int min_f = -1;
        // current node = node in open list with lowest f value
        // Find the node with the lowest f value in the open list
        int openlistlength = arraylist_size(open_list);
        for (int i = 0; i < openlistlength; i++) {
            currentNode = (Node*)arraylist_get(open_list, i);
            if (currentNode->f >= 0 && (!closed_list[currentNode->position.y][currentNode->position.x]) &&
                (min_f == -1 || currentNode->f < min_f)) {
                min_f = currentNode->f;
                current.x = currentNode->position.x;
                current.y = currentNode->position.y;
                nodeIndex = i;
            }
        }

        if (current.x <= -1 || current.y <= -1) {
            //printf("NO PATH POSSIBLE!!!!!!\n");
            // No path possible
            break;
        }
        // if current is the target node // path has been found 
        if (current.x == end.x && current.y == end.y) {
            //printf("PATH FOUND!!!!!!!!!\n");
            // Path found
            break;
        }

        // remove current node from open list
        if (!(openlistlength == 0)) {
            arraylist_remove(open_list, nodeIndex);
        }
        openlistlength = arraylist_size(open_list);

        // add current to closed list
        closed_list[current.y][current.x] = true;

        // foreach neighbour of the current node
            // if new path to neighbour is shorter OR neighbour is not in OPEN
                // set f cost of neighbour
                // set parent of neighbour t ocurrent
                // if neighbour is not in OPEN
                    // add neighbour to OPEN
    
        // foreach neighbour of the current node
        for (int i = 0; i < 4; i++) {
            int dx = moves_list[i].dx;
            int dy = moves_list[i].dy;

            int neighbor_x = current.x + dx;
            int neighbor_y = current.y + dy;
            int tentative_g = currentNode->g + 1;

            Node neighborNode = {
                .parent = {.x = current.x, .y = current.y},
                .position = {.x = neighbor_x, .y = neighbor_y},
                .f = 0,
                .g = tentative_g,
                .h = 0
            };

            // if neighbour is not travesrable or neighbour is in closed list
            // skip to the next neighbour
            if (!is_walkable(map, neighbor_x, neighbor_y)) {
                //printf("UNWALKABLE! NEIGHBOUR COORDS = %d, %d\n", neighbor_x, neighbor_y);
                continue;
            }
            if (closed_list[neighbor_y][neighbor_x]) {
                //printf("CLOSED LIST! NEIGHBOUR COORDS = %d, %d\n", neighbor_x, neighbor_y);
                continue;
            }

            // if new path to neighbour is shorter OR 
            // neighbour is not in OPEN
            bool isNeighbourInOpen = false;

            if (openlistlength == 0) {
                isNeighbourInOpen = false;
            }
            // check if neighbour is in open list
            for (int i = 0; i < openlistlength; i++) {
                void* result = arraylist_get(open_list, i);
                Node* currentNode = (Node*)result;

                if (currentNode->position.x == neighbor_x && currentNode->position.y == neighbor_y) {
                    isNeighbourInOpen = true;
                }
            }

            // if neighbour is not in OPEN
            if (!isNeighbourInOpen){
                neighborNode.h = heuristic(neighborNode.position, end);
                // set f cost of neighbour
                neighborNode.f = neighborNode.g + neighborNode.h;
                // set parent of neighbour to current
                neighborNode.parent = current;
                came_from[neighbor_y][neighbor_x] = current;
                // if neighbour is not in OPEN
                // add neighbour to OPEN

                nodes[neighbor_y][neighbor_x]->f = neighborNode.f;
                nodes[neighbor_y][neighbor_x]->g = neighborNode.g;
                nodes[neighbor_y][neighbor_x]->h = neighborNode.h;
                nodes[neighbor_y][neighbor_x]->parent = neighborNode.parent;
                nodes[neighbor_y][neighbor_x]->position = neighborNode.position;

                arraylist_add(open_list, nodes[neighbor_y][neighbor_x]);
                openlistlength = arraylist_size(open_list);

                for (int i = 0; i < openlistlength; i++) {
                    Node* currentNode = (Node*)arraylist_get(open_list, i);
                }
            }
        }
    }
    reconstruct_path(map, came_from, end, start);
    //printMap(map);
}
// ==--==--==--==--==--==--==--==--==--==--==--==--==
// ==--==--==--==--==--==--==--==--==--==--==--==--==
// ==--==--==--==-- A-STAR ALGORITHM --==--==--==--==
// ==--==--==--==--==--==--==--==--==--==--==--==--==

/* [] END OF FILE */