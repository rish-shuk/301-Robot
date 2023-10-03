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
#include "map.h"
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
enum DirectionState {Forward, TurnRight, TurnLeft, AdjustToTheLeft, AdjustToTheRight, Stop, Unknown, waitForTurn, waitForRightTurn, waitForLeftTurn, ForwardAfterTurn, Backward};
enum OrientationState {Up, Down, Left, Right};
enum DirectionState currentDirection = Stop;
enum DirectionState previousDirection = Unknown;
enum OrientationState currentOrientation = Right;
enum OrientationState previousOrientation = Right;
enum DirectionState GetNextStep();
// --- YIPPE
// ----------------------------------------
uint8 s3 = 0;
uint8 s4 = 0;
uint8 s5 = 0;
uint8 s6 = 0;
//* ========================================
// Calculating Distance
#define WHEEL_DIAMETER_MM 64.5

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
// ----- INITIALIZATIONS ----------
    CYGlobalIntEnable;
    ResetSensorFlags();
    init(); // initialise clocks, pwms, adc, dac etc- done in header file
    isr_speed_StartEx(speedTimer); // start interrupt
    isr_Timer_LED_StartEx(TIMER_FINISH);
    S3_detected_StartEx(S3_DETECTED);
    S4_detected_StartEx(S4_DETECTED);
    S5_detected_StartEx(S5_DETECTED);
    S6_detected_StartEx(S6_DETECTED);
    Timer_LED_Start();

    findPath(map);// find shortest path- store this in map wasn't being called before

// ------USB SETUP ----------------    
#ifdef USE_USB    
    USBUART_Start(0,USBUART_5V_OPERATION);
#endif        
    RF_BT_SELECT_Write(0);
    
    //usbPutString("Initialised UART");
    for(;;)
    {
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
    s3 = 0;
    s4 = 0;
    s5 = 0;
    s6 = 0;
}

float yBlocksize = 127.5;
float xBlocksize = 92.5;
uint8 currentRow = 1;
uint8 currentCol = 1;

enum DirectionState GetNextStep() {
    enum DirectionState directionState;
    // Determines robot movement and orientation to follow optimal path
    //enum DirectionState directionState = Stop;
    if((currentRow == 1) && (currentCol == 5)) {
        directionState = Stop; 
        return directionState;
    }
    switch (previousOrientation) {
            case Up:
                if(map[currentRow - 1][currentCol] == 8) {
                    currentOrientation = Up; 
                    directionState = Forward;
                    currentRow--;// update position
                } else if (map[currentRow][currentCol - 1] == 8) {
                    currentOrientation = Left;
                    directionState = TurnLeft;
                    //currentRow--;
                } else if (map[currentRow][currentCol + 1] == 8) {
                    currentOrientation = Right;
                    directionState = TurnRight;
                    //currentRow++; // update position
                }
                break;
            case Down:
                if(map[currentRow + 1][currentCol] == 8) {
                    currentOrientation = Down;
                    directionState = Forward;
                    currentRow++;
                } else if (map[currentRow][currentCol - 1] == 8) {
                    currentOrientation = Right;
                    directionState = TurnRight;
                    //currentRow--;
                } else if (map[currentRow][currentCol + 1] == 8) {
                    currentOrientation = Left;
                    //directionState = TurnLeft;
                    //currentRow++; // update position
                }
                break;
            case Left:
                if(map[currentRow][currentCol - 1] == 8) {
                    currentOrientation = Left;
                    directionState = Forward;
                    currentCol--; // update position
                } else if (map[currentRow - 1][currentCol] == 8) {
                    currentOrientation = Up;
                    directionState = TurnRight;
                    //currentRow++; // update position
                } else if (map[currentRow + 1][currentCol] == 8) {
                    currentOrientation = Down;
                    directionState = TurnLeft;
                    //currentCol--; // update position
                }
                break;
            case Right:
                if(map[currentRow][currentCol + 1] == 8) {
                    currentOrientation = Right;
                    directionState = Forward;
                    currentCol++; // update position
                } else if (map[currentRow - 1][currentCol] == 8) {
                    currentOrientation = Up;
                    directionState = waitForLeftTurn;
                    //currentCol++; // update position
                } else if (map[currentRow + 1][currentCol] == 8) {
                    currentOrientation = Down;
                    directionState = waitForRightTurn;
                    //currentCol--; // update position
                }
                break;
            default:
                break;
        }
    return directionState;
}

