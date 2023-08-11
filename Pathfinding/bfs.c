#include <stdio.h>

int test = 0;

int main()
{
    const int rows = 15;
    const int cols = 19;
    int arr[rows][cols]; // Initializes all elements to 0

    // Access and modify array elements if needed
    arr[0][0] = 42;

    // Printing the array
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("[%2d], ", arr[i][j]); // Using %4d to print each element with 4-character width
        }
        printf("\n"); // Move to the next row after each row is printed
    }

    return 0;
}
