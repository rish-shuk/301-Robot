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
#include <project.h>

void init() {
    Clock_PWM_Start();
    PWM_1_Start();
    PWM_1_WritePeriod(65535);
    PWM_2_Start();
    PWM_2_WritePeriod(65535);

    // start quadrature decoders
    QuadDec_M1_Start();
    QuadDec_M2_Start();
    SpeedTimer_Start();
    SpeedClock_Start();
}

/* [] END OF FILE */
