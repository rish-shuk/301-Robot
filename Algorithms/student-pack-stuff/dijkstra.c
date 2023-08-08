#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_ROWS 15
#define MAX_COLS 19

struct Location {
    int row;
    int col;
};

struct PriorityQueue {
    struct Location location;
    int distance;
};

struct Location moves[] = { {0, 1}, {0, -1}, {1, 0}, {-1, 0} };

bool isValidMove(int r, int c, int rows, int cols, int map[MAX_ROWS][MAX_COLS]) {
    return (r >= 0 && r < rows && c >= 0 && c < cols && map[r][c] == 0);
}

void dijkstra(int map[MAX_ROWS][MAX_COLS], int rows, int cols, struct Location startlocation, struct Location targetlocation) {
    int visitedMap[MAX_ROWS][MAX_COLS];
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            visitedMap[i][j] = 1;
        }
    }

    struct PriorityQueue pq[MAX_ROWS * MAX_COLS];
    int pqSize = 0;

    // Initialize distances and paths arrays
    int distances[MAX_ROWS][MAX_COLS];
    struct Location paths[MAX_ROWS][MAX_COLS];

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            distances[i][j] = INT_MAX;
        }
    }

    distances[startlocation.row][startlocation.col] = 0;
    paths[startlocation.row][startlocation.col] = startlocation;

    while (pqSize > 0) {
        int idx = 0;
        for (int i = 1; i < pqSize; i++) {
            if (pq[i].distance < pq[idx].distance) {
                idx = i;
            }
        }
        struct PriorityQueue current = pq[idx];
        pq[idx] = pq[pqSize - 1];
        pqSize--;

        int row = current.location.row;
        int col = current.location.col;

        if (visitedMap[row][col] == 1) {
            visitedMap[row][col] = 0;

            if (row == targetlocation.row && col == targetlocation.col) {
                break;
            }

            for (int i = 0; i < sizeof(moves) / sizeof(moves[0]); i++) {
                int newRow = row + moves[i].row;
                int newCol = col + moves[i].col;

                if (isValidMove(newRow, newCol, rows, cols, map) && visitedMap[newRow][newCol] == 1) {
                    int newDistance = distances[row][col] + 1;

                    if (newDistance < distances[newRow][newCol]) {
                        distances[newRow][newCol] = newDistance;
                        paths[newRow][newCol] = (struct Location){row, col};

                        pq[pqSize].location.row = newRow;
                        pq[pqSize].location.col = newCol;
                        pq[pqSize].distance = newDistance;
                        pqSize++;
                    }
                }
            }
        }
    }

    // Process results and print paths
    // (You'll need to implement the path processing and printing here)
}

int main() {
    // Define and read the map
    int map[MAX_ROWS][MAX_COLS];
    int rows, cols;
    // Read map and rows, cols values

    struct Location startlocation = { /* Set start row and col */ };
    struct Location targetlocation = { /* Set target row and col */ };

    dijkstra(map, rows, cols, startlocation, targetlocation);

    return 0;
}
