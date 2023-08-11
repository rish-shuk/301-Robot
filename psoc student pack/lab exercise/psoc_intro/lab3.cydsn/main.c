
/* ========================================
 * Laboratory Exercise 3
 *
 * Program timer to flash LED at 1Hz, use interrupts
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

CY_ISR(InterruptHandler) {
    LED_Write(~LED_Read()); // toggle LED
    Timer_1_ReadStatusRegister(); // clear interrupt
}

int main()
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    timer_clock_Start(); // start clock
    Timer_1_Start(); // start timer
    isr_TC_StartEx(InterruptHandler); // start and enable interrupt

    for(;;)
    {
        /* Place your application code here. */
       
    }

    
}

/* [] END OF FILE */
