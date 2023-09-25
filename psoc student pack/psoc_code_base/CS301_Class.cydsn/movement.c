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

// CHANGE SPEED
// max speed is 20.83cm/s
// min speed is 11.69cm/s
#define MOVEMENT_SPEED_MULTIPLIER 1.0

float rangeMovementMultiplier = 0.4;
float minMovementMultiplier = 0.7;

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
    // totalMultiplier = 0.5 + (0.7 * movement_speed_multiplier)
    float totalMultiplier = minMovementMultiplier + (rangeMovementMultiplier * MOVEMENT_SPEED_MULTIPLIER);
    
    uint16 count1 = (32767 + (maxForwardPWM * totalMultiplier));
    uint16 count2 = (32767 - (maxTurningPWM * totalMultiplier));
    PWM_1_WriteCompare(count1); // 65% - Left Wheel Clockwise
    PWM_2_WriteCompare(count2); // 40% - Right Wheel Clockwise
}

// keep rotating anticlockwise
// used for adjusting deviations
void adjustLeft() {
    // totalMultiplier = 0.5 + (0.7 * movement_speed_multiplier)
    float totalMultiplier = minMovementMultiplier + (rangeMovementMultiplier * MOVEMENT_SPEED_MULTIPLIER);
    
    uint16 count1 = (32767 + (maxTurningPWM * totalMultiplier));
    uint16 count2 = (32767 - (maxForwardPWM * totalMultiplier));
    PWM_1_WriteCompare(count1); // 60% - Left Wheel Clockwise
    PWM_2_WriteCompare(count2); // 35% - Right Wheel Clockwise
}

// Anti-clockwise
void turnLeft() {
    uint16 count1 = (32767 - (maxTurningPWM * 1));
    uint16 count2 = (32767 - (maxTurningPWM * 1));
    PWM_1_WriteCompare(count1); // 40% -- Left Wheel spins Clockwise
    PWM_2_WriteCompare(count2); // 40% -- Right wheel spins clockwise
}

// Clockwise
void turnRight() {
    uint16 count1 = (32767 + (maxTurningPWM * 1));
    uint16 count2 = (32767 + (maxTurningPWM * 1));
    PWM_1_WriteCompare(count1); // 60% -- Left wheel spins clockwise
    PWM_2_WriteCompare(count2); // 60% - Right wheel spins Anti Clockwise
}

void moveForward() {
    // totalMultiplier = 0.5 + (0.7 * movement_speed_multiplier)
    float totalMultiplier = minMovementMultiplier + (rangeMovementMultiplier * MOVEMENT_SPEED_MULTIPLIER);
    
    uint16 count1 = (32767 + (maxForwardPWM * totalMultiplier));
    uint16 count2 = (32767 - (maxForwardPWM * totalMultiplier));
    PWM_1_WriteCompare(count1); // 65% - Left Wheel Clockwise
    PWM_2_WriteCompare(count2); // 35% - Right Wheel Clockwise
}

void moveBackward() {
    PWM_1_WriteCompare(22937); // 35% - Left Wheel Anti Clockwise
    PWM_2_WriteCompare(42598); // 65% - Right Wheel Anti Clockwise
}


/* [] END OF FILE */