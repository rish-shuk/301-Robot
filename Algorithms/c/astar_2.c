#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "map_1.h"
#include "map_8.h"

void print_map_1() {
    // Access and print the array data
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            printf("%d", map_1[i][j]);
        }
        printf("\n");
    }
}

void print_map_8() {
    // Access and print the array data
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
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

void reconstruct_path(Location came_from[][MAP_WIDTH], Location current, Location start, Location* path, int* path_length) {
    *path_length = 0;
    while (current.x != -1 && current.y != -1) {
        path[*path_length] = current;
        (*path_length)++;
        current = came_from[current.y][current.x];
    }
}

void astar(int map[MAP_HEIGHT][MAP_WIDTH], Location start, Location end, Location* path, int* path_length) {
    Node open_list[MAP_HEIGHT][MAP_WIDTH] = {0};
    bool closed_list[MAP_HEIGHT][MAP_WIDTH] = {false};
    Location came_from[MAP_HEIGHT][MAP_WIDTH] = {0};

    Node start_node = {
        .parent = {.x = -1, .y = -1},
        .f = 0,
        .g = 0,
        .h = heuristic(start, end)
    };
    open_list[start.y][start.x] = start_node;

    while (true) {
        Location current = {-1, -1};
        double min_f = -1;

        // Find the node with the lowest f value in the open list
        for (int y = 0; y < MAP_HEIGHT; y++) {
            for (int x = 0; x < MAP_WIDTH; x++) {
                if (open_list[y][x].f > 0 && (!closed_list[y][x]) &&
                    (min_f == -1 || open_list[y][x].f < min_f)) {
                    min_f = open_list[y][x].f;
                    current.x = x;
                    current.y = y;
                }
            }
        }

        if (current.x == -1 || current.x == end.x && current.y == end.y) {
            // Path found or no path possible
            break;
        }

        closed_list[current.y][current.x] = true;

        // Generate successors
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                if (dx == 0 && dy == 0) {
                    continue;
                }

                int neighbor_x = current.x + dx;
                int neighbor_y = current.y + dy;

                if (!is_walkable(map, neighbor_x, neighbor_y) || closed_list[neighbor_y][neighbor_x]) {
                    continue;
                }

                double tentative_g = open_list[current.y][current.x].g + 1;
                bool neighbor_is_better = false;

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
                }

                if (neighbor_is_better) {
                    // Add the neighbor to the open list
                    open_list[neighbor_y][neighbor_x].parent = current;
                }
            }
        }
    }

    reconstruct_path(came_from, end, start, path, path_length);
}

// using map_1
int main() {
    Location start = {.x = 1, .y = 1};
    Location end = {.x = 8, .y = 8};
    Location path[MAP_HEIGHT * MAP_WIDTH];
    int path_length;


    printf("Map\n");
    print_map_1();

    
    astar(map_1, start, end, path, &path_length);



    // print map_1 with path ---
    // copy map
    int map_with_path[MAP_HEIGHT][MAP_WIDTH] = {0};
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            map_with_path[i][j] = map_1[i][j];
        }
    }
        printf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-\n");

    printf("Map with Path:\n");
    for (int i = path_length - 1; i >= 0; i--) {
        map_with_path[path[i].x][path[i].y] = 5;
    }

    return 0;
}