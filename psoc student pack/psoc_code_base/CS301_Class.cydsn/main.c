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
enum RobotMovement {Forward, TurnRight, TurnLeft, AdjustToTheLeft, AdjustToTheRight, Stop, Unknown, waitForTurn, ForwardAfterTurn, Backward, Spin180};
enum RobotMovement currentDirection, previousDirection = Forward; 
enum RobotMovement GetMovementAccordingToInstruction();
enum OrientationState currentRobotOrientation, previousOrientation = Down;
Instruction currentInstruction;
int numSteps;
void traversePath(int numSteps, Instruction instructionList[]);
Instruction * instructionList; // pointer to array
uint32 instructionIndex = 0;
Instruction GetInstructionAtIndex(int numSteps, Instruction instructionList[numSteps], int instructionIndex);
float CalculateDistanceToTravel(float blockSize);
void MoveToNextInstruction();
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
// ========================================= FLAGS
uint32 stopBuffer = 0;

uint8 turnStartedFlag = 0;
uint8 turnFinishedFlag = 0;

uint8 forwardUntilTargetStartedFlag = 0;
uint8 uTurnStartedFlag = 0;
uint8 uTurnFinishedFlag = 0;

uint8 junctionConfiguration[4] = {0};

float blockSizeTotal = 0;
//char map[MAX_ROWS][MAX_COLS]; // global map array- stores the map

