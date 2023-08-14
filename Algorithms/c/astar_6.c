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
    int crashindicator = 0;
    while (current.x != -1 && current.y != -1) {
        //printf("while - loop -");
        //printf("current length: %d\n", path_length);
        //path[path_length] = current;
        (path_length)++;
        printf("current location: %d, %d\n", current.x, current.y);
        current = came_from[current.y][current.x];
        if (current.x == 0 && current.y == 0) {
            crashindicator++;
        }
        if (crashindicator > 3) {
            printf("INFINTIE LOOP OCCURED: 0,0 EVERYWHERE - CRASH\n");
            break;
        }
    }
    printf("WE ARE OUT OF WHILE LOOP IN RECONSTRUCT\n");
}

// this function is in the works of being fixed. -- some parts are fixed.
int astar(int map[MAP_ROWS][MAP_COLS], Location start, Location end, Location* path, bool *path_taken[MAP_ROWS][MAP_COLS]) {
    // OPEN - the set of nodes to be evalutated
    // CLOSED - the set of nodes already evaluated
    // -=-=- this should be fine v
    
    // 15/08/23 -- CHANGING OPENLIST AND CLOSED LIST TO BE AN ARRAYLIST
    //Node open_list[MAP_ROWS][MAP_COLS] = {0};
    arraylist* open_list = arraylist_create();

    bool closed_list[MAP_ROWS][MAP_COLS] = {false};
    Location came_from[MAP_ROWS][MAP_COLS] = {0};
    // -=-=- this should be fine ^



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
        .position = {.x = start.x, .y = start.y},
        .f = 0,
        .g = 0,
        .h = heuristic(start, end)
    };
    //open_list[start.y][start.x] = start_node;
    arraylist_add(open_list, &start_node);
    came_from[start.y][start.x] = start_node.parent;
    path_taken[start.y][start.x] = true;
    //closed_list[start.y][start.x] = true;
    // -=-=- this should be fine ^

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
            //printf("ok we are looping through openlistlength\n");
            currentNode = (Node*)arraylist_get(open_list, i);
            printf("Current Nodes in Open List: %d, %d\n", currentNode->position.x, currentNode->position.y);
            if (currentNode->f >= 0 && (!closed_list[currentNode->position.y][currentNode->position.x]) &&
                (min_f == -1 || currentNode->f < min_f)) {
                //printf("ok we set some stuff\n");
                min_f = currentNode->f;
                current.x = currentNode->position.x;
                current.y = currentNode->position.y;
                nodeIndex = i;
            }
            //printf("ok we are ending loop\n");
        }
        printf("New current node: %d, %d\n", current.x, current.y);

        // -=-=- THIS WORKS BUT THE OPENLIST ITSELF IS NOT WORKING
        // for (int y = 0; y < MAP_ROWS; y++) {
        //     for (int x = 0; x < MAP_COLS; x++) {
        //         //printf("before if statement of finding lowet f\n");
        //         if (open_list[y][x].f >= 0 && (!closed_list[y][x]) &&
        //             (min_f == -1 || open_list[y][x].f < min_f)) {
        //             min_f = open_list[y][x].f;
        //             current.x = x;
        //             current.y = y;
        //             //printf("YEAH WE IN\n");
        //             printf("FOUND LOWEST F AT (%d, %d) of %d\n", current.x, current.y, open_list[y][x].f);
        //         }
        //     }
        // }
        // -=-=- 
        //printf("BEFORE PATH FOUND OR NO PATH POSSIBLE\n");

        // -=-=- this should be fine v
        // SPLIT INTO TWO FOR DEBUGGING PURPOSES // 3:16am me, isn't this breaking things???
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
        // -=-=- this should be fine ^


        // remove current node from open list
        printf("Before Open List Length: %d, Node Index: %d\n", openlistlength, nodeIndex);
        if (!openlistlength == 0) {
            arraylist_remove(open_list, nodeIndex);
        }
        openlistlength = arraylist_size(open_list);
        printf("After Removing Open List Length: %d, Node Index: %d\n", openlistlength, nodeIndex);
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

        // foreach neighbour of the current node
        for (int i = 0; i < 4; i++) {
            int dx = moves_list[i].dx;
            int dy = moves_list[i].dy;

            int neighbor_x = current.x + dx;
            int neighbor_y = current.y + dy;
            int tentative_g = currentNode->g + 1;
            printf("Current Position: %d, %d\n", current.x, current.y);
            printf("Current Neighbour being looked at %d, %d\n", neighbor_x, neighbor_y);

            Node neighborNode = {
                .parent = {.x = current.x, .y = current.y},
                .position = {.x = neighbor_x, .y = neighbor_y},
                .f = 0,
                .g = tentative_g,
                .h = 0
            };

            //printf("BEFORE WALKABLE\n");

            // if neighbour is not travesrable or neighbour is in closed list
            // skip to the next neighbour
            if (!is_walkable(map, neighbor_x, neighbor_y)) {
                printf("UNWALKABLE! NEIGHBOUR COORDS = %d, %d\n", neighbor_x, neighbor_y);
                continue;
            }
            if (closed_list[neighbor_y][neighbor_x]) {
                printf("CLOSED LIST! NEIGHBOUR COORDS = %d, %d\n", neighbor_x, neighbor_y);
                continue;
            }

            printf("PASSED THROUGH! NEIGHBOUR COORDS = %d, %d\n", neighbor_x, neighbor_y);
            path_taken[neighbor_y][neighbor_x] = true;
            //int tentative_g = open_list[current.y][current.x].g + 1;

            //bool neighbor_is_better = false;

            // if new path to neighbour is shorter OR 
            // neighbour is not in OPEN
            printf("current size of openlistlength: %d\n", openlistlength);
            printf("size of openlist is %d\n", arraylist_size(open_list));
            bool isNeighbourInOpen = false;

            if (openlistlength == 0) {
                isNeighbourInOpen = false;
            }
            // check if neighbour is in open list
            for (int i = 0; i < openlistlength; i++) {
                // printf("ok we loop once more?\n");
                printf("ok what is current size of openlistlength: %d\n", openlistlength);
                printf("ok size of openlist is %d\n", arraylist_size(open_list));
                void* result = arraylist_get(open_list, i);
                Node* currentNode = (Node*)result;
                //printf("do we crash here?\n");
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
                arraylist_add(open_list, &neighborNode);
                openlistlength = arraylist_size(open_list);
                //     printf("current = %d, %d\n", current.x, current.y);
                //     printf("came_from[%d][%d] = %d, %d\n", neighbor_x, neighbor_y, came_from[neighbor_y][neighbor_x].x, came_from[neighbor_y][neighbor_x].y);
                printf("ok we add node\n");
                printf("ok what is current size of openlistlength: %d\n", openlistlength);
                printf("ok size of openlist is %d\n", arraylist_size(open_list));
                for (int i = 0; i < openlistlength; i++) {
                    Node* currentNode = (Node*)arraylist_get(open_list, i);
                    printf("Current Nodes in Open List (checking inside neighbour check): %d, %d\n", currentNode->position.x, currentNode->position.y);
                }
            }

            //printf("==-=-==-==before if=-=-=-==-=\n");
            // if (!open_list[neighbor_y][neighbor_x].f || tentative_g < open_list[neighbor_y][neighbor_x].g) {
            //     neighbor_is_better = true;
            //     open_list[neighbor_y][neighbor_x].g = tentative_g;
            //     open_list[neighbor_y][neighbor_x].h = heuristic(
            //         (Location){.x = neighbor_x, .y = neighbor_y},
            //         end
            //     );
            //     open_list[neighbor_y][neighbor_x].f = open_list[neighbor_y][neighbor_x].g +
            //                                         open_list[neighbor_y][neighbor_x].h;
            //     came_from[neighbor_y][neighbor_x] = current;
            //     printf("current = %d, %d\n", current.x, current.y);
            //     printf("came_from[%d][%d] = %d, %d\n", neighbor_x, neighbor_y, came_from[neighbor_y][neighbor_x].x, came_from[neighbor_y][neighbor_x].y);
            // }
            //printf("==-=-==-==after if=-=-=-==-=\n");

            // if (neighbor_is_better) {
            //     // Add the neighbor to the open list
            //     open_list[neighbor_y][neighbor_x].parent = current;
            // }
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
    bool *path_taken[MAP_ROWS][MAP_COLS] = {false};

    printf("Map\n");
    print_map_1();

    printf("BEFORE ASTAR ALGORTHM?\n");
    path_length = astar(map_1, start, end, path, path_taken);
    printf("AFTER ASTAR ALGORTHM?\n");


    // print map_1 with path ---
    // copy map
    
    int map_with_path[MAP_ROWS][MAP_COLS] = {0};
    for (int i = 0; i < MAP_ROWS; i++) {
        for (int j = 0; j < MAP_COLS; j++) {
            map_with_path[i][j] = map_1[i][j];
        }
    }
        printf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-\n");

    printf("Map with Path:\n");
    for (int i = path_length - 1; i >= 0; i--) {
        map_with_path[path[i].x][path[i].y] = 5;
    }
    printf("DO WE FINISH?\n");
    
    // debug testing, checking where we ended up
    for (int i = 0; i < MAP_ROWS; i++) {
        for (int j = 0; j < MAP_COLS; j++) {
            if (path_taken[i][j]) {
            map_with_path[i][j] = 9;
            }
        }
    }

    for (int i = 0; i < MAP_ROWS; i++) {
        for (int j = 0; j < MAP_COLS; j++) {
            printf("%d", map_with_path[i][j]);
        }
        printf("\n");
    }
    return 0;
}