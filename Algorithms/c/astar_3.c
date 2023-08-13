#include <stdio.h>
#include <stdbool.h>

typedef struct {
    int row;
    int col;
} Location;

typedef struct {
    Location location;
    Location* path;
    int f;
} LocationInfo;

void copyPath(Location* src, Location* dest, int length) {
    for (int i = 0; i < length; i++) {
        dest[i] = src[i];
    }
}

bool isValidMove(int r, int c, int rows, int cols, int** map) {
    return r >= 0 && r < rows && c >= 0 && c < cols && map[r][c] == 0;
}

void astar(int** map, int rows, int cols, Location startlocation, Location targetlocation, int* retmap, int* retvisited, Location* retsteps) {
    Location* visitedMap = malloc(rows * sizeof(Location));
    for (int i = 0; i < rows; i++) {
        visitedMap[i] = malloc(cols * sizeof(Location));
    }

    Location moves[4] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};

    LocationInfo* openList = malloc(sizeof(LocationInfo));
    openList[0].location = startlocation;
    openList[0].path = malloc(sizeof(Location));
    copyPath(&startlocation, openList[0].path, 1);
    openList[0].f = 0;
    int openListSize = 1;

    LocationInfo* closedList = malloc(sizeof(LocationInfo));
    int closedListSize = 0;

    Location* steps = malloc(sizeof(Location));
    int stepsSize = 0;

    while (openListSize > 0) {
        int minIndex = 0;
        for (int i = 1; i < openListSize; i++) {
            if (openList[i].f < openList[minIndex].f) {
                minIndex = i;
            }
        }
        LocationInfo current = openList[minIndex];
        for (int i = minIndex; i < openListSize - 1; i++) {
            openList[i] = openList[i + 1];
        }
        openListSize--;

        int row = current.location.row;
        int col = current.location.col;

        if (visitedMap[row][col].row == 0 && *visitedMap[row][col].col == 0) {
            continue;
        }

        visitedMap[row][col].row = 0;
        visitedMap[row][col].col = 0;

        if (row == targetlocation.row && col == targetlocation.col) {
            break;
        }

        for (int i = 0; i < 4; i++) {
            int newRow = row + moves[i].row;
            int newCol = col + moves[i].col;

            if (isValidMove(newRow, newCol, rows, cols, map) && visitedMap[newRow][newCol].row == 1 && visitedMap[newRow][newCol].col == 1) {
                Location* newPath = malloc((current.pathLength + 1) * sizeof(Location));
                copyPath(current.path, newPath, current.pathLength);
                newPath[current.pathLength] = (Location){newRow, newCol};
                int g = current.pathLength + 1;
                int h = abs(newRow - targetlocation.row) + abs(newCol - targetlocation.col);
                int f = g + h;

                LocationInfo newLocation;
                newLocation.location = (Location){newRow, newCol};
                newLocation.path = newPath;
                newLocation.pathLength = current.pathLength + 1;
                newLocation.f = f;

                bool inOpenList = false;
                for (int j = 0; j < openListSize; j++) {
                    if (openList[j].location.row == newLocation.location.row && openList[j].location.col == newLocation.location.col) {
                        inOpenList = true;
                        steps[stepsSize] = (Location){row, col};
                        stepsSize++;
                        break;
                    }
                }

                bool inClosedList = false;
                for (int j = 0; j < closedListSize; j++) {
                    if (closedList[j].location.row == newLocation.location.row && closedList[j].location.col == newLocation.location.col) {
                        inClosedList = true;
                        break;
                    }
                }

                if (!inOpenList && !inClosedList) {
                    openList[openListSize] = newLocation;
                    openListSize++;
                }
            }
        }

        closedList[closedListSize] = current;
        closedListSize++;
    }

    // Cleanup and assign results
    for (int i = 0; i < rows; i++) {
        free(visitedMap[i]);
    }
    free(visitedMap);

    for (int i = 0; i < openListSize; i++) {
        free(openList[i].path);
    }
    free(openList);

    for (int i = 0; i < closedListSize; i++) {
        free(closedList[i].path);
    }
    free(closedList);

    for (int i = 0; i < stepsSize; i++) {
        retsteps[i] = steps[i];
    }
    free(steps);

    // Copy map and visitedMap to retmap and retvisited
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            retmap[i * cols + j] = map[i][j];
            retvisited[i * cols + j] = visitedMap[i][j].row == 0 && visitedMap[i][j].col == 0 ? 0 : 1;
        }
    }
}

int main() {
    // You would need to provide appropriate values for map, rows, cols, startlocation, and targetlocation

    return 0;
}