#include <stdio.h>
#include <stdlib.h>
#include "map_1.h"
#include "map_8.h"

void print_map_1() {
    // Access and print the array data
    for (int i = 0; i < NUM_ROWS; i++) {
        for (int j = 0; j < NUM_COLS; j++) {
            printf("%d", map_1[i][j]);
        }
        printf("\n");
    }
}

void print_map_8() {
    // Access and print the array data
    for (int i = 0; i < NUM_ROWS; i++) {
        for (int j = 0; j < NUM_COLS; j++) {
            printf("%d", map_8[i][j]);
        }
        printf("\n");
    }
}

void print_path_on_map_X() {
    
}
//------------------------------------------------
struct Location {
    int x;
    int y;
};

struct Node {
    struct Location location;
    int g;
    int h;
    int f;
    int index;
};

struct List {
    struct Node* nodes[300];
};

int manhattan_distance(struct Location startLocation, struct Location targetLocation) {
    // Manhattan distance is the sum of the absolute values of the horizontal and the vertical distance
    // between two points on a grid
    // We can use this to calculate the heuristic value of a node as our maze will only be in 4 directions.
    int x1 = startLocation.x;
    int y1 = startLocation.y;
    int x2 = targetLocation.x;
    int y2 = targetLocation.y;
    int dx = abs(x1 - x2);
    int dy = abs(y1 - y2);
    return dx + dy;
}

struct Node get_lowest_f(struct List list) {
    // return the node with the lowest f value
    struct Node lowest_f_node;
    lowest_f_node.f = 1000;
    for (int i = 0; i < 300; i++) {
        if (list.nodes[i] != NULL) {
            if (list.nodes[i]->f < lowest_f_node.f) {
                lowest_f_node = *list.nodes[i];
                lowest_f_node.index = i;
            }
        }
    }
    return lowest_f_node;
}

// return a pointer to a 2D array of ints -- The steps/coordinates to take to get to the target node
// take in the start location and end location
int** astar(struct Location startLocation, struct Location targetLocation) {
    // A* Search Algorithm
    // 1.  Initialize the open list
    struct List openList;
    // 2.  Initialize the closed list
    struct List closedList;
    // Initialse start and target nodes
    struct Node startNode;
    struct Node targetNode;
    startNode.location = startLocation; 
    startNode.f = 0;
    startNode.g = 0;
    startNode.h = 0;
    startNode.index = 0;
    targetNode.location = targetLocation;
    //     put the starting node on the open
    //     list (you can leave its f at zero)
    openList.nodes[0] = &startNode;

    // 3.  while the open list is not empty
    while (openList.nodes[0] != NULL) {
        // a) find the node with the least f on the open list, set this to be the current node.
        struct Node currentNode;
        currentNode = get_lowest_f(openList);
        // b) pop currentNode off the open list, and add to closed list
        int listIndex = 0;

    }   


}





  
//     c) generate q's 8 successors and set their 
//        parents to q
   
//     d) for each successor
//         i) if successor is the goal, stop search
        
//         ii) else, compute both g and h for successor
//           successor.g = q.g + distance between 
//                               successor and q
//           successor.h = distance from goal to 
//           successor (This can be done using many 
//           ways, we will discuss three heuristics- 
//           Manhattan, Diagonal and Euclidean 
//           Heuristics)
          
//           successor.f = successor.g + successor.h

//         iii) if a node with the same position as 
//             successor is in the OPEN list which has a 
//            lower f than successor, skip this successor

//         iV) if a node with the same position as 
//             successor  is in the CLOSED list which has
//             a lower f than successor, skip this successor
//             otherwise, add  the node to the open list
//      end (for loop)
  
//     e) push q on the closed list
//     end (while loop)


int main() {

    
    return 0;
}