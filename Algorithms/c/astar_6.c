#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "map_1.h"
#include "map_8.h"

void print_map_1() {
    // Access and print the array data
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            printf("%d", map_1[j][i]);
        }
        printf("\n");
    }
}

void print_map_8() {
    // Access and print the array data
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            printf("%d", map_8[j][i]);
        }
        printf("\n");
    }
}

typedef struct {
    int x;
    int y;
} Location;

typedef struct {
    Location parent;
    double f, g, h;
} Node;

bool is_valid_location(int x, int y) {
    return x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT;
}

bool is_walkable(int map[MAP_HEIGHT][MAP_WIDTH], int x, int y) {
    return is_valid_location(x, y) && map[y][x] == 0;
}

double heuristic(Location a, Location b) {
    return abs(a.x - b.x) + abs(a.y - b.y);
}

int reconstruct_path(Location came_from[MAP_HEIGHT][MAP_WIDTH], Location current, Location start, Location* path) {
    int path_length = 0;
    printf("========\n");
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            printf("[%d, %d]", came_from[y][x].x, came_from[y][x].y);
        }
        printf("\n");
    }
    printf("========\n");
    while (current.x != -1 && current.y != -1) {
        printf("while - loop -");
        //printf("current length: %d\n", path_length);
        //path[path_length] = current;
        (path_length)++;
        //printf("current location: %d, %d\n", current.x, current.y);
        current = came_from[current.y][current.x];
    }
    printf("WE ARE OUT OF WHILE LOOP IN RECONSTRUCT\n");
}

int astar(int map[MAP_HEIGHT][MAP_WIDTH], Location start, Location end, Location* path) {
    // OPEN - the set of nodes to be evalutated
    // CLOSED - the set of nodes already evaluated
    Node open_list[MAP_HEIGHT][MAP_WIDTH] = {0};
    bool closed_list[MAP_HEIGHT][MAP_WIDTH] = {false};
    Location came_from[MAP_HEIGHT][MAP_WIDTH] = {0};

    // add the start node to the open list
    Node start_node = {
        .parent = {.x = -1, .y = -1},
        .f = 0,
        .g = 0,
        .h = heuristic(start, end)
    };
    open_list[start.y][start.x] = start_node;
    
    // loop
    while (true) {
        Location current = {-1, -1};
        double min_f = -1;
        // current node = node in open list with lowest f value
        // Find the node with the lowest f value in the open list
        for (int y = 0; y < MAP_HEIGHT; y++) {
            for (int x = 0; x < MAP_WIDTH; x++) {
                //printf("before if statement of finding lowet f\n");
                if (open_list[y][x].f >= 0 && (!closed_list[y][x]) &&
                    (min_f == -1 || open_list[y][x].f < min_f)) {
                    min_f = open_list[y][x].f;
                    current.x = x;
                    current.y = y;
                    //printf("YEAH WE IN\n");
                }
            }
        }
        //printf("BEFORE PATH FOUND OR NO PATH POSSIBLE\n");
        // if current is the target node // path has been found 
        if (current.x == -1 || current.x == end.x && current.y == end.y) {
            // Path found or no path possible
            break;
        }
        // "remove current node from open list"
        // add current to closed list
        closed_list[current.y][current.x] = true;

        // foreach neighbour of the current node
            // if new path to neighbour is shorter OR neighbour is not in OPEN
                // set f cost of neighbour
                // set parent of neighbour t ocurrent
                // if neighbour is not in OPEN
                    // add neighbour to OPEN
        // Generate successors
        //printf("BEFORE FOR LOOP OF NEIGHOBUR\n");
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                //printf("before if");
                if (dx == 0 && dy == 0) {
                    continue;
                }

                int neighbor_x = current.x + dx;
                int neighbor_y = current.y + dy;
                //printf("BEFORE WALKABLE\n");

                // if neighbour is not travesrable or neighbour is in closed list
                // skip to the next neighbour
                if (!is_walkable(map, neighbor_x, neighbor_y) || closed_list[neighbor_y][neighbor_x]) {
                    continue;
                }

                double tentative_g = open_list[current.y][current.x].g + 1;
                bool neighbor_is_better = false;
                // if new path to neighbour is shorter OR neighbour is not in OPEN
                    // set f cost of neighbour
                    // set parent of neighbour t ocurrent
                    // if neighbour is not in OPEN
                        // add neighbour to OPEN
                //printf("==-=-==-==hello=-=-=-==-=\n");
                if (!open_list[neighbor_y][neighbor_x].f || tentative_g < open_list[neighbor_y][neighbor_x].g) {
                    neighbor_is_better = true;
                    open_list[neighbor_y][neighbor_x].g = tentative_g;
                    open_list[neighbor_y][neighbor_x].h = heuristic(
                        (Location){.x = neighbor_x, .y = neighbor_y},
                        end
                    );
                    open_list[neighbor_y][neighbor_x].f = open_list[neighbor_y][neighbor_x].g +
                                                           open_list[neighbor_y][neighbor_x].h;
                    came_from[neighbor_y][neighbor_x] = current;
                    printf("came_from[%d][%d] = %d, %d\n", neighbor_y, neighbor_x, came_from[neighbor_y][neighbor_x].x, came_from[neighbor_y][neighbor_x].y);
                }

                if (neighbor_is_better) {
                    // Add the neighbor to the open list
                    open_list[neighbor_y][neighbor_x].parent = current;
                }
            }
        }
    }

    return reconstruct_path(came_from, end, start, path);
}

// using map_1
int main() {
    Location start = {.x = 1, .y = 1};
    Location end = {.x = 7, .y = 7};
    Location path[1000] = {0};
    int path_length = 0;


    printf("Map\n");
    print_map_1();

    printf("BEFORE ASTAR ALGORTHM?\n");
    path_length = astar(map_1, start, end, path);
    printf("AFTER ASTAR ALGORTHM?\n");


    // print map_1 with path ---
    // copy map
    
    int map_with_path[MAP_HEIGHT][MAP_WIDTH] = {0};
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            map_with_path[j][i] = map_1[j][i];
        }
    }
        printf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-\n");

    printf("Map with Path:\n");
    for (int i = path_length - 1; i >= 0; i--) {
        map_with_path[path[i].x][path[i].y] = 5;
    }
    printf("DO WE FINISH?\n");

    return 0;
}