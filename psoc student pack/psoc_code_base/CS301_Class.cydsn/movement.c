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
#define maxForwardPWM 9830
#define maxTurningPWM 6553
#define movementMultiplier 0.8

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
    uint16 count1 = (32767 + (maxForwardPWM * movementMultiplier));
    uint16 count2 = (32767 - (maxTurningPWM * movementMultiplier));
    PWM_1_WriteCompare(count1); // 65% - Left Wheel Clockwise
    PWM_2_WriteCompare(count2); // 40% - Right Wheel Clockwise
}

// keep rotating anticlockwise
// used for adjusting deviations
void adjustLeft() {
    uint16 count1 = (32767 + (maxTurningPWM * movementMultiplier));
    uint16 count2 = (32767 - (maxForwardPWM * movementMultiplier));
    PWM_1_WriteCompare(count1); // 60% - Left Wheel Clockwise
    PWM_2_WriteCompare(count2); // 35% - Right Wheel Clockwise
}

// Anti-clockwise
void turnLeft() {
    uint16 count1 = (32767 - (maxTurningPWM * movementMultiplier));
    uint16 count2 = (32767 - (maxTurningPWM * movementMultiplier));
    PWM_1_WriteCompare(count1); // 40% -- Left Wheel spins Clockwise
    PWM_2_WriteCompare(count2); // 40% -- Right wheel spins clockwise
}

// Clockwise
void turnRight() {
    uint16 count1 = (32767 + (maxTurningPWM * movementMultiplier));
    uint16 count2 = (32767 + (maxTurningPWM * movementMultiplier));
    PWM_1_WriteCompare(count1); // 60% -- Left wheel spins clockwise
    PWM_2_WriteCompare(count2); // 60% - Right wheel spins Anti Clockwise
}

void moveForward() {
    uint16 count1 = (32767 + (maxForwardPWM * movementMultiplier));
    uint16 count2 = (32767 - (maxForwardPWM * movementMultiplier));
    PWM_1_WriteCompare(count1); // 65% - Left Wheel Clockwise
    PWM_2_WriteCompare(count2); // 35% - Right Wheel Clockwise
}

void moveBackward() {
    PWM_1_WriteCompare(22937); // 35% - Left Wheel Anti Clockwise
    PWM_2_WriteCompare(42598); // 65% - Right Wheel Anti Clockwise
}


/* [] END OF FILE */