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

void moveForwardForSpecifiedCount() {
    PWM_1_WriteCompare(42598); // 65%
    PWM_2_WriteCompare(22937); // 35%
    
    int quadPulseCount = 0;
    QuadDec_M1_SetCounter(0);
    while(quadPulseCount < 110) {
        quadPulseCount = QuadDec_M1_GetCounter();
    }
    QuadDec_M1_SetCounter(0);
}

// rotate clockwise 90deg
void rotationClockwise() {
    PWM_1_WriteCompare(42598); // 65%
    PWM_2_WriteCompare(42598); // 65%
    
    int quadPulseCount = 0;
    QuadDec_M1_SetCounter(0);
    while(quadPulseCount < 110) {
        quadPulseCount = QuadDec_M1_GetCounter();
    }
    QuadDec_M1_SetCounter(0);
    //CyDelay(500); // change to quadrature encoder pulses, rather than time delay
    //stopMoving(); // stop movement, ready for next instruction
}

// rotate anticlockwise 90deg- CHANGE TO NOT USE TIMER BUT SIMPLY CHECK FOR NEW CONDITIONS????
void rotationAntiClockwise() {
    PWM_1_WriteCompare(22937); // 35%
    PWM_2_WriteCompare(22937); // 35%
    
    int quadPulseCount = 0;
    QuadDec_M1_SetCounter(0);
    while(quadPulseCount > -105) {
        quadPulseCount = QuadDec_M1_GetCounter();  
    }
    QuadDec_M1_SetCounter(0);
    //CyDelay(500);
    //stopMoving(); // stop movement, ready for next instruction
}

// keep rotating clockwise
// used for adjusting deviations
void adjustRight() {
    PWM_1_WriteCompare(39321); // 60% -- Left wheel spins clockwise
    PWM_2_WriteCompare(39321); // 60%
    
}

// keep rotating anticlockwise
// used for adjusting deviations
void adjustLeft() {
    LED_Write(~(LED_Read()));
    PWM_1_WriteCompare(26214); // 40% -- Left Wheel spins Clockwise
    PWM_2_WriteCompare(26214); // 40% -- Right wheel spins clockwise
    //PWM_1_WriteCompare(36044); // 55% -- Left Wheel spins Clockwise
    //PWM_2_WriteCompare(26214); // 40% -- Right wheel spins clockwise
}

void moveForward() {
    PWM_1_WriteCompare(42598); // 65% - Left Wheel Clockwise
    PWM_2_WriteCompare(22937); // 35% - Right Wheel Clockwise
}


/* [] END OF FILE */
