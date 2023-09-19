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
#include "initialise.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_MOVEMENT_PERCENTAGE 1.00 // 100% of PWM COUNT



float PWM_Forward_Percentage = 0.15;

uint16 turnCount = 0;
uint16 maxTurnCount = 25; // clock counts for one complete 90 deg turn

// Calculate Speed
uint16 setPWM1speed(float percentage)
{
    uint16 compareCount = round((0.5 + percentage) * MAX_PWM_COUNT * MAX_MOVEMENT_PERCENTAGE);
    return compareCount;
}

uint16 setPWM2speed(float percentage) 
{
    uint16 compareCount = round((0.5 - percentage) * MAX_PWM_COUNT * MAX_MOVEMENT_PERCENTAGE);
    return compareCount;
}

/*uint16 calculatePWM_1_SpeedCompareCount(uint8 percentage)
{
    float decimalSpeed = percentage / 100; // 50% / 100 = 0.5
    float maxDecimalSpeed = MAX_MOVEMENT_PERCENTAGE / 100;


    uint16 compareCount = (decimalSpeed * MAX_PWM_COUNT) * maxDecimalSpeed;
    return compareCount;
}*/

// Direction/ Movement macros
//* ========================================
// stop moving
void stopMoving()
{
    PWM_1_WriteCompare(setPWM1speed(0)); // 50%
    PWM_2_WriteCompare(setPWM2speed(0)); // 50%

    //PWM_1_WriteCompare(calculateSpeedCompareCount(50)); // 50%
    // PWM_2_WriteCompare(calculateSpeedCompareCount(50)); // 50%
}

// rotate clockwise 90deg
void rotationClockwise()
{
    PWM_1_WriteCompare(setPWM1speed(15)); // 65% (50% + 15%) // Left Wheel Clockwise
    // THE REASON WHY WE USE PWM1 IS BECAUSE FOR PWM_2, WHEN >50%, IT SPINS ANTICLOCKWISE 
    PWM_2_WriteCompare(setPWM1speed(15)); // 65% // Right Wheel AntiClockwise

    // PWM_1_WriteCompare(calculateSpeedCompareCount(65)); // 65%
    // PWM_2_WriteCompare(calculateSpeedCompareCount(65)); // 65%

    int quadPulseCount = 0;
    QuadDec_M1_SetCounter(0);
    while (quadPulseCount < 110)
    {
        quadPulseCount = QuadDec_M1_GetCounter();
    }
    QuadDec_M1_SetCounter(0);
    // CyDelay(500); // change to quadrature encoder pulses, rather than time delay
    // stopMoving(); // stop movement, ready for next instruction
}

// rotate anticlockwise 90deg- CHANGE TO NOT USE TIMER BUT SIMPLY CHECK FOR NEW CONDITIONS????
void rotationAntiClockwise()
{
    PWM_1_WriteCompare(22937); // 35%
    PWM_2_WriteCompare(22937); // 35%

    // PWM_1_WriteCompare(calculateSpeedCompareCount(35)); // 35%
    // PWM_2_WriteCompare(calculateSpeedCompareCount(35)); // 35%

    int quadPulseCount = 0;
    QuadDec_M1_SetCounter(0);
    while (quadPulseCount > -105)
    {
        quadPulseCount = QuadDec_M1_GetCounter();
    }
    QuadDec_M1_SetCounter(0);
    // CyDelay(500);
    // stopMoving(); // stop movement, ready for next instruction
}

// keep rotating clockwise
// used for adjusting deviations
void adjustRight()
{
    PWM_1_WriteCompare(setPWM1speed(PWM_Forward_Percentage)); // 65% - Left Wheel Clockwise
    PWM_2_WriteCompare(setPWM2speed(PWM_Forward_Percentage + 0.05)); // 40% - Right Wheel Clockwise
    // PWM_1_WriteCompare(calculateSpeedCompareCount(65)); // 65%
    // PWM_2_WriteCompare(calculateSpeedCompareCount(40)); // 40%
}

// keep rotating anticlockwise
// used for adjusting deviations
void adjustLeft()
{
    PWM_1_WriteCompare(setPWM1speed(PWM_Forward_Percentage - 0.05)); // 60% - Left Wheel Clockwise
    PWM_2_WriteCompare(setPWM2speed(PWM_Forward_Percentage)); // 35% - Right Wheel Clockwise
    // PWM_1_WriteCompare(calculateSpeedCompareCount(60)); // 60%
    // PWM_2_WriteCompare(calculateSpeedCompareCount(35)); // 35%
    //  love cock and balls
}

// Anti-clockwise
void turnLeft()
{
    PWM_1_WriteCompare(26214); // 40% -- Left Wheel spins Clockwise
    PWM_2_WriteCompare(26214); // 40% -- Right wheel spins clockwise
    // PWM_1_WriteCompare(calculateSpeedCompareCount(40)); // 40%
    // PWM_2_WriteCompare(calculateSpeedCompareCount(40)); // 40%
}

// Clockwise
void turnRight()
{
    PWM_1_WriteCompare(39321); // 60% -- Left wheel spins clockwise
    PWM_2_WriteCompare(39321); // 60% - Right wheel spins Anti Clockwise
    // PWM_1_WriteCompare(calculateSpeedCompareCount(60)); // 60%
    // PWM_2_WriteCompare(calculateSpeedCompareCount(60)); // 60%
}

void moveForward()
{
    PWM_1_WriteCompare(setPWM1speed(PWM_Forward_Percentage)); // 65% - Left Wheel Clockwise
    PWM_2_WriteCompare(setPWM2speed(PWM_Forward_Percentage)); // 35% - Right Wheel Clockwise
    // PWM_1_WriteCompare(calculateSpeedCompareCount(65)); // 65%
    // PWM_2_WriteCompare(calculateSpeedCompareCount(35)); // 35%
}











void moveBackward()
{
    PWM_1_WriteCompare(22937); // 35% - Left Wheel Anti Clockwise
    PWM_2_WriteCompare(42598); // 65% - Right Wheel Anti Clockwise
    // PWM_1_WriteCompare(calculateSpeedCompareCount(35)); // 35%
    // PWM_2_WriteCompare(calculateSpeedCompareCount(63)); // 65%
}

/* [] END OF FILE */
