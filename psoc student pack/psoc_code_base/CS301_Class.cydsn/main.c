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
// Sensors and Booleans
void ResetSensorFlags();
enum DirectionState {Forward, TurnRight, TurnLeft, StopToTurnRight, StopToTurnLeft, Stop, Unknown};
//* ========================================
char buffer[69];
int quadDec2Count = 0;
int timerInt = 0;
int keepLedOn = 0;
uint8 s1 = 0; // black = 0, white = 1
uint8 s2 = 0;
uint8 s3 = 0;
uint8 s4 = 0;
uint8 s5 = 0;
uint8 s6 = 0;
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

CY_ISR(S1_DETECTED) {
    // Sensor has detected WHITE
    s1 = 1; // , Black = 0, White = 1
}

CY_ISR(S2_DETECTED) {
    // Sensor has detected WHITE
    s2 = 1; // , Black = 0, White = 1
}

CY_ISR(S3_DETECTED) {
    // Sensor has detected WHITE
    s3 = 1; // , Black = 0, White = 1
}

CY_ISR(S4_DETECTED) {
    // Sensor has detected WHITE
    s4 = 1; // , Black = 0, White = 1
}

CY_ISR(S5_DETECTED) {
    // Sensor has detected WHITE
    s5 = 1; // , Black = 0, White = 1
}

CY_ISR(S6_DETECTED) {
    // Sensor has detected WHITE
    s6 = 1; // , Black = 0, White = 1
}

CY_ISR(TIMER_FINISH) {
    // Check flags, set LEDs to high
    if(s1 == 1) {
        LED_Write(1u);
    } else {
        LED_Write(0u);
    }
    
    // Reset Sensor Flags for Next rising Eddge
    // (s1 = 0, s2 = 0... etc.)
    ResetSensorFlags();
    Timer_LED_ReadStatusRegister();
}

int main()
{
// --------------------------------    
// ----- INITIALIZATIONS ----------
    CYGlobalIntEnable;
    init(); // initialise clocks, pwms, adc, dac etc- done in header file
    //findPath(map, "");// find shortest path- store this in map
    isr_speed_StartEx(speedTimer); // start interrupt
    isr_Timer_LED_StartEx(TIMER_FINISH);
    S1_detected_StartEx(S1_DETECTED);
    Timer_LED_Start();
    
    
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
        //traverseMap(map);
        rotationAntiClockwise();
        rotationClockwise();
        
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

// Resets all sensor flags to 0
void ResetSensorFlags() {
    s1 = 0;
    s2 = 0;
    s3 = 0;
    s4 = 0;
    s5 = 0;
}

// This function checks the sensor flags s1-s6 through a boolean truth table and
// returns a enum direction state depending on the flag configuration
// if no conditons are met, it returns Unknown -- need to fix this edge case
// s1 = 0 -- Black
// s1 = 1 -- White
enum DirectionState CheckTableDirection() {
    enum DirectionState directionState = Unknown;
    directionState = Unknown;
    
    //stop to turn left
    if (s1 && s2 && !s3 && !s4 && !s5 && s6) {
        directionState = StopToTurnLeft;
        return directionState;   
    }
    
    //stop to turn right
    if (s1 && s2 && !s3 && !s4 && s5 && !s6) {
        directionState = StopToTurnRight;
        return directionState;   
    }
    
    //forward
    if (!s1 && !s2 && !s3 && !s4 && s5 && s6) {
        directionState = Forward;
        return directionState;   
    }
    
    //turn left
    if (s5 && !s6) {
        directionState = TurnLeft;
        return directionState;
    }
    
    //turn right
    if (!s5 && s6) {
        directionState = TurnRight;
        return directionState;
    }
    
    // If the code gets up to this point then no conditions have been met
    // The sensors are in a configuration that has not been covered
    // The currentDirection to turn into is unknown.
    return directionState;
}

