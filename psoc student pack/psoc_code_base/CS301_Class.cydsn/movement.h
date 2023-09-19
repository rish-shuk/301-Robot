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

// keep rotating clockwise
void adjustRight();

// keep rotating anticlockwise
void adjustLeft();

// Move forward
void moveForward();

void turnRight();

void turnLeft();

void moveBackward();

#endif /* MOVEMENT_FUNCTIONS_H */
