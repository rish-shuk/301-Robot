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
enum DirectionState {Forward, TurnRight, TurnLeft, AdjustToTheLeft, AdjustToTheRight, Stop, Unknown, HardForward, waitForTurn, ForwardAfterTurn, Backward};
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
#define WHEEL_DIAMETER_MM 64.5
#define STOPPING_DISTANCE 10000 // in MM
uint32 totalMilliseconds = 0;
float totalDistance = 0; // in mm
//* ========================================
char buffer[69];
int quadDec2Count = 0;
int timerInt = 0;
int keepLedOn = 0;
uint32 stopBuffer = 0;

//char map[MAX_ROWS][MAX_COLS]; // global map array- stores the map

CY_ISR (speedTimer) {
    timerInt = 1;
    //quadDec_M1 used for turning macros
    quadDec2Count = QuadDec_M2_GetCounter();
    
    if ((currentDirection == Forward || 
        currentDirection == HardForward ||
        currentDirection == waitForTurn || 
        currentDirection == ForwardAfterTurn ||
        currentDirection == AdjustToTheLeft ||
        currentDirection == AdjustToTheRight) &&
        quadDec2Count != 0) {
        //uint32 newDistance = ((abs(quadDec2Count) / 57.0) * CY_M_PI * WHEEL_DIAMETER_MM)/4;
        float newDistance = (abs(quadDec2Count) * CY_M_PI * WHEEL_DIAMETER_MM)/228;
        
        totalDistance = totalDistance + newDistance;
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

CY_ISR(TIMER_FINISH) {
    //LED_Write(0u);
    if (currentDirection == Stop) {
        stopBuffer = stopBuffer + 1;
    } else {
        stopBuffer = 0;
    }
    SetRobotMovement(); 
    ResetSensorFlags(); // Reset Sensor Flags for Next rising Eddge
    Timer_LED_ReadStatusRegister();
}


int main()
{
// --------------------------------    
// ----- INITIALIZATIONS ----------
    CYGlobalIntEnable;
    ResetSensorFlags();
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
    //stopMoving();

// ------USB SETUP ----------------    
#ifdef USE_USB    
    USBUART_Start(0,USBUART_5V_OPERATION);
#endif        
    RF_BT_SELECT_Write(0);
    
    //usbPutString("Initialised UART");
    for(;;)
    {
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
        //handle_usb();
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
    
    if (totalDistance >= STOPPING_DISTANCE) {
        directionState = Stop;
        return directionState;
    }
   
    if (previousDirection == Stop) {
        if (stopBuffer <= 10) {
            directionState = Stop;
        } else {
            directionState = ForwardAfterTurn;
        }
        return directionState;
    }
    
    if (previousDirection == ForwardAfterTurn) {
        if (s3 || s4) {
            directionState = Forward;
            return directionState;
        }
    }
    

    if(previousDirection == TurnRight) {
        if(s5 && s6) {
            directionState = TurnRight;
            return directionState;
        } 
        else if (!s5 || !s6) {
            directionState = Stop;
            return directionState;
        }
    }    

    if(previousDirection == TurnLeft) {
        if(s5 && s6) {
            directionState = TurnLeft;
            return directionState;
        } 
        else if (!s5 || !s6) {
            directionState = Stop;
            return directionState;
        }
    }
    
    if (previousDirection == waitForTurn) {
        // If we are waiting for a turn, look for left sensor and right sensor, otherwise keep waiting for turn (moving forward)
        if (!s3) {
            directionState = TurnLeft;
            return directionState;
        }
        if (!s4) {
            directionState = TurnRight;   
            return directionState;
        }
        directionState = waitForTurn;
        return directionState;
    }
    


    // wait for turn at end of line
    if(s5 && s6 && (previousDirection == Forward || (previousDirection == AdjustToTheLeft || previousDirection == AdjustToTheRight))) {
        directionState = waitForTurn; // need to wait to check for a black line
        return directionState;
    }
    
    // course correction
    if (previousDirection == Forward || previousDirection == AdjustToTheLeft || previousDirection == AdjustToTheRight) {
        
        /*
        // If both are on white, we assume we are off the line and use the back two sensors as backup c.c
        if (s5 && s6) {
            
            if (s1 && s2) {
                directionState = Forward;
                return directionState;
            }
            if (!s1) {
                directionState = AdjustToTheRight;
                return directionState;
            }
            if (!s2) {
                directionState = AdjustToTheLeft;
                return directionState;
            }
            
            directionState = Forward;
            return directionState;
        }
            */
        
        
        if(s6) {
            directionState = AdjustToTheLeft; // keep adjusting to the left
            return directionState;
        }
        if(s5) {
            directionState = AdjustToTheRight; // keep adjusting to the right
            return directionState;
        }
    }
    
    // forward 111100
    if (s3 && s4 && !s5 && !s6) {
        directionState = Forward;
        return directionState;   
    }
    
    //turn left 110111
    if (!s3 && s4 && s5 && s6) {
        directionState = TurnLeft;
        return directionState;
    }
    
    // turn right if 111011
    if (s3 && !s4 && s5 && s6) {
        directionState = TurnRight;
        return directionState;
    }
    
    
    // ====== After Initial turn ======
    // -- This accounts for the transition period between turning at an intersection --
    /*if (previousDirection == TurnRight || previousDirection == TurnLeft) {
        if ((s1 && s2 && s3 && !s4 && !s5 && !s6) ||
        (s1 && s2 && !s3 && s4 && !s5 && !s6)) {
            directionState = HardForward;
            return directionState;
        }
    }*/
    
    

    // If currentDirection is Unknown, we continue with the previous direction.
    // However, if the previous direction is also Unknown, we will just move forward.
    if (previousDirection == Unknown) {
        directionState = Forward;
        return directionState;
    }
    
    // Possible reason
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
            turnRight();
            break;
        case TurnLeft:
            turnLeft(); // try course correction methods for L/R turn
            break;
        case AdjustToTheRight:
            adjustRight();
            break;
        case AdjustToTheLeft:
            adjustLeft();
            break;
        case Stop:
            stopMoving();
            break;
        case HardForward:
            break;
        case waitForTurn:
            moveForward(); 
            break;
        case ForwardAfterTurn:
            moveForward();
            break;
        case Backward:
            moveBackward();
            break;
        case Unknown:
            // UNKNOWN CONFIGURATION
            break;  
    }
}
