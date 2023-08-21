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
void quadCountToRPM(uint16 count);
//* ========================================
char buffer[69];
int quadDec2Count = 0;
int timerInt = 0;

char map[MAX_ROWS][MAX_COLS]; // global map array- stores the map

CY_ISR (speedTimer) {
    timerInt = 1;
    // quadDec_M1 used for turning macros
    quadDec2Count = QuadDec_M2_GetCounter();
    QuadDec_M2_SetCounter(0); // reset count
    QuadDec_M2_Start(); // restart counter
    SpeedTimer_ReadStatusRegister(); // clear interrupt
}

CY_ISR (ADC_CONV_FINISH) {
    //LED_Write(~LED_Read()); // flash LED on end of conversion 
    // get result from ADC channel 0, if greater than 2.5V
   if(ADC_CountsTo_mVolts(ADC_GetResult16(0)) > 2500) {
    LED_Write(1u);
    } else {
        LED_Write(0u);
    }
    ADC_StartConvert(); // start conversion again- software trigger
}   

int main()
{
// --------------------------------    
// ----- INITIALIZATIONS ----------
    CYGlobalIntEnable;
    init(); // initialise clocks, pwms, adc, dac etc- done in header file
    isr_eoc_StartEx(ADC_CONV_FINISH);
    //findPath(map, "");// find shortest path- store this in map
    isr_speed_StartEx(speedTimer); // start interrupt
    ADC_Start();
    ADC_StartConvert();
    //int32 adcVolts = 0;
    
// ------USB SETUP ----------------    
#ifdef USE_USB    
    USBUART_Start(0,USBUART_5V_OPERATION);
#endif         
        
    RF_BT_SELECT_Write(0);
    
    for(;;)
    {   
        //adcVolts = ADC_CountsTo_mVolts(ADC_GetResult16(0));
        /*if(adcVolts < 2500) {
            LED_Write(1u);
        } else {
            LED_Write(0u);
        }*/
        //traverseMap(map);
        /*rotationAntiClockwise();
        rotationClockwise();
        
        if(timerInt == 1) {
            // calculate RPM of M2
            quadCountToRPM(quadDec2Count);
        }*/
        
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
