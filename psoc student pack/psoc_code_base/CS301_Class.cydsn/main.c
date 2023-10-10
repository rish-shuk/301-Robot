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
enum RobotMovement {Forward, TurnRight, TurnLeft, AdjustToTheLeft, AdjustToTheRight, Stop, Unknown, waitForTurn, ForwardAfterTurn, Backward};
enum RobotMovement currentDirection, previousDirection = Forward; 
enum OrientationState currentOrientation, previousOrientation = Right;
Instruction currentInstruction;
int numSteps;
void traversePath(int numSteps, Instruction instructionList[]);
Instruction * instructionList;
// ----------------------------------------
uint8 s3, s4, s5, s6 = 0;
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
        currentDirection == waitForLeftTurn ||
        currentDirection == waitForRightTurn ||
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
    instructionList = findPath(map, food_list);
    numSteps = instructionsListLength();
    
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

float xBlocksize = 127.5; // 127.5
float yBlocksize = 80; // 80
uint8 currentRow = 1;
uint8 currentCol = 1;

/*enum DirectionState GetNextStep() {
    enum DirectionState directionState;
    // Determines robot movement and orientation to follow optimal path
    // enum DirectionState directionState = Stop;
    previousOrientation = currentOrientation;
    
    if(map[currentRow][currentCol] == 9) {
        directionState = Stop; // reached target
        return directionState;
    }
        
    switch (previousOrientation) {
            case Up:
                if(map[currentRow - 1][currentCol] == 8 || map[currentRow - 1][currentCol] == 9) {
                    currentOrientation = Up; 
                    directionState = Forward;
                    currentRow--;// update position
                } else if (map[currentRow][currentCol - 1] == 8 || map[currentRow][currentCol - 1] == 9) {
                    currentOrientation = Left;
                    directionState = waitForLeftTurn;
                    //currentRow--;
                } else if (map[currentRow][currentCol + 1] == 8 || map[currentRow][currentCol + 1] == 9) {
                    currentOrientation = Right;
                    directionState = waitForRightTurn;
                    //currentRow++; // update position
                }
                break;
            case Down:
                if(map[currentRow + 1][currentCol] == 8 || map[currentRow + 1][currentCol] == 9) {
                    currentOrientation = Down;
                    directionState = Forward;
                    currentRow++;
                } else if (map[currentRow][currentCol - 1] == 8 || map[currentRow][currentCol - 1] == 9) {
                    currentOrientation = Left;
                    directionState = waitForRightTurn;
                } else if (map[currentRow][currentCol + 1] == 8 || map[currentRow][currentCol + 1] == 9) {
                    currentOrientation = Right;
                    directionState = waitForLeftTurn;
                }
                break;
            case Left:
                if(map[currentRow][currentCol - 1] == 8 || map[currentRow][currentCol - 1] == 9) {
                    currentOrientation = Left;
                    directionState = Forward;
                    currentCol--; // update position
                } else if (map[currentRow - 1][currentCol] == 8 || map[currentRow - 1][currentCol] == 9) {
                    currentOrientation = Up;
                    directionState = waitForRightTurn;
                } else if (map[currentRow + 1][currentCol] == 8 || map[currentRow + 1][currentCol] == 9) {
                    currentOrientation = Down;
                    directionState = waitForLeftTurn;
                }
                break;
            case Right:
                if(map[currentRow][currentCol + 1] == 8 || map[currentRow][currentCol + 1] == 9) {
                    currentOrientation = Right;
                    directionState = Forward;
                    currentCol++; // update position
                } else if (map[currentRow - 1][currentCol] == 8 || map[currentRow - 1][currentCol] == 9) {
                    currentOrientation = Up;
                    directionState = waitForLeftTurn;
                    //currentCol++; // update position
                } else if (map[currentRow + 1][currentCol] == 8 || map[currentRow + 1][currentCol] == 9) {
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
*/
uint8 stoppedAfterTurn = 0;
uint8 ignoreSensor = 0;
// needs to make sure robot is going in the correct direction (supplied from instruction)
enum RobotMovement CheckSensorDirection() {
    float blocksize;
    if(currentOrientation == Up || currentOrientation == Down) {
        blocksize = yBlocksize;
    } else {
        blocksize = xBlocksize;
    }
    //if(currentInstruction)
    enum RobotMovement directionState = Stop; // initialise state as stop
    
    if (stoppedAfterTurn == 1) {
        if (stopBuffer <= 50) {
            directionState = Stop; // stop buffer- prevents overturning
            previousDirection = directionState;
            return directionState;
        }
        directionState = GetNextStep(); // get next step at each block
        totalDistance = 0; // reset distance
        previousDirection = directionState;
        stoppedAfterTurn = 0;
        return directionState;
    }
    if (totalDistance >= blocksize) {
        directionState = GetNextStep(); // get next step at each block
        totalDistance = 0; // reset distance
        previousDirection = directionState;
        return directionState;
    }
    
