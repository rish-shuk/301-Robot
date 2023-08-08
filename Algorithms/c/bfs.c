#include <stdio.h>
#include <stdlib.h>

#define MAX_ROWS 100 // Maximum number of rows in the map
#define MAX_COLS 100 // Maximum number of columns in the map
int test = 0;

int main() {
    int map[MAX_ROWS][MAX_COLS];
    int rows = 0;

    // Open the file
    FILE *file = fopen("map.txt", "r");
    if (file == NULL) {
        printf("Failed to open the file.\n");
        return 1;
    }

    // Read lines and convert characters to integers
    char line[MAX_COLS];
    while (fgets(line, sizeof(line), file) != NULL) {
        for (int i = 0; line[i] != '\0' && line[i] != '\n'; i++) {
            map[rows][i] = line[i] - '0';
        }
        rows++;
    }

    // Close the file
    fclose(file);

    // Print the resulting 2D array
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < MAX_COLS; j++) {
            if (map[i][j] == 0) {
                printf("0 ");
            } else if (map[i][j] == 1) {
                printf("1 ");
            }
        }
        printf("\n");
    }

    return 0;
}