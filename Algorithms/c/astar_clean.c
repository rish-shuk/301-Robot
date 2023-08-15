#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "map_1.h"
#include "map_8.h"
#include "arraylist.c"

void print_map_1() {
    // Access and print the array data
    for (int i = 0; i < MAP_ROWS; i++) {
        for (int j = 0; j < MAP_COLS; j++) {
            printf("%c", map_1[i][j]);
        }
        printf("\n");
    }
}

void print_map_8() {
    // Access and print the array data
    for (int i = 0; i < MAP_ROWS; i++) {
        for (int j = 0; j < MAP_COLS; j++) {
            printf("%d", map_8[i][j]);
        }
        printf("\n");
    }
}

typedef struct {
    int x;
    int y;
} Location;

typedef struct {
    Location parent, position;
    int f, g, h;
} Node;

typedef struct {
    int dx;
    int dy;
} Moves;

// this function has been checked and should be fine
// checks if the x, y coordinates are within the map and does not go out of bounds
bool is_valid_location(int x, int y) {
    return x >= 0 && x < MAP_COLS && y >= 0 && y < MAP_ROWS;
}

// this function has been checked and should be fine
// checks if the current map coordinates are walkable (meaning at map[y][x] == 0)
bool is_walkable(char map[MAP_ROWS][MAP_COLS], int x, int y) {
    // 0 = Path
    // 1 = Obstacle
    return is_valid_location(x, y) && map[y][x] == '0';
}

// manhatten distance
int heuristic(Location a, Location b) {
    return (abs(a.x - b.x) + abs(a.y - b.y)) * 10;
}

// this function is in the works of being fixed.
// OBJECTIVE: USING THE came_from ARRAY, RECONSTRUCT THE PATH FROM THE END TO THE START
int reconstruct_path(Location came_from[MAP_ROWS][MAP_COLS], Location current, Location start) {
    printf("========\n");
    for (int y = 0; y < MAP_ROWS; y++) {
        for (int x = 0; x < MAP_COLS; x++) {
            printf("[%02d, %02d]", came_from[y][x].x, came_from[y][x].y);
        }
        printf("\n");
    }
    printf("========\n");

    char map_with_path[MAP_ROWS][MAP_COLS] = {0};
    for (int i = 0; i < MAP_ROWS; i++) {
        for (int j = 0; j < MAP_COLS; j++) {
            map_with_path[i][j] = map_1[i][j];
        }
    }

    int crashindicator = 0;
    // we should be at the end
    map_with_path[current.y][current.x] = 'X';

    while (current.x != -1 && current.y != -1) {
        current = came_from[current.y][current.x];
        map_with_path[current.y][current.x] = '*';
        if (current.x == 0 && current.y == 0) {
            crashindicator++;
        }
        if (crashindicator > 3) {
            printf("INFINTIE LOOP OCCURED: 0,0 EVERYWHERE - CRASH\n");
            break;
        }
        // we should be at the start
        if (current.x == start.x && current.y == start.y) {
            map_with_path[current.y][current.x] = 'S';
        }
    }

    for (int i = 0; i < MAP_ROWS; i++) {
        for (int j = 0; j < MAP_COLS; j++) {
            printf("%c", map_with_path[i][j]);
        }
        printf("\n");
    }
}

// astar algorithm
void astar(char map[MAP_ROWS][MAP_COLS], Location start, Location end) {
    // OPEN - the set of nodes to be evalutated
    // CLOSED - the set of nodes already evaluated
    arraylist* open_list = arraylist_create();
    bool closed_list[MAP_ROWS][MAP_COLS] = {false};
    Location came_from[MAP_ROWS][MAP_COLS] = {0};
    Node *nodes[MAP_ROWS][MAP_COLS] = {0};

    for (int i = 0; i < MAP_ROWS; i++) {
        for (int j = 0; j < MAP_COLS; j++) {
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
        Location current = {-1, -1};
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
            printf("NO PATH POSSIBLE!!!!!!\n");
            // No path possible
            break;
        }
        // if current is the target node // path has been found 
        if (current.x == end.x && current.y == end.y) {
            printf("PATH FOUND!!!!!!!!!\n");
            // Path found
            break;
        }

        // remove current node from open list
        if (!openlistlength == 0) {
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
    reconstruct_path(came_from, end, start);
}

// using map_1
int main() {
    Location start = {.x = 1, .y = 2};
    Location end = {.x = 3, .y = 1};

    printf("Map\n");
    print_map_1();

    printf("BEFORE ASTAR ALGORTHM?\n");
    astar(map_1, start, end);
    printf("AFTER ASTAR ALGORTHM?\n");

    return 0;
}