CY_ISR (speedTimer) {
    timerInt = 1;
    //quadDec_M1 used for turning macros
    quadDec2Count = QuadDec_M2_GetCounter();
    
    if ((currentDirection == Forward || 
        currentDirection == ForwardAfterTurn ||
        currentDirection == waitForTurn || 
        currentDirection == AdjustToTheLeft ||
        currentDirection == AdjustToTheRight ||
        currentDirection == Backward) &&
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
    instructionList = findPath(map, food_list, 0);
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

float xBlocksize = 122; // 122 mm
float yBlocksize = 80; // 80 mm
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
/*
enum RobotMovement CheckSensorDirection() {
    float blocksize;
    if(currentRobotOrientation == Up || currentRobotOrientation == Down) {
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
        //directionState = GetNextStep(); // get next step at each block
        totalDistance = 0; // reset distance
        previousDirection = directionState;
        stoppedAfterTurn = 0;
        return directionState;
    }
    if (totalDistance >= blocksize) {
        //directionState = GetNextStep(); // get next step at each block
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
*/

enum RobotMovement ForwardCourseCorrection();
enum RobotMovement ForwardCourseCorrection() {
    // if S5 and S6 are on black, move forward
    if (!s5 && !s6) {
        if (previousDirection != AdjustToTheLeft) {
            return AdjustToTheLeft;
        }
        if (previousDirection != AdjustToTheRight) {
            return AdjustToTheRight;    
        }
    }
    
    // ATTEMPTED COURSE CORRECTION WHEN BOTH ON WHITE
    if (s5 && s6) {
        if (previousDirection != AdjustToTheLeft) {
            return AdjustToTheLeft;
        }
        if (previousDirection != AdjustToTheRight) {
            return AdjustToTheRight;    
        }
    }

    // if S5 OR S6 are on white, adjust accordingly
    if (s5) {
        return AdjustToTheRight;    
    }
    if (s6) {
        return AdjustToTheLeft;    
    }
   
    // We should never actually get to this point
    // If S5 and S6 condition are GONE, then we will reach this point.
    return Stop;
}

enum RobotMovement SpinCourseCorrection();

uint8 spinCourseCorrectionStarted = 0;
enum RobotMovement lastDirectionAfter180 = Unknown;
enum RobotMovement SpinCourseCorrection() {
    // Set first iteration flag.
    if (!spinCourseCorrectionStarted) {
        spinCourseCorrectionStarted = 1;    
        lastDirectionAfter180 = currentDirection;
    }
    
    // if S5 and S6 are on black, stop
    if (!s5 && !s6) {
        spinCourseCorrectionStarted = 0;
        return Backward;
    }
    
    // ATTEMPTED COURSE CORRECTION WHEN BOTH ON WHITE
    if (s5 && s6) {
        if (lastDirectionAfter180 != TurnLeft) {
            return Forward;
        }
        if (lastDirectionAfter180 != TurnRight) {
            return Forward;    
        }
    }

    // if S5 OR S6 are on white, adjust accordingly
    if (s5) {
        return TurnRight;    
    }
    if (s6) {
        return TurnLeft;    
    }
   
    // We should never actually get to this point
    // If S5 and S6 condition are GONE, then we will reach this point.
    return Stop;
}
enum RobotMovement GetMovementAccordingToInstruction() {
    float blocksize;
    if(currentRobotOrientation == Up || currentRobotOrientation == Down) {
        blocksize = yBlocksize;
    } else {
        blocksize = xBlocksize;
    }
    
    // This function makes it so that it will return a robotmovement direction according to the current instruction
    // AND move to the next instruction when EITHER totalDistance >= blocksize, 
        // OR an EVENT happens in the current instruction
        // e.g. CurrentInstruction: GoForward 
        //      S3 on BLACK
        //      Check Ignore Count
        //      If Ignore Count = 0, move to next instruction.
    
    enum InstructionDirection currentInstructionDirection = currentInstruction.direction;
    
    switch (currentInstructionDirection) {
        case GoForward:
            // if s3 or s4 go off, check ignoreCount
            //      if ignoreCount == 0, go to next direction
            //      return stop
            if (!s3) {
                if (currentInstruction.ignoreL <= 0) {
                    MoveToNextInstruction();
                    return Stop;
                }
                currentInstruction.ignoreL--;
            }
            
            if (!s4) {
                if (currentInstruction.ignoreR <= 0) {
                    MoveToNextInstruction();
                    return Stop;
                }
                currentInstruction.ignoreR--;
            }
            return ForwardCourseCorrection();
            break;
        case waitForLeftTurn:
            // continue moving forward until s3 goes on black
            // if we are turning left already
                // wait until s5 || s6 are on black
                // return stop

            if (turnFinishedFlag) {
                if (s3) {
                    turnFinishedFlag = 0;
                    MoveToNextInstruction();    
                }
                else {
                    return ForwardCourseCorrection();        
                }
            }
            
            
            if (currentDirection == Stop) {
                // We should be facing a different direction now so we move to the next instruction.    
                if (stopBuffer <= 50) {
                    return Stop;   
                }
                return ForwardCourseCorrection();
            }
         
            // if we are already turning left, then check if s5 && s6 are on black
            if (currentDirection == TurnLeft) {
                // If turn started and we are still on black, continue turning
                // Otherwise, if all sensors are on white, turnStartedFlag = 0;
                if (turnStartedFlag) {
                    if (s3 && (s5 && s6)) {
                        turnStartedFlag = 0;
                    }
                }
                if (!turnStartedFlag && !s3 && (!s5 && !s6)) {
                    turnFinishedFlag = 1;
                    return Stop;
                }
                else
                {
                    return TurnLeft;
                }
            }
            
            // If we are not already turning left then once s3 goes on BLACK, turn left
            if (!s3) {
                turnStartedFlag = 1;
                return TurnLeft;
            }
            
            // Otherwise, keep going forward
            return ForwardCourseCorrection(); 
            break;
        case waitForRightTurn:
            // continue moving forward until s4 goes on black
            // if we are turning left already
                // wait until s5 || s6 are on black
                // return stop

            if (turnFinishedFlag) {
                if (s4) {
                    turnFinishedFlag = 0;
                    MoveToNextInstruction();    
                }
                else {
                    return ForwardCourseCorrection();        
                }
            }
            
            
            if (currentDirection == Stop) {
                // We should be facing a different direction now so we move to the next instruction.    
                if (stopBuffer <= 50) {
                    return Stop;   
                }

                return ForwardCourseCorrection();
            }
                      
            // if we are already turning right, then check if s5 && s6 are on black
            if (currentDirection == TurnRight) {
                // If turn started and we are still on black, continue turning
                // Otherwise, if all sensors are on white, turnStartedFlag = 0;
                if (turnStartedFlag) {
                    if (s4 && (s5 && s6)) {
                        turnStartedFlag = 0;
                    }
                }
                if (!turnStartedFlag && !s4 && (!s5 && !s6)) {
                    turnFinishedFlag = 1;
                    return Stop;
                }
                else
                {
                    return TurnRight;
                }
            }
            
            // If we are not already turning right then once s4 goes on BLACK, turn left
            if (!s4) {
                turnStartedFlag = 1;
                return TurnRight;
            }
            
            // Otherwise, keep going forward
            return ForwardCourseCorrection(); 
            break;
        case ForwardUntilTarget:
            // Reset distance on first iteration of this instruction
            if (!forwardUntilTargetStartedFlag) {
                forwardUntilTargetStartedFlag = 1;
                totalDistance = 0;

                blockSizeTotal = blocksize * 3;
            }
            
            if (!s3) {
                if (currentInstruction.ignoreL <= 0) {
                    MoveToNextInstruction();
                    return Stop;
                }
                currentInstruction.ignoreL--;
            }
            
            if (!s4) {
                if (currentInstruction.ignoreR <= 0) {
                    MoveToNextInstruction();
                    return Stop;
                }
                currentInstruction.ignoreR--;
            }
            
            // If totalDistance >= blockSizeTotal then we should be at target
            if (totalDistance >= blockSizeTotal) {
                // Get next instruction
                MoveToNextInstruction();

                
                return Stop;
            }
            
            return ForwardCourseCorrection();
            break;
        case StopAtTarget:
            // Reset Forward Until Target Flags (Had to move in here to make it work)
            // Reset flags
            totalDistance = 0;
            forwardUntilTargetStartedFlag = 0;
            blockSizeTotal = 0;
            
            if (stopBuffer <= 200) {
                return Stop;    
            }
            return TurnRight;
            break;
        case uTurn:
            if (stopBuffer <= 200) {
                return Stop;    
            }
            
            if (!uTurnStartedFlag) {
                uTurnStartedFlag = 1;
                return Spin180;
            } else {
                uTurnStartedFlag = 0;
                stopBuffer = 0;
                return Stop;
                /*
                // After 180, if we are not on black, then we course correct
                if (!s5 && !s6) {
                    uTurnFinishedFlag = 1;    
                }
                
                if (!uTurnFinishedFlag) {
                    return SpinCourseCorrection();    
                }
                
                if (uTurnFinishedFlag) {
                    // GetNextInstruction
                    uTurnFinishedFlag = 0;
                    uTurnStartedFlag = 0;
                    return Backward;    
                }*/
                return Backward;
            }
            break;
        default:
            return Stop;
            break;
    }
    return Stop;
}

// Calculate total blocksize to travel until target
float CalculateDistanceToTravel(float blockSize) {
    float totalBlockSize;
    
    // Check for Row, Col that target is 
    int targetRow = food_list[0][0];
    int targetCol = food_list[0][1];
    
    int pathCount = 0;
    
    // Depending on the robot orientation
    // count++ if row, col is 8
    // count reset if row, col is 1 or 0
    // save count if row, col is 9
    switch (currentRobotOrientation) {
        case Up:
            // Column
            // Start from Bottom (since we're facing up)
            for (int i = MAX_ROWS; i >= 0; i--) {
                if (map[i][targetCol] == 9) {
                    break;    
                }
                if (map[i][targetCol] == 8) {
                    pathCount++;    
                }
                if (map[i][targetCol] == 0 || map[i][targetCol] == 1) {
                    pathCount = 0;
                }
            }
            break;
        case Down:
            // Target Column, Check Row
            // Start from Top
            for (int i = 0; i < MAX_ROWS; i++) {
                if (map[i][targetCol] == 9) {
                    break;    
                }
                if (map[i][targetCol] == 8) {
                    pathCount++;    
                }
                if (map[i][targetCol] == 0 || map[i][targetCol] == 1) {
                    pathCount = 0;
                }
            }
            break;
        case Left:
            // Target Row, Check Col
            // Start from Right
            for (int i = MAX_COLS; i >= 0; i--) {
                if (map[targetRow][i] == 9) {
                    break;    
                }
                if (map[targetRow][i] == 8) {
                    pathCount++;    
                }
                if (map[targetRow][i] == 0 || map[i][targetCol] == 1) {
                    pathCount = 0;
                }
            }
            break;
        
        case Right:
            // Target Row, Check Col
            // Start from Left
            for (int i = 0; i < MAX_COLS; i++) {
                if (map[targetRow][i] == 9) {
                    break;    
                }
                if (map[targetRow][i] == 8) {
                    pathCount++;    
                }
                if (map[targetRow][i] == 0 || map[i][targetCol] == 1) {
                    pathCount = 0;
                }

            }
            break;
        
    }
    totalBlockSize = blockSize * pathCount;
    return totalBlockSize;
}

// get next instruction
void MoveToNextInstruction() {
    instructionIndex++;
}


Instruction GetInstructionAtIndex(int numSteps, Instruction instructionList[numSteps], int instructionIndex) {
    Instruction nextInstruction;
    // input is list of instructions and robot will react accordingly
    for(int i = instructionIndex; i < numSteps; i++) {
        if(instructionList[i].direction != Skip) {
            nextInstruction.direction = instructionList[i].direction;
            nextInstruction.ignoreL = instructionList[i].ignoreL;
            nextInstruction.ignoreR = instructionList[i].ignoreR;
            nextInstruction.expectedOrientation = instructionList[i].expectedOrientation;
            return nextInstruction; // return next instruction
        }
        instructionIndex = i;
    }
    return nextInstruction;
}

// Sets robot movement direction state according to currentDirection which is set by Check
void SetRobotMovement() {
    currentInstruction = GetInstructionAtIndex(numSteps, instructionList, instructionIndex); // get current/ next instruction
    previousDirection = currentDirection;
    currentDirection = GetMovementAccordingToInstruction(); // check sensors, adjust robot movement
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
        case Spin180:
            RotateClockwise180Degrees();
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