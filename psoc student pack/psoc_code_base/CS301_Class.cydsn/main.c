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
// USBUART
void usbPutString(char *s);
void usbPutChar(char c);
void handle_usb();
//* ========================================
int16 quadCountToRPM(uint16 count);
//* ========================================
// Sensors, Course correction and Movement Direction.
void ResetSensorFlags();
void SetRobotMovement();
enum DirectionState CheckSensorDirection();
enum DirectionState {Forward, TurnRight, TurnLeft, AdjustToTheLeft, AdjustToTheRight, Stop, Unknown};
enum DirectionState currentDirection = Stop;
enum DirectionState previousDirection = Unknown;
// --- YIPPE
// ----------------------------------------
uint8 s1 = 0; // black = 0, white = 1
uint8 s2 = 0;
uint8 s3 = 0;
uint8 s4 = 0;
uint8 s5 = 0;
uint8 s6 = 0;
//* ========================================
// Calculating Distance
#define WHEEL_DIAMETER_MM 65
uint32 totalMilliseconds = 0;
uint32 totalDistance = 0; // in mm
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
    
    if (currentDirection == Forward && quadDec2Count != 0) {
        totalDistance = totalDistance + (abs(quadDec2Count) / 57.0) * CY_M_PI * WHEEL_DIAMETER_MM;
    }
    
    QuadDec_M2_SetCounter(0); // reset count
    QuadDec_M2_Start(); // restart counter
    SpeedTimer_ReadStatusRegister(); // clear interrupt
}

CY_ISR(S1_DETECTED) {
    // Sensor has detected WHITE
    s1 = 1; // , Black = 0, White = 1
    //LED_Write(1u);
    //moveForward();
}

CY_ISR(S2_DETECTED) {
    // Sensor has detected WHITE
    s2 = 1; // , Black = 0, White = 1
    //LED_Write(1u);
    //moveForward();
}

CY_ISR(S3_DETECTED) {
    // Sensor has detected WHITE
    s3 = 1; // , Black = 0, White = 1
    //LED_Write(1u);
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

CY_ISR(SIGNAL_TIMER_FINISH) {
    // Reset Sensor Flags for Next rising Eddge
    // (s1 = 0, s2 = 0... etc.)
    //LED_Write(0u);
    

    Timer_LED_ReadStatusRegister();
}


int main()
{
// --------------------------------    
// ----- INITIALIZATIONS ----------
    CYGlobalIntEnable;
    init(); // initialise clocks, pwms, adc, dac etc- done in header file
    //findPath(map, "");// find shortest path- store this in map
    //isr_speed_StartEx(speedTimer); // start interrupt
    isr_Timer_LED_StartEx(SIGNAL_TIMER_FINISH);
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
    RF_BT_SELECT_Write(0);
    
    //usbPutString("Initialised UART");
    for(;;)
    {   
        SetRobotMovement();
        ResetSensorFlags();
        //traverseMap(map);
        //rotationAntiClockwise();
        //rotationClockwise();
        
        if(timerInt == 1) {
            timerInt = 0;
            // calculate RPM of M2
            quadCountToRPM(quadDec2Count);
            //sprintf(buffer, "%lu", totalDistance);
            //usbPutString(buffer);
            //usbPutString(" ");
        }
        
        if (flag_KB_string == 1)
        {
            //usbPutString("Total Distance: ");
            //sprintf(buffer, "%lu", totalDistance);
            //usbPutString(buffer);
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
    //sprintf(buffer, "%d", rpm); // store in buffer
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
    previousDirection = currentDirection;
    
    // all white
    if (!(s1 && s2 && s3 && s4 && s5 && s6)) {
        stopMoving();
    }
    
    //forward if 111100
    if (s1 && s2 && s3 && s4 && !s5 && !s6) {
        directionState = Forward;
        return directionState;   
    }
    
    /* COURSE CORRECTION COURSE CORRECTION COURSE CORRECTION */
    // Only need to course correct when direction state is forward
    
    if (previousDirection == Forward) {
        // If robot is deviating to the left where top left or bottom right is on white
        if (s5 || !s1) {
            directionState = AdjustToTheRight;
            return directionState;
        }
        
        // If robot is deviating to the right where top right sensor or bottom left on white
        if (s6 || !s2) {
            directionState = AdjustToTheLeft;
            return directionState;
        }
    }
    /* COURSE CORRECTION COURSE CORRECTION COURSE CORRECTION */
    
    // Left sensor is on black and right sensor is on white
    //turn left if 110100
    if (s1 && s2 && !s3 && s4 && !s5 && !s6) {
        directionState = TurnLeft;
        return directionState;
    }
    
    // Right sensor is on white and right sensor is on black
    // everything else is on white
    //turn right if 110100
    if (s1 && s2 && !s3 && s4 && !s5 && !s6) {
        directionState = TurnRight;
        return directionState;
    }
    
    // if all sensors are on black -- we are currently in darkness so don't move
    // stop 000000
    if (!(s1 && s2 && s3 && s4&& s5 && s6)) {
        directionState = Stop;
        return directionState;
    }
    
    // If the code gets up to this point then no conditions have been met
    // The sensors are in a configuration that has not been covered
    // The currentDirection to turn into is unknown.
    
    // if currentDirection is Unknown, we continue with the previous direction
    // However, if the previous direction is also Unknown, we will just move forward.
    if (previousDirection == Unknown) {
        directionState = Forward;
        return directionState;
    }
            
    return previousDirection;
}

// Sets robot movement direction state according to currentDirection which is set by Check
void SetRobotMovement() {
    currentDirection = CheckSensorDirection();   
    
    switch (currentDirection) {
        //Forward, TurnRight, TurnLeft, AdjustToTheRight, AdjustToTheLeft, Stop, Unknown
        case Forward:
            moveForward();
            break;
        case TurnRight:
            // turn until 11000
            while(!(s1 && s2 && !s3 && s4 && !s5 && !s6)) {
                rotationAntiClockwise();
            }
            break;
        case TurnLeft:
            // turn until XX0X00
            while(!(s1 && s2 && !s3 && !s5 && !s6)) {
                rotationAntiClockwise();
            }
            break;
        case AdjustToTheRight: // rotate clockwise until s5 is on black and forward configuration
            // rotate until 111100
            while(!(s1 && s2 && s3 && s4 && !s5 && !s6)) {
                rotationClockwise();
            }
            break;
        case AdjustToTheLeft: // rotate anticlockwise until s6 is on black
            // rotate until 111100
            while(!(s1 && s2 && s3 && s4 && !s6 && !s5)) {
                rotationAntiClockwise();
            }
            break;
        case Stop:
            stopMoving();
            break;
        case Unknown:
            // UNKNOWN CONFIGURATION
            break;  
    }
}

