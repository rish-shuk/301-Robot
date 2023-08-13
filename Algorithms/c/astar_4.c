#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Define the heuristic function (Euclidean distance)
float heuristic(int ax, int ay, int bx, int by) {
    return sqrt((ax - bx) * (ax - bx) + (ay - by) * (ay - by));
}

// Define the A* algorithm
int astar(int **grid, int rows, int cols, int start_x, int start_y, int end_x, int end_y, int **path, int *path_len) {
    typedef struct {
        int x;
        int y;
        float f_score;
    } Node;

    int dx[4] = {1, -1, 0, 0};
    int dy[4] = {0, 0, 1, -1};

    Node start = {start_x, start_y, heuristic(start_x, start_y, end_x, end_y)};
    Node *open_set = (Node *)malloc(rows * cols * sizeof(Node));
    open_set[0] = start;
    int open_set_size = 1;

    int **came_from = (int **)malloc(rows * sizeof(int *));
    for (int i = 0; i < rows; i++) {
        came_from[i] = (int *)malloc(cols * sizeof(int));
        for (int j = 0; j < cols; j++) {
            came_from[i][j] = -1;
        }
    }

    float **g_score = (float **)malloc(rows * sizeof(float *));
    for (int i = 0; i < rows; i++) {
        g_score[i] = (float *)malloc(cols * sizeof(float));
        for (int j = 0; j < cols; j++) {
            g_score[i][j] = INFINITY;
        }
    }
    g_score[start_x][start_y] = 0.0;

    float **f_score = (float **)malloc(rows * sizeof(float *));
    for (int i = 0; i < rows; i++) {
        f_score[i] = (float *)malloc(cols * sizeof(float));
        for (int j = 0; j < cols; j++) {
            f_score[i][j] = INFINITY;
        }
    }
    f_score[start_x][start_y] = heuristic(start_x, start_y, end_x, end_y);

    while (open_set_size > 0) {
        Node current = open_set[0];
        int current_idx = 0;

        // Find node with lowest f_score in open_set
        for (int i = 1; i < open_set_size; i++) {
            if (open_set[i].f_score < current.f_score) {
                current = open_set[i];
                current_idx = i;
            }
        }

        // Remove current from open_set
        open_set[current_idx] = open_set[open_set_size - 1];
        open_set_size--;

        if (current.x == end_x && current.y == end_y) {
            // Reconstruct path
            int path_idx = 0;
            int x = end_x;
            int y = end_y;
            while (x != start_x || y != start_y) {
                path[path_idx][0] = x;
                path[path_idx][1] = y;
                path_idx++;
                int tmp_x = x;
                x = came_from[tmp_x][y];
                y = came_from[tmp_x][y];
            }
            path[path_idx][0] = start_x;
            path[path_idx][1] = start_y;
            *path_len = path_idx + 1;

            // Cleanup and return success
            for (int i = 0; i < rows; i++) {
                free(came_from[i]);
                free(g_score[i]);
                free(f_score[i]);
            }
            free(came_from);
            free(g_score);
            free(f_score);
            free(open_set);
            return 1;
        }

        for (int i = 0; i < 4; i++) {
            int neighbor_x = current.x + dx[i];
            int neighbor_y = current.y + dy[i];

            if (neighbor_x >= 0 && neighbor_x < rows && neighbor_y >= 0 && neighbor_y < cols && grid[neighbor_x][neighbor_y] != 1) {
                float tentative_g_score = g_score[current.x][current.y] + 1;

                if (tentative_g_score < g_score[neighbor_x][neighbor_y]) {
                    came_from[neighbor_x][neighbor_y] = current.x;
                    g_score[neighbor_x][neighbor_y] = tentative_g_score;
                    f_score[neighbor_x][neighbor_y] = tentative_g_score + heuristic(neighbor_x, neighbor_y, end_x, end_y);

                    // Check if neighbor is already in open_set
                    int in_open_set = 0;
                    for (int j = 0; j < open_set_size; j++) {
                        if (open_set[j].x == neighbor_x && open_set[j].y == neighbor_y) {
                            in_open_set = 1;
                            break;
                        }
                    }

                    // Add neighbor to open_set if not already present
                    if (!in_open_set) {
                        Node neighbor_node = {neighbor_x, neighbor_y, f_score[neighbor_x][neighbor_y]};
                        open_set[open_set_size] = neighbor_node;
                        open_set_size++;
                    }
                }
            }
        }
    }

    // Cleanup and return failure
    for (int i = 0; i < rows; i++) {
        free(came_from[i]);
        free(g_score[i]);
        free(f_score[i]);
    }
    free(came_from);
    free(g_score);
    free(f_score);
    free(open_set);
    return 0;
}

int main() {
    int rows = 5;
    int cols = 5;
    int **matrix = (int **)malloc(rows * sizeof(int *));
    for (int i = 0; i < rows; i++) {
        matrix[i] = (int *)malloc(cols * sizeof(int));
    }

    // Initialize the matrix
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            matrix[i][j] = 0;
        }
    }
    matrix[1][1] = 1;
    matrix[1][3] = 1;
    matrix[3][1] = 1;
    matrix[3][3] = 1;

    int **path = (int **)malloc((rows * cols) * sizeof(int *));
    for (int i = 0; i < rows * cols; i++) {
        path[i] = (int *)malloc(2 * sizeof(int));
    }
    int path_len = 0;

    int start_x = 0;
    int start_y = 0;
    int end_x = 4;
    int end_y = 4;

    int result = astar(matrix, rows, cols, start_x, start_y, end_x, end_y, path, &path_len);

    if (result == 1) {
        printf("Path:");
        for (int i = 0; i < path_len; i++) {
            printf(" (%d, %d)", path[i][0], path[i][1]);
        }
        printf("\n");
    } else {
        printf("No path found.\n");
    }

    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
    for (int i = 0; i < rows * cols; i++) {
        free(path[i]);
    }
    free(path);

    return 0;
}
