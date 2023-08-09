#include <stdio.h>
#include <stdlib.h>

#define ROWS 15
#define COLS 19

void readMap(char *filePath, int map[ROWS][COLS]) {
    FILE *file = fopen(filePath, "r");

    if (file == NULL) {
        perror("Error opening file");
        exit(1); // Exit the program if file opening fails
    }

    // Read the file and populate the array
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            char c;
            do {
                c = fgetc(file);
            } while (c != '0' && c != '1' && c != EOF); // Skip characters that are not '0' or '1'
            if (c == EOF) {
                fclose(file);
                return; // Exit the function if end of file is reached
            }
            map[i][j] = c - '0';  // Convert character to integer
        }
    }

    fclose(file); // Close text file
}

int main() {
    int map[ROWS][COLS];
    readMap("map_1.txt", map);

    // Print the map in a grid format
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            printf("%d", map[i][j]);
        }
        printf("\n");
    }

    return 0;
}
