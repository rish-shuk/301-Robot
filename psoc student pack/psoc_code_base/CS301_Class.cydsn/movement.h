/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#ifndef MOVEMENT_FUNCTIONS_H
#define MOVEMENT_FUNCTIONS_H

#include "project.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Direction/ Movement macros
//* ========================================
// Stop moving
void stopMoving();

// Move forward by hard coded amount in function
void moveForwardForSpecifiedCount();

// Rotate clockwise 90 degrees
void rotationClockwise();

// Rotate anticlockwise 90 degrees
void rotationAntiClockwise();

// keep rotating clockwise
void keepRotatingClockwise();

// keep rotating anticlockwise
void keepRotatingAntiClockwise();

// Move forward
void moveForward();

// Traverse Line
void traverseLine();

#endif /* MOVEMENT_FUNCTIONS_H */
