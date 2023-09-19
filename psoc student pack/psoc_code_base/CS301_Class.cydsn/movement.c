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
#include "project.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

uint16 turnCount = 0;
uint16 maxTurnCount = 25; // clock counts for one complete 90 deg turn


// Direction/ Movement macros
//* ========================================
// stop moving
void stopMoving() {
    PWM_1_WriteCompare(32768); // 50%
    PWM_2_WriteCompare(32768); // 50%
}

// keep rotating clockwise
// used for adjusting deviations
void adjustRight() {
    PWM_1_WriteCompare(42598); // 65% - Left Wheel Clockwise
    PWM_2_WriteCompare(26214); // 40% - Right Wheel Clockwise
}

// keep rotating anticlockwise
// used for adjusting deviations
void adjustLeft() {
    PWM_1_WriteCompare(39321); // 60% - Left Wheel Clockwise
    PWM_2_WriteCompare(22937); // 35% - Right Wheel Clockwise
}

// Anti-clockwise
void turnLeft() {
    PWM_1_WriteCompare(26214); // 40% -- Left Wheel spins Clockwise
    PWM_2_WriteCompare(26214); // 40% -- Right wheel spins clockwise
}

// Clockwise
void turnRight() {
    PWM_1_WriteCompare(39321); // 60% -- Left wheel spins clockwise
    PWM_2_WriteCompare(39321); // 60% - Right wheel spins Anti Clockwise
}

void moveForward() {
    PWM_1_WriteCompare(42598); // 65% - Left Wheel Clockwise
    PWM_2_WriteCompare(22937); // 35% - Right Wheel Clockwise
}

void moveBackward() {
    PWM_1_WriteCompare(22937); // 35% - Left Wheel Anti Clockwise
    PWM_2_WriteCompare(42598); // 65% - Right Wheel Anti Clockwise
}


/* [] END OF FILE */