enum DirectionState CheckSensorDirection() {
    float blocksize;
    if(currentOrientation == Up || currentOrientation == Down) {
        blocksize = 128.40;
    } else {
        blocksize = 92.50;
    }
    enum DirectionState directionState = Stop; // initialise state as stop
    previousDirection = currentDirection; // store currentDirection as previousDirection for next pass
    
    if (totalDistance >= blocksize) {
        directionState = GetNextStep(); // get next step at each block
        totalDistance = 0; // reset distance
        return directionState;
    }
    
    // STOP BUFFER * ========================================
    if (previousDirection == Stop) {
        if (stopBuffer <= 10) {
            directionState = Stop; // stop buffer- prevents overturning
        } else {
            directionState = ForwardAfterTurn;
        }
        return directionState;
    }

    // TURNING- will only turn due to getNextStep() * ========================================    
    if (previousDirection == ForwardAfterTurn) {
        if (s3 || s4) {
            directionState = Forward; // turns when robot has rotated 90º
            return directionState;
        }
    }

    if(previousDirection == waitForRightTurn) {
        if(s4) {
            directionState = TurnRight;
            return directionState;
        } else {
            directionState = waitForRightTurn;
            return directionState;
        }
    }

    if(previousDirection == waitForLeftTurn) {
        if(s3) {
            directionState = TurnLeft;
            return directionState;
        } else {
            directionState = waitForLeftTurn;
            return directionState;
        }
    }

    if(previousDirection == TurnRight) {
        if(s5 && s6) {
            directionState = TurnRight; // keep turning while s5 & s6 are high
            return directionState;
        } 
        else if (!s5 || !s6) {
            directionState = Stop; // stop turning when s5 & s6 are low
            totalDistance = 0; // correct totalDistance
            return directionState;
        }
    }    

    if(previousDirection == TurnLeft) {
        if(s5 && s6) {
            directionState = TurnLeft; // keep turning while s5 & s6 are high
            return directionState;
        } 
        else if (!s5 || !s6) {
            directionState = Stop; // stop turning when s5 & s6 are low
            totalDistance = 0; // correct totalDistance
            return directionState;
        }
    }
    
    // COURSE CORRECTION * ========================================
    if (previousDirection == Forward || previousDirection == AdjustToTheLeft || previousDirection == AdjustToTheRight) {
        if(s6) {
            directionState = AdjustToTheLeft; // keep adjusting to the left
            return directionState;
        }
        if(s5) {
            directionState = AdjustToTheRight; // keep adjusting to the right
            return directionState;
        }
    }
    
    // FORWARD * ========================================
    if (s3 && s4 && !s5 && !s6) {
        directionState = Forward;
        return directionState;   
    }
    // SENSORS ALL HIGH CONDITION- waiting for a turn * ========================================
    if(s5 && s6 && (previousDirection != waitForTurn)) {
        directionState = waitForTurn; // need to keep going forward until s3 || s4 are low before turning
        return directionState;
    }

    if (previousDirection == waitForTurn && (s3 || s4)) {
        directionState = GetNextStep(); // determine which way to turn
        return directionState;
    }

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
        case waitForTurn:
            moveForward(); 
            break;
        case ForwardAfterTurn:
            moveForward();
            break;
        case Backward:
            moveBackward();
            break;
        case waitForLeftTurn:
            moveForward();
            break;
        case waitForRightTurn:
            moveForward();
            break;
        case Unknown:
            // UNKNOWN CONFIGURATION
            break;  
    }
}

