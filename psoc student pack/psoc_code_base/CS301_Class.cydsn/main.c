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
enum DirectionState {Forward, TurnRight, TurnLeft, AdjustToTheLeft, AdjustToTheRight, Stop, Unknown, HardForward, waitForTurn, ForwardAfterTurn, Backward};
enum DirectionState directionState;
enum Orientation {Up, Down, Left, Right}; // taken from origin at bottom left corner
enum DirectionState currentDirection = Stop;
enum DirectionState previousDirection = Unknown;
enum Orientation currentOrientation = Up;
enum Orientation previousOrientation = Up;
// ----------------------------------------
uint8 s1, s2, s3, s4, s5, s6 = 0; // black = 0, white = 1- initialise sensor signals
//* ========================================
uint32 totalMilliseconds = 0;
float totalDistance = 0; // in mm
//* ========================================
char buffer[69];
int quadDec2Count = 0;
int timerInt = 0;
int keepLedOn = 0;
uint32 stopBuffer = 0;

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
    //findPath(map, FILEPATH);
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
float yBlockSize = 12.84;
float xBlockSize = 9.13;
float blockSize;
uint8 currentRow;
uint8 currentCol; // need to initialise

enum DirectionState getDirection() {
    // determine block size
    if(previousOrientation == Up || previousOrientation == Down) {
        blockSize = yBlockSize;
    } else {
        blockSize = xBlockSize;
    }
    // find next direction after we reach a new coordinate
    if(totalDistance <= blockSize) {
        // MOVEMENT
        if (previousDirection == Stop) {
            if (stopBuffer <= 10) {
                directionState = Stop;
            } else {
                directionState = ForwardAfterTurn;
            }
            //currentDirection = previousDirection;
            return directionState;
        }
        
        if (previousDirection == ForwardAfterTurn) {
            if (s3 || s4) {
                //totalDistance = 0; // reset total distance after turn
                directionState = Forward;
                //currentDirection = previousDirection;
                return directionState;
            }
        }

        if(previousDirection == TurnRight) {
            if(s5 && s6) {
                directionState = TurnRight;
                //currentDirection = previousDirection;
                return directionState;
            } 
            else if (!s5 || !s6) {
                directionState = Stop;
                //currentDirection = previousDirection;
                return directionState;
            }
        }    

        if(previousDirection == TurnLeft) {
            if(s5 && s6) {
                directionState = TurnLeft;
                //currentDirection = previousDirection;
                return directionState;
            } 
            else if (!s5 || !s6) {
                directionState = Stop;
                //currentDirection = previousDirection;
                return directionState;
            }
        }

        // course correction
        if (previousDirection == Forward || previousDirection == AdjustToTheLeft || previousDirection == AdjustToTheRight) {
            if(s6) {
                directionState = AdjustToTheLeft; // keep adjusting to the left
            }
            if(s5) {
                directionState = AdjustToTheRight; // keep adjusting to the right
            }
            //previousOrientation = currentOrientation; // unchanged orientation
            return directionState;
        }
        
        // forward 1100
        if (s3 && s4 && !s5 && !s6) {
            directionState = Forward;
            //previousOrientation = currentOrientation; // unchanged orientation
            return directionState;   
        }

        if (previousDirection == Unknown) {
            directionState = Forward;
            //previousDirection = currentDirection;
            return directionState;
        }
    
        // Possible reason
        //previousDirection = currentDirection;
        return previousDirection;

    } else {
        switch (previousOrientation) {
            case Up:
                if(map[currentRow][currentCol + 1] == 8) {
                    currentOrientation = Up;
                    directionState = Forward;
                } else if (map[currentRow - 1][currentCol] == 8) {
                    currentOrientation = Left;
                    directionState = TurnLeft;
                } else if (map[currentRow + 1][currentCol] == 8) {
                    currentOrientation = Right;
                    directionState = TurnRight;
                }
                break;
            case Down:
                if(map[currentRow][currentCol - 1] == 8) {
                    currentOrientation = Down;
                    directionState = Forward;
                } else if (map[currentRow - 1][currentCol] == 8) {
                    currentOrientation = Right;
                    directionState = TurnRight;
                } else if (map[currentRow + 1][currentCol] == 8) {
                    currentOrientation = Left;
                    directionState = TurnLeft;
                }
                break;
            case Left:
                if(map[currentRow - 1][currentCol] == 8) {
                    currentOrientation = Left;
                    directionState = Forward;
                } else if (map[currentRow][currentCol + 1] == 8) {
                    currentOrientation = Up;
                    directionState = TurnRight;
                } else if (map[currentRow][currentCol - 1] == 8) {
                    currentOrientation = Down;
                    directionState = TurnLeft;
                }
                break;
            case Right:
                if(map[currentRow + 1][currentCol] == 8) {
                    currentOrientation = Right;
                    directionState = Forward;
                } else if (map[currentRow][currentCol + 1] == 8) {
                    currentOrientation = Up;
                    directionState = TurnLeft;
                } else if (map[currentRow][currentCol - 1] == 8) {
                    currentOrientation = Down;
                    directionState = TurnRight;
                }
                break;
            default:
                break;
        }
        totalDistance = 0; // reset total distance
    }
}

enum DirectionState CheckSensorDirection() {

    directionState = Stop;
    currentOrientation = Up; // initialise at start
    previousDirection = currentDirection;
    // determine orientation and relevant blocksize
    if(currentOrientation == Up || Down) {
        blockSize = yBlockSize;
    } else {
        blockSize = xBlockSize;
    }
    // BLOCK TRACKING
    if (totalDistance >= blockSize) {
        switch(currentOrientation) {
            case Up:
                currentCol++;
                break;
            case Down:
                currentCol--;
                break;
            case Left:
                currentRow--;
                break;
            case Right:
                currentRow++;
                break;
            default:
                break;
        }
        totalDistance = 0; // reset total distance
    }

