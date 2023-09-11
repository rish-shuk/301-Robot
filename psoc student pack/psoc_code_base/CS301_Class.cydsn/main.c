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
#include "usbUART.h"

//* ========================================
void quadCountToRPM(uint16 count);
//* ========================================
char buffer[69];
int quadDec2Count = 0;
int timerInt = 0;
int keepLedOn = 0;
uint8 isHigh = 0; // flag to detect high
uint16 adcResultMVprev = 0;
uint16 adcResultMV;
uint8 readings[100] = {0};
uint8 counter = 0;

char map[MAX_ROWS][MAX_COLS]; // global map array- stores the map

CY_ISR (speedTimer) {
    timerInt = 1;
    // quadDec_M1 used for turning macros
    quadDec2Count = QuadDec_M2_GetCounter();
    QuadDec_M2_SetCounter(0); // reset count
    QuadDec_M2_Start(); // restart counter
    SpeedTimer_ReadStatusRegister(); // clear interrupt
}
<<<<<<< HEAD

CY_ISR(EDGE_DETECTED) {
    isHigh = 1; // set isHigh to 1
}

CY_ISR(TIMER_FINISH) {
    if(isHigh == 1) {
        LED_Write(1u);
    } else {
        LED_Write(0u);
    }

    isHigh = 0; // reset for next rising edge
    Timer_LED_ReadStatusRegister();
}

=======
>>>>>>> main
int main()
{
// --------------------------------    
// ----- INITIALIZATIONS ----------
    CYGlobalIntEnable;
    init(); // initialise clocks, pwms, adc, dac etc- done in header file
    //findPath(map, "");// find shortest path- store this in map
    isr_speed_StartEx(speedTimer); // start interrupt
<<<<<<< HEAD
    isr_Timer_LED_StartEx(TIMER_FINISH);
    edge_detected_StartEx(EDGE_DETECTED);
    Timer_LED_Start();
    
=======
>>>>>>> main
    
// ------USB SETUP ----------------    
#ifdef USE_USB    
    USBUART_Start(0,USBUART_5V_OPERATION);
#endif        
#ifdef USE_USB    
    USBUART_Start(0,USBUART_5V_OPERATION);
#endif        
        
    RF_BT_SELECT_Write(0);
    
<<<<<<< HEAD
    for(;;) {
        if (counter == 100) {
            usbPutString("White Samples: ");
            int whiteCounter = 0;
            for (int i = 0; i < 100; i++) {
                if (readings[i] == 1) {
                    whiteCounter++;   
                }
            }
            char* buffer[64];
            snprintf(*buffer, 64, "White Counter: ");
            usbPutString(*buffer);
=======
    for(;;)
    {
        //traverseMap(map);
        rotationAntiClockwise();
        rotationClockwise();
        
        if(timerInt == 1) {
            // calculate RPM of M2
            quadCountToRPM(quadDec2Count);
>>>>>>> main
        }
        
        if (flag_KB_string == 1)
        {
            //usbPutString(line);
            flag_KB_string = 0;

        }           
    }
    return 0;
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
