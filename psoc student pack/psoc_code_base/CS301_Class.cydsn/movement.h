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

// Speed Macros
<<<<<<< HEAD
uint16 setPWMAbove50(float percentage);
uint16 setPWMBelow50(float percentage);
=======
uint16 calculateSpeed(uint16 count);

>>>>>>> parent of 8cfde69 (refactor: :clown_face: make calculate speed function name more descriptive)
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