   // MOVEMENT
    if (previousDirection == Stop) {
        if (stopBuffer <= 10) {
            directionState = Stop;
        } else {
            directionState = ForwardAfterTurn;
        }
        currentDirection = previousDirection;
        return directionState;
    }
    
    // If one of the sensors are still on the line after we turn, move forward
    if (previousDirection == ForwardAfterTurn) {
        if (s3 || s4) {
            totalDistance = 0; // reset total distance after turn
            directionState = Forward;
            currentDirection = previousDirection;
            return directionState;
        }
    }

    if(previousDirection == TurnRight) {
        if(s5 && s6) {
            directionState = TurnRight;
            currentDirection = previousDirection;
            return directionState;
        } 
        else if (!s5 || !s6) {
            directionState = Stop;
            currentDirection = previousDirection;
            return directionState;
        }
    }    

    if(previousDirection == TurnLeft) {
        if(s5 && s6) {
            directionState = TurnLeft;
            currentDirection = previousDirection;
            return directionState;
        } 
        else if (!s5 || !s6) {
            directionState = Stop;
            currentDirection = previousDirection;
            return directionState;
        }
    }
    
    /*if (previousDirection == waitForTurn) {
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
    }*/

    // reached fork/ alternate paths or needs to turn
    if((previousDirection == Forward && (s3 || s4)) || (previousDirection == waitForTurn && s3 && s4)) {
        // check for next step in calculated path, robot will know it's location and next step
        // optimal steps are marked with an 8
        switch (previousOrientation) {
            case Up:
                if(map[currentRow][currentCol + 1] == 8) {
                    currentDirection = Up;
                    directionState = Forward;
                } else if (map[currentRow - 1][currentCol] == 8) {
                    currentDirection = Left;
                    directionState = TurnLeft;
                } else if (map[currentRow + 1][currentCol] == 8) {
                    currentDirection = Right;
                    directionState = TurnRight;
                }
                break;
            case Down:
                if(map[currentRow][currentCol - 1] == 8) {
                    currentDirection = Down;
                    directionState = Forward;
                } else if (map[currentRow - 1][currentCol] == 8) {
                    currentDirection = Right;
                    directionState = TurnRight;
                } else if (map[currentRow + 1][currentCol] == 8) {
                    currentDirection = Left;
                    directionState = TurnLeft;
                }
                break;
            case Left:
                if(map[currentRow - 1][currentCol] == 8) {
                    currentDirection = Left;
                    directionState = Forward;
                } else if (map[currentRow][currentCol + 1] == 8) {
                    currentDirection = Up;
                    directionState = TurnRight;
                } else if (map[currentRow][currentCol - 1] == 8) {
                    currentDirection = Down;
                    directionState = TurnLeft;
                }
                break;
            case Right:
                if(map[currentRow + 1][currentCol] == 8) {
                    currentDirection = Right;
                    directionState = Forward;
                } else if (map[currentRow][currentCol + 1] == 8) {
                    currentDirection = Up;
                    directionState = TurnLeft;
                } else if (map[currentRow][currentCol - 1] == 8) {
                    currentDirection = Down;
                    directionState = TurnRight;
                }
                break;
            default:
                break;
        }
        return directionState;
    }

    // wait for turn at end of line
    /*if(s5 && s6 && (previousDirection == Forward || (previousDirection == AdjustToTheLeft || previousDirection == AdjustToTheRight))) {
        directionState = waitForTurn; // need to wait to check for a black line
        return directionState;
    }*/
    
    // course correction
    if (previousDirection == Forward || previousDirection == AdjustToTheLeft || previousDirection == AdjustToTheRight) {
        if(s6) {
            directionState = AdjustToTheLeft; // keep adjusting to the left
        }
        if(s5) {
            directionState = AdjustToTheRight; // keep adjusting to the right
        }
        previousOrientation = currentOrientation; // unchanged orientation
        return directionState;
    }
    
    // forward 1100
    if (s3 && s4 && !s5 && !s6) {
        directionState = Forward;
        previousOrientation = currentOrientation; // unchanged orientation
        return directionState;   
    }
    
    //turn left 0111
    /*if (!s3 && s4 && s5 && s6) {
        directionState = TurnLeft;
        switch(previousOrientation) {
            case Up:
                currentDirection = Left;
                break;
            case Down:
                currentDirection = Right;
                break;
            case Left:
                currentDirection = Up;
                break;
            case Right:
                currentDirection = Down;
                break;
            default:
                break;
        }
        previousDirection = currentDirection;
        return directionState;
    }*/
    
    // turn right if 1011
    /*if (s3 && !s4 && s5 && s6) {
        directionState = TurnRight;
        switch(previousOrientation) {
            case Up:
                currentDirection = Right;
                break;
            case Down:
                currentDirection = Left;
                break;
            case Left:
                currentDirection = Down;
                break;
            case Right:
                currentDirection = Up;
                break;
            default:
                break;
        }
        previousDirection = currentDirection;
        return directionState;
    }*/
    
    // If currentDirection is Unknown, we continue with the previous direction.
    // However, if the previous direction is also Unknown, we will just move forward.
    if (previousDirection == Unknown) {
        directionState = Forward;
        previousDirection = currentDirection;
        return directionState;
    }
    
    // Possible reason
    previousDirection = currentDirection;
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
