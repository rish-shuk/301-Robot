#include <stdio.h>
#include <stdlib.h>

#define ROWS 15
#define COLS 19

void readMap(const char *filePath, int map[ROWS][COLS]) {
    FILE *file = fopen(filePath, "r");

    if (file == NULL) {
        perror("Error opening file");
        exit(1); // Exit the program if file opening fails
    }

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (fscanf(file, "%d", &map[i][j]) != 1) {
                fprintf(stderr, "Error reading data from file");
                fclose(file);
                exit(1); // Exit the program if reading data fails
            }
        }
    }

    fclose(file); // Close text file
}

int main() {
    int map[ROWS][COLS];
    readMap("maps/map1.txt", map);

    // Print the read array for verification
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            printf("%d ", map[i][j]);
        }
        printf("\n");
    }

    return 0;
}
