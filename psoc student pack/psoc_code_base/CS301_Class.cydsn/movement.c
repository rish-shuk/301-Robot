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
#include <math.h>

#define MAX_MOVEMENT_PERCENTAGE 1.00 // 100% of PWM COUNT



float PWM_Forward_Percentage = 0.15;

uint16 turnCount = 0;
uint16 maxTurnCount = 25; // clock counts for one complete 90 deg turn

// Calculate Speed
uint16 setPWMAbove50(float percentage)
{
    uint16 compareCount = round((0.5 + percentage) * MAX_PWM_COUNT * MAX_MOVEMENT_PERCENTAGE);
    return compareCount;
}

uint16 setPWMBelow50(float percentage) 
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
    PWM_1_WriteCompare(setPWMAbove50(0)); // 50%
    PWM_2_WriteCompare(setPWMBelow50(0)); // 50%

    //PWM_1_WriteCompare(calculateSpeedCompareCount(50)); // 50%
    // PWM_2_WriteCompare(calculateSpeedCompareCount(50)); // 50%
}

// keep rotating clockwise
// used for adjusting deviations
void adjustRight()
{
    PWM_1_WriteCompare(setPWMAbove50(PWM_Forward_Percentage)); // 65% - Left Wheel Clockwise
    PWM_2_WriteCompare(setPWMBelow50(PWM_Forward_Percentage + 0.05)); // 40% - Right Wheel Clockwise
    // PWM_1_WriteCompare(calculateSpeedCompareCount(65)); // 65%
    // PWM_2_WriteCompare(calculateSpeedCompareCount(40)); // 40%
}

// keep rotating anticlockwise
// used for adjusting deviations
void adjustLeft()
{
    PWM_1_WriteCompare(setPWMAbove50(PWM_Forward_Percentage - 0.05)); // 60% - Left Wheel Clockwise
    PWM_2_WriteCompare(setPWMBelow50(PWM_Forward_Percentage)); // 35% - Right Wheel Clockwise
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
    PWM_1_WriteCompare(setPWMAbove50(PWM_Forward_Percentage)); // 65% - Left Wheel Clockwise
    PWM_2_WriteCompare(setPWMBelow50(PWM_Forward_Percentage)); // 35% - Right Wheel Clockwise
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
