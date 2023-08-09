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

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            int ch = fgetc(file);
            map[i][j] = ch;
        }
    }

    fclose(file); // Close text file
}

int main() {
    int map[ROWS][COLS];
    readMap("map_1.txt", map);

    // Print the characters in the map without newlines
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (map[i][j] != '\n') {
                printf("%c", map[i][j]);
            }
        }
        printf("\n");
    }


    return 0;
}
