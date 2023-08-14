/* ========================================
 * Fully working code: 
 * PWM      : 
 * Encoder  : 
 * ADC      :
 * USB      : port displays speed and position.
 * CMD: "PW xx"
 * Copyright Univ of Auckland, 2016
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF Univ of Auckland.
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
#include "pathfinding.h"
#include "initialise.h"
#include "movement.h"

//* ========================================
void usbPutString(char *s);
void usbPutChar(char c);
void handle_usb();
void changeDutyCycle(float percentage);
void reverseRightWheel();
void forwardRightWheel();
void forwardLeftWheel();
void reverseLeftWheel();
void rotationClockwise();
void rotationAntiClockwise();
void directionForward();
void directionBackward();
void stopMoving();
void quadCountToRPM(uint16 count);
//* ========================================
char buffer[69];
int quadDec2Count = 0;
int timerInt = 0;
uint16 turnCount = 0;
uint16 maxTurnCount = 25; // clock counts for one complete 90 deg turn


CY_ISR (speedTimer) {
    timerInt = 1;
    // quadDec_M1 used for turning macros
    quadDec2Count = QuadDec_M2_GetCounter();
    QuadDec_M2_SetCounter(0); // reset count
    QuadDec_M2_Start(); // restart counter
    SpeedTimer_ReadStatusRegister(); // clear interrupt
}
int main()
{
// --------------------------------    
// ----- INITIALIZATIONS ----------
    CYGlobalIntEnable;
    init(); // initialise clocks, pwms, adc, dac etc- done in header file
    isr_speed_StartEx(speedTimer); // start interrupt
    
// ------USB SETUP ----------------    
#ifdef USE_USB    
    USBUART_Start(0,USBUART_5V_OPERATION);
#endif        
#ifdef USE_USB    
    USBUART_Start(0,USBUART_5V_OPERATION);
#endif        
        
    RF_BT_SELECT_Write(0);
    
    for(;;)
    {
        rotationClockwise();
        rotationAntiClockwise();
        
        if(timerInt == 1) {
            // calculate RPM of M2
            quadCountToRPM(quadDec2Count);
        }
        
        if (flag_KB_string == 1)
        {
            //usbPutString(line);
            flag_KB_string = 0;

        }           
    }   
}

// Calculations
//* ========================================
void quadCountToRPM(uint16 count)
{
    float cps = count/57.00;
    int16 rpm = (int16)(cps*15); // rpm value
    sprintf(buffer, "%d", rpm); // store in buffer
    //usbPutString(buffer);
    //usbPutString("rpm ");
}

/* [] END OF FILE */