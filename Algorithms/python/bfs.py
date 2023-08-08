import numpy as np
import os

# READING MAP
# =============================================================================

def read_map_file(filename):
    with open(filename, 'r') as file:
        lines = file.readlines()

    # Remove newlines from each line and convert them into integers
    map_data = [[int(cell) for cell in line.strip()] for line in lines]

    return np.array(map_data)

# Replace 'map.txt' with the path to your map file
map_file = os.getcwd() + '\Algorithms\student-pack-stuff\matlab\map_1.txt'
map_array = read_map_file(map_file)

# Print the resulting 2D array
print(map_array)
# READING MAP
# =============================================================================

# Method 1: Using nested lists
# rows = 15
# columns = 19

# # Initializing the array with zeros
# array = [[0 for _ in range(columns)] for _ in range(rows)]

# # Print the array
# for row in array:
#     print(f'{row}, ')



def GetMapFromFile(filepath = '\student-pack-stuff\matlab\map_1.txt'):
    # ASSUMPTIONS: Map is always 15 (rows) x 19 (columns) and ends with a new line


    # 1 == Path, 0 == Cannot got

    # get map txt from file path
    # read map file and convert into 2d array
    # return 2d array

    return 