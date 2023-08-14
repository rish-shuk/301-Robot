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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <project.h>
//* ========================================
#include "defines.h"
#include "vars.h"


void init() {
    Clock_PWM_Start();
    PWM_1_Start();
    PWM_1_WritePeriod(100);
    PWM_2_Start();
    PWM_2_WritePeriod(100);

    // start quadrature decoders
    QuadDec_M1_Start();
    QuadDec_M2_Start();
    SpeedTimer_Start();
    SpeedClock_Start();
}

/* [] END OF FILE */