    // STOP BUFFER * ========================================
    if (previousDirection == Stop) {
        if (stopBuffer <= 50) {
            directionState = Stop; // stop buffer- prevents overturning
            previousDirection = directionState;
        } else {
            //directionState = ForwardAfterTurn;
        }
        return directionState;
    }

    // TURNING * ========================================    
    if (previousDirection == ForwardAfterTurn) {
        if (s3 || s4) {
            //ignoreSensor = 0;
            //usbPutString("Forward\n");
            directionState = Forward; // turns when robot has rotated 90º
            previousDirection = directionState;
            return directionState;
        }
    }

    if(previousDirection == waitForRightTurn) {
        //ignoreSensor = 0;
        if(!s4) {
            //usbPutString("Turn Right\n");
            directionState = TurnRight;
            previousDirection = directionState;
            return directionState;
        } else {
            //usbPutString("Wait for Right Turn\n");
            directionState = waitForRightTurn;
            previousDirection = directionState;
            return directionState;
        }
    }

    if(previousDirection == waitForLeftTurn) {
        //ignoreSensor = 0;
        if(!s3) {
            //usbPutString("Turn Left\n");
            directionState = TurnLeft;
            previousDirection = directionState;
            return directionState;
        } else {
            //usbPutString("Wait for Left Turn\n");
            directionState = waitForLeftTurn;
            previousDirection = directionState;
            return directionState;
        }
    }

    if(previousDirection == TurnRight) {
        if(s5 && s6) {
            //usbPutString("Turn Right\n");
            directionState = TurnRight; // keep turning while s5 & s6 are high
            previousDirection = directionState;
            return directionState;
        } 
        else if (!s5 || !s6) {
            //ignoreSensor = 1; // ignore turn check after turn completed
            //usbPutString("Stop after Right Turn");
            directionState = Stop; // stop turning when s5 & s6 are low
            totalDistance = 0; // correct/ RESET totalDistance
            previousDirection = directionState;
            stoppedAfterTurn = 1; // set flag- differentiate from stop at targetLocation
            return directionState;
        }
    }    

    if(previousDirection == TurnLeft) {
        if(s5 && s6) {
            //usbPutString("Turn Left\n");
            directionState = TurnLeft; // keep turning while s5 & s6 are high
            previousDirection = directionState;
            return directionState;
        } 
        else if (!s5 || !s6) {
            //ignoreSensor = 1; // ignore sensor after turn
           // usbPutString("Stop after Left Turn\n");
            directionState = Stop; // stop turning when s5 & s6 are low
            totalDistance = 0; // correct/ RESET totalDistance
            previousDirection = directionState;
            stoppedAfterTurn = 1; // set flag- differentiate from stop at targetLocation
            return directionState;
        }
    }
    
    // COURSE CORRECTION * ========================================
    if (previousDirection == Forward || previousDirection == AdjustToTheLeft || previousDirection == AdjustToTheRight) {
        //ignoreSensor = 0;
        if(s6) {
            //usbPutString("Adjust to the left\n");
            directionState = AdjustToTheLeft; // keep adjusting to the left
            previousDirection = directionState;
            return directionState;
        }
        if(s5) {
            //usbPutString("Adjust to the right\n");
            directionState = AdjustToTheRight; // keep adjusting to the right
            previousDirection = directionState;
            return directionState;
        }
    }
    
    // FORWARD * ========================================
    if (s3 && s4 && !s5 && !s6) {
        //usbPutString("Forward\n");
        directionState = Forward;
        previousDirection = directionState;
        return directionState;   
    }

    // If currentDirection is Unknown, we continue with the previous direction.
    // However, if the previous direction is also Unknown, we will just move forward.
    if (previousDirection == Unknown) {
        directionState = Forward;
        previousDirection = directionState;
        return directionState;
    }

    // Possible reason
    previousDirection = currentDirection;
    return previousDirection;
}

// get next instruction
Instruction getNextInstruction(int numSteps, Instruction instructionList[numSteps]) {
    Instruction out;
    // input is list of instructions and robot will react accordingly
    for(int i = 0; i < numSteps; i++) {
        if(instructionList[i].direction != Skip) {
            out.direction = instructionList[i].direction;
            out.ignoreL = instructionList[i].ignoreL;
            out.ignoreR = instructionList[i].ignoreR;
            return out; // return next instruction
        }
    }
    return out;
}

// Sets robot movement direction state according to currentDirection which is set by Check
void SetRobotMovement() {
    currentInstruction = getNextInstruction(numSteps, instructionList); // get current/ next instruction
    currentDirection = CheckSensorDirection(); // check sensors, adjust robot movement
    // move robot depending on sensors
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
        case ForwardAfterTurn:
            moveForward();
            break;
        case Backward:
            moveBackward();
            break;
        case waitForTurn:
            break;
        /*case waitForLeftTurn:
            moveForward();
            break;
        case waitForRightTurn:
            moveForward();
            break;*/
        case Unknown:
            // UNKNOWN CONFIGURATION
            break;  
    }
}