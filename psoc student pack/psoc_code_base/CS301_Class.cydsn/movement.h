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
uint16 setPWMAbove50(float percentage);
uint16 setPWMBelow50(float percentage);
uint16 calculateSpeed(uint16 count);

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

//void RotateClockwise180Degrees();

void HardForward();
#endif /* MOVEMENT_FUNCTIONS_H */
