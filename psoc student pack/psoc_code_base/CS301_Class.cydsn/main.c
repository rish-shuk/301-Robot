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
//#include "pathfinding.h"
#include "initialise.h"
#include "movement.h"
#include "usbUART.h"

//* ========================================
int16 quadCountToRPM(uint16 count);
//* ========================================
// Sensors, Course correction and Movement Direction.
void ResetSensorFlags();
void SetRobotMovement();
enum DirectionState CheckSensorDirection();
enum DirectionState {Forward, TurnRight, TurnLeft, StopToTurnRight, StopToTurnLeft, Stop, Unknown};
enum DirectionState currentDirection = Stop;
// ----------------------------------------
uint8 s1 = 0; // black = 0, white = 1
uint8 s2 = 0;
uint8 s3 = 0;
uint8 s4 = 0;
uint8 s5 = 0;
uint8 s6 = 0;
//* ========================================
// Calculating Distance
#define WHEEL_DIAMETER_MM 64
uint32 totalMilliseconds = 0;
int totalDistance = 0; // in mm
//* ========================================
char buffer[69];
int quadDec2Count = 0;
int timerInt = 0;
int keepLedOn = 0;


//char map[MAX_ROWS][MAX_COLS]; // global map array- stores the map

CY_ISR (speedTimer) {
    timerInt = 1;
    //quadDec_M1 used for turning macros
    quadDec2Count = QuadDec_M2_GetCounter();
    QuadDec_M2_SetCounter(0); // reset count
    QuadDec_M2_Start(); // restart counter
    SpeedTimer_ReadStatusRegister(); // clear interrupt
}

CY_ISR(S1_DETECTED) {
    // Sensor has detected WHITE
    s1 = 1; // , Black = 0, White = 1
    LED_Write(1u);
    //moveForward();
}

CY_ISR(S2_DETECTED) {
    // Sensor has detected WHITE
    s2 = 1; // , Black = 0, White = 1
    LED_Write(1u);
    //moveForward();
}

CY_ISR(S3_DETECTED) {
    // Sensor has detected WHITE
    s3 = 1; // , Black = 0, White = 1
    LED_Write(1u);
}

CY_ISR(S4_DETECTED) {
    // Sensor has detected WHITE
    s4 = 1; // , Black = 0, White = 1
    //LED_Write(1u);
}

CY_ISR(S5_DETECTED) {
    // Sensor has detected WHITE
    s5 = 1; // , Black = 0, White = 1
    //LED_Write(1u);
}

CY_ISR(S6_DETECTED) {
    // Sensor has detected WHITE
    s6 = 1; // , Black = 0, White = 1
    //LED_Write(1u);
}

CY_ISR(TIMER_FINISH) {
    // Check flags, set LEDs to high
    if(s2 || s3 || s4 || s5 || s6) {
        //LED_Write(1u);
    } else {
        LED_Write(0u);
    }
    // FOR CALCULATING DISTANCE
    // Since this timer is every 9.7ms we calculate distance travelled the last 9.7ms
    // We don't calculate if we are currently turning
    // total distance = total distance + ( RPM * PI * DIAMETER * (TIME SINCE LAST CALCULATATION) )
    if (currentDirection == Forward) {
        totalDistance = totalDistance + abs(quadCountToRPM(quadDec2Count)) * CY_M_PI * WHEEL_DIAMETER_MM * 9.7;
    }
    
    // Reset Sensor Flags for Next rising Eddge
    // (s1 = 0, s2 = 0... etc.)
    SetRobotMovement();
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
    S2_detected_StartEx(S2_DETECTED);
    S3_detected_StartEx(S3_DETECTED);
    S4_detected_StartEx(S4_DETECTED);
    S5_detected_StartEx(S5_DETECTED);
    S6_detected_StartEx(S6_DETECTED);
    Timer_LED_Start();
    stopMoving();
    
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
        //rotationAntiClockwise();
        //rotationClockwise();
        //stopMoving();
        //moveForward();
        //ResetSensorFlags();
        //SetRobotMovement();
        
        
        
        if(timerInt == 1) {
            // calculate RPM of M2
            quadCountToRPM(quadDec2Count);
            //sprintf(buffer, "%d", currentDirection);
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
int16 quadCountToRPM(uint16 count)
{
    float cps = count/57.00;
    int16 rpm = (int16)(cps*15); // rpm value
    sprintf(buffer, "%d", rpm); // store in buffer
    return rpm;
    //usbPutString(buffer);
    //usbPutString("rpm ");
}

// Resets all sensor flags to 0 - i.e. currently out of map
void ResetSensorFlags() {
    s1 = 0;
    s2 = 0;
    s3 = 0;
    s4 = 0;
    s5 = 0;
    s6 = 0;
}

// This function checks the sensor flags s1-s6 through a boolean truth table and
// returns a enum direction state depending on the flag configuration
// if no conditons are met, it returns Unknown -- need to fix this edge case
// s1 = 0 -- Black
// s1 = 1 -- White
enum DirectionState CheckSensorDirection() {
    enum DirectionState directionState = Stop;
    //directionState = Unknown;
    
    // go forward if front two sensors are on black
    // S1 and S2 are on black
    //if (!s1 && !s2) {
    //    directionState = Forward;
    //    return directionState;
    //}
    
    /*
    //stop to turn left
    // S1 and S2 are on white
    // S3 and S4 are on black
    // S5 is on black, S6 is on white
    if (s1 && s2 && !s3 && !s4 && !s5 && s6) {
        directionState = StopToTurnLeft;
        return directionState;   
    }
    
    //stop to turn right
    if (s1 && s2 && !s3 && !s4 && s5 && !s6) {
        directionState = StopToTurnRight;
        return directionState;   
    }
    
    //forward if all sensors are on white
    
    if (s1 && s2 && s3 && s4 & s5 && s6) {
        directionState = Forward;
        return directionState;   
    }

    //turn left
    if (!s5 && s6) {
        directionState = TurnLeft;
        return directionState;
    }
    
    //turn right
    if (s5 && !s6) {
        directionState = TurnRight;
        return directionState;
    }
    */
    // ============================ TESTING
    if (s1) {
        directionState = Forward;
        return directionState;   
    }
    
    //turn left
    if (s2) {
        directionState = TurnLeft;
        return directionState;
    }
    
    //turn right
    if (s3) {
        directionState = TurnRight;
        return directionState;
    }
    // ============================ TESTING
    // If the code gets up to this point then no conditions have been met
    // The sensors are in a configuration that has not been covered
    // The currentDirection to turn into is unknown.
    return directionState;
}

// Sets robot movement direction state according to currentDirection which is set by Check
void SetRobotMovement() {
    currentDirection = CheckSensorDirection();   
    
    switch (currentDirection) {
        //Forward, TurnRight, TurnLeft, StopToTurnRight, StopToTurnLeft, Stop, Unknown
        case Forward:
            //LED_Write(1u);
            moveForward();
            break;
        case TurnRight:
            rotationClockwise();
            break;
        case TurnLeft:
            rotationAntiClockwise();
            break;
        case StopToTurnRight:
            //stopMoving();
            rotationClockwise();
            // need to move forward for a specifed amount of time so that left sensors dont activate old path
            break;
        case StopToTurnLeft:
            //stopMoving();
            rotationAntiClockwise();
            break;
        case Stop:
            //stopMoving();
            stopMoving();
            break;
        case Unknown:
            // UNKNOWN CONFIGURATION
            stopMoving();
            break;  
    }
}


