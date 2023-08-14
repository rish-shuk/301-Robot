#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "map_1.h"
#include "map_8.h"

void print_map_1() {
    // Access and print the array data
    for (int i = 0; i < MAP_ROWS; i++) {
        for (int j = 0; j < MAP_COLS; j++) {
            printf("%d", map_1[i][j]);
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
    Location parent;
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
bool is_walkable(int map[MAP_ROWS][MAP_COLS], int x, int y) {
    // 0 = Path
    // 1 = Obstacle
    return is_valid_location(x, y) && map[y][x] == 0;
}

// this function has been checked and should be fine
int heuristic(Location a, Location b) {
    return (abs(a.x - b.x) + abs(a.y - b.y)) * 10;
}

// this function is in the works of being fixed.
int reconstruct_path(Location came_from[MAP_ROWS][MAP_COLS], Location current, Location start, Location* path) {
    int path_length = 0;
    printf("========\n");
    for (int y = 0; y < MAP_ROWS; y++) {
        for (int x = 0; x < MAP_COLS; x++) {
            printf("[%d, %d]", came_from[y][x].x, came_from[y][x].y);
        }
        printf("\n");
    }
    printf("========\n");
    while (current.x != -1 && current.y != -1) {
        //printf("while - loop -");
        //printf("current length: %d\n", path_length);
        //path[path_length] = current;
        (path_length)++;
        printf("current location: %d, %d\n", current.x, current.y);
        current = came_from[current.y][current.x];
    }
    printf("WE ARE OUT OF WHILE LOOP IN RECONSTRUCT\n");
}

// this function is in the works of being fixed. -- some parts are fixed.
int astar(int map[MAP_ROWS][MAP_COLS], Location start, Location end, Location* path) {
    // OPEN - the set of nodes to be evalutated
    // CLOSED - the set of nodes already evaluated
    // -=-=- this should be fine v
    Node open_list[MAP_ROWS][MAP_COLS] = {0};
    bool closed_list[MAP_ROWS][MAP_COLS] = {false};
    Location came_from[MAP_ROWS][MAP_COLS] = {0};
    // -=-=- this should be fine ^

    // Initialise every node on the open list to be of highest F level
    for (int y = 0; y < MAP_ROWS; y++) {
        for (int x = 0; x < MAP_COLS; x++) {
            open_list[y][x] = (Node){.f = 999};
            printf("we are here? at %d\n", open_list[y][x].f);
        }
    }

    // Create moves list
    Moves moves_list[4] = {
        {.dx = 0, .dy = -1}, // up
        {.dx = 0, .dy = 1}, // down
        {.dx = -1, .dy = 0}, // left
        {.dx = 1, .dy = 0}, // right
    };

    // -=-=- this should be fine v
    // add the start node to the open list
    // f = g + h
    // g = distance from current node to the start node
    // h = distance from current node to the target node.
    Node start_node = {
        .parent = {.x = -1, .y = -1},
        .f = 0,
        .g = 0,
        .h = heuristic(start, end)
    };
    open_list[start.y][start.x] = start_node;
    came_from[start.y][start.x] = start_node.parent;
    //closed_list[start.y][start.x] = true;
    // -=-=- this should be fine ^

    // loop
    while (true) {
        Location current = {-1, -1};
        int min_f = 0;
        // current node = node in open list with lowest f value
        // Find the node with the lowest f value in the open list
        // -=-=- this should be fine v
        for (int y = 0; y < MAP_ROWS; y++) {
            for (int x = 0; x < MAP_COLS; x++) {
                //printf("before if statement of finding lowet f\n");
                if (open_list[y][x].f >= 0 && (!closed_list[y][x]) &&
                    (min_f == -1 || open_list[y][x].f < min_f)) {
                    min_f = open_list[y][x].f;
                    current.x = x;
                    current.y = y;
                    //printf("YEAH WE IN\n");
                    printf("FOUND LOWEST F AT (%d, %d) of %d\n", current.x, current.y, open_list[y][x].f);
                }
            }
        }
        // -=-=- this should be fine ^
        //printf("BEFORE PATH FOUND OR NO PATH POSSIBLE\n");

        // -=-=- this should be fine v
        // SPLIT INTO TWO FOR DEBUGGING PURPOSES
        if (current.x == -1) {
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
        // -=-=- this should be fine ^


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


        // -- THIS NEEDS TO BE REWORKED, MAKING SURE THAT THE came_from DOES NOT GET OVERWRITTEN 
        // -- ALSO X VALUE OF LOCATIONS IN came_FROM IS NOT CORRECT -- NEED TO FIND OUT WHY
        // -- CONSIDERING THAT IT BREAKS OUT OF THELOOP, IT MUST HAVE 

        // -- THIS IS CHECKING DIAGNOALS, NEED TO ONLY CHECK ADJACENT NODES -- THIS MIGHT BE THE ROOT OF THE PROBLEM
        for (int i = 0; i < 4; i++) {
            int dx = moves_list[i].dx;
            int dy = moves_list[i].dy;

            int neighbor_x = current.x + dx;
            int neighbor_y = current.y + dy;
            //printf("BEFORE WALKABLE\n");

            // if neighbour is not travesrable or neighbour is in closed list
            // skip to the next neighbour
            if (!is_walkable(map, neighbor_x, neighbor_y) || closed_list[neighbor_y][neighbor_x]) {
                printf("UNWALKABLE OR CLOSED LIST! NEIGHBOUR COORDS = %d, %d\n", neighbor_x, neighbor_y);
                continue;
            }

            printf("PASSED THROUGH! NEIGHBOUR COORDS = %d, %d\n", neighbor_x, neighbor_y);

            int tentative_g = open_list[current.y][current.x].g + 1;
            bool neighbor_is_better = false;
            // if new path to neighbour is shorter OR neighbour is not in OPEN
                // set f cost of neighbour
                // set parent of neighbour t ocurrent
                // if neighbour is not in OPEN
                    // add neighbour to OPEN
            //printf("==-=-==-==before if=-=-=-==-=\n");
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
                printf("current = %d, %d\n", current.x, current.y);
                printf("came_from[%d][%d] = %d, %d\n", neighbor_x, neighbor_y, came_from[neighbor_y][neighbor_x].x, came_from[neighbor_y][neighbor_x].y);
            }
            //printf("==-=-==-==after if=-=-=-==-=\n");

            if (neighbor_is_better) {
                // Add the neighbor to the open list
                open_list[neighbor_y][neighbor_x].parent = current;
            }
        }
    }

    return reconstruct_path(came_from, end, start, path);
}

// using map_1
int main() {
    Location start = {.x = 1, .y = 2};
    Location end = {.x = 3, .y = 3};
    Location path[1000] = {0};
    int path_length = 0;


    printf("Map\n");
    print_map_1();

    printf("BEFORE ASTAR ALGORTHM?\n");
    path_length = astar(map_1, start, end, path);
    printf("AFTER ASTAR ALGORTHM?\n");


    // print map_1 with path ---
    // copy map
    
    int map_with_path[MAP_ROWS][MAP_COLS] = {0};
    for (int i = 0; i < MAP_ROWS; i++) {
        for (int j = 0; j < MAP_COLS; j++) {
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