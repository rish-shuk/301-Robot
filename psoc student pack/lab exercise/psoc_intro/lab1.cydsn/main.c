
/* ========================================
 * Laboratory Exercise 1
 *
 * Build and download project
 * Push SW1 to light led
 * 
 * Copyright Univ of Auckland, 2016
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF Univ of Auckland.
 *
 * ========================================
*/
#include <project.h>

int main()
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    //Clock_1_Start(); // start the clock
    uint8 pinState;
    for(;;)
    {
        /* Place your application code here. */
        pinState = SW1_Read();
        if(pinState > 0) {
            LED_Write(1u); // turn on LED when pressed
        } else {
            LED_Write(0u); // turn off LED when not pressed
        }
    }
}

/* [] END OF FILE */
