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
    PWM_1_WriteCompare(32768);
    PWM_2_WriteCompare(32768);
}

// rotate clockwise 90deg- will repeat until sensors are in relevant positions
void rotationClockwise() {
    PWM_1_WriteCompare(42598);
    PWM_2_WriteCompare(42598); // 65%
    
    /*int quadPulseCount = 0;
    QuadDec_M1_SetCounter(0);
    while(quadPulseCount < 110) {
        quadPulseCount = QuadDec_M1_GetCounter();
    }
    QuadDec_M1_SetCounter(0);*/
    //CyDelay(500); // change to quadrature encoder pulses, rather than time delay
    //stopMoving(); // stop movement, ready for next instruction
}

// rotate anticlockwise 90deg- will repeat until sensors are in relevant positions
void rotationAntiClockwise() {
    PWM_2_WriteCompare(22937);
    PWM_1_WriteCompare(22937); // 35%
    
    /*int quadPulseCount = 0;
    QuadDec_M1_SetCounter(0);
    while(quadPulseCount > -105) {
        quadPulseCount = QuadDec_M1_GetCounter();  
    }
    QuadDec_M1_SetCounter(0);*/
    //CyDelay(500);
    //stopMoving(); // stop movement, ready for next instruction
}

// keep rotating clockwise
/*void keepRotatingClockwise() {
    PWM_1_WriteCompare(65);
    PWM_2_WriteCompare(65);
    
}

// keep rotating anticlockwise
void keepRotatingAntiClockwise() {
    PWM_1_WriteCompare(35);
    PWM_2_WriteCompare(35);
}*/

void moveForward() {
    PWM_1_WriteCompare(42598); // 65%
    PWM_2_WriteCompare(22937); // 35%
}

void traverseLine() {
    // robot will run this code on a loop based on truth table 
    
    
}

/* [] END OF FILE */
