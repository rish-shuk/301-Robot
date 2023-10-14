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
volatile static uint32 instructionIndex = 0;
Instruction GetInstructionAtIndex();
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
uint16 forwardBuffer = 0;
volatile static uint8 turnStartedFlag = 0;
volatile static uint8 turnFinishedFlag = 0;

volatile static uint8 forwardUntilTargetStartedFlag = 0;
volatile static uint8 uTurnStartedFlag = 0;
volatile static uint8 uTurnFinishedFlag = 0;

volatile static uint8 leftStatusFlag = 0;
volatile static uint8 rightStatusFlag = 0;

volatile static uint8 spinCourseCorrectionStarted = 0;
volatile static uint8 currentIgnoreL = 0;
volatile static uint8 currentIgnoreR = 1;

void RotateClockwise180Degrees();

volatile static uint8 totalLineCount = 0;
volatile static uint8 lineCount = 0;
volatile static uint8 onLineFlag = 0;

volatile static uint8 tooEarlyFlag = 0;
volatile static uint8 tooLateFlag = 0;

uint8 junctionConfiguration[4] = {0};
uint8 currentFoodListIndex = 0;

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
    if (currentDirection == Forward || currentDirection == AdjustToTheLeft || currentDirection ==  AdjustToTheRight) {
        forwardBuffer = forwardBuffer + 1;
    } else {
        forwardBuffer = 0;    
    }
    SetRobotMovement(); 
    ResetSensorFlags(); // Reset Sensor Flags for Next rising Eddge
    Timer_LED_ReadStatusRegister();
}


int main() {
// ----- INITIALIZATIONS ----------
    CYGlobalIntEnable;
    ResetSensorFlags();
    init(); // initialise clocks, pwms, adc, dac etc- done in header file
    
    // MAKE SURE TO CHANGE ORIENTATION
    instructionList = findPathNewOrientation(map, food_list, currentFoodListIndex, Down); // initialise with starting orientation
    numSteps = instructionsListLength();
    
    currentInstruction = GetInstructionAtIndex();
    
    currentIgnoreL = instructionList[instructionIndex].ignoreL;
    currentIgnoreR = instructionList[instructionIndex].ignoreR;
    
    isr_speed_StartEx(speedTimer); // start interrupt
    isr_Timer_LED_StartEx(TIMER_FINISH);
    S3_detected_StartEx(S3_DETECTED);
    S4_detected_StartEx(S4_DETECTED);
    S5_detected_StartEx(S5_DETECTED);
    S6_detected_StartEx(S6_DETECTED);
    Timer_LED_Start();

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
    }
    return 0;
}

// Calculations
//* ========================================
int16 quadCountToRPM(uint16 count) {
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

float xBlocksize = 116; // 122 mm
float yBlocksize = 72; // 80 mm
uint8 currentRow = 1;
uint8 currentCol = 1;

uint8 stoppedAfterTurn = 0;
uint8 ignoreSensor = 0;
// needs to make sure robot is going in the correct direction (supplied from instruction)

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

enum RobotMovement lastDirectionAfter180 = Unknown;

enum RobotMovement SpinCourseCorrection() {
    // Set first iteration flag.
    if (!spinCourseCorrectionStarted) {
        spinCourseCorrectionStarted = 1;    
        lastDirectionAfter180 = currentDirection;
    }
    
    // ATTEMPTED COURSE CORRECTION WHEN BOTH ON WHITE
    if (s5 && s6) {
        if (lastDirectionAfter180 != TurnLeft) {
            return TurnLeft;
        }
        if (lastDirectionAfter180 != TurnRight) {
            return TurnRight;    
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
enum RobotMovement SpinTurnCourseCorrection();
enum RobotMovement SpinTurnCourseCorrection() {
    // if S5 and S6 are on black, move forward
    if (!s5 && !s6) {
        if (previousDirection != AdjustToTheLeft) {
            return TurnLeft;
        }
        if (previousDirection != AdjustToTheRight) {
            return TurnRight;    
        }
    }
    
    // ATTEMPTED COURSE CORRECTION WHEN BOTH ON WHITE
    if (s5 && s6) {
        if (previousDirection != AdjustToTheLeft) {
            return TurnLeft;
        }
        if (previousDirection != AdjustToTheRight) {
            return TurnRight;    
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

uint8 firstTurnIteration = 0;
enum RobotMovement GetMovementAccordingToInstruction() {
    float blocksize;
    if(currentInstruction.expectedOrientation == Up || currentInstruction.expectedOrientation == Down) {
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
    
    /*
    if (instructionList[instructionIndex + 1].direction == uTurn && currentInstructionDirection == ForwardUntilTarget) {
        if (s5 && s6) {
            MoveToNextInstruction(); // case to start uTurns when needed. prevents uTurn overshooting
            return Stop;
        }
    }
    */
    
    switch (currentInstructionDirection) {
        case GoForward:
            if(currentDirection == Stop) {
                if(stopBuffer <= 25) {
                    return Stop;
                } else {
                    stopBuffer = 100;
                }
            }
            if (s3) {
                leftStatusFlag = 1;
            }
            
            if (s4) {
                rightStatusFlag = 1;
            }
            
            // LEFT WING CHECK =-=-=-=-=-=-=-=-=-=-=
            if (leftStatusFlag) {
                if (!s3) {
                    leftStatusFlag = 0;    
                    if (currentIgnoreL == 0) {
                        MoveToNextInstruction();
                        return Stop;
                    }
                    if (currentIgnoreL > 0) {
                        currentIgnoreL--;    
                    }
                }
            }
            // RIGHT WING CHECK =-=-=-=-=-=-=-=-=-=-=
            if (rightStatusFlag) {
                if (!s4) {
                    rightStatusFlag = 0;
                    if (currentIgnoreR == 0) {
                        MoveToNextInstruction();
                        return Stop;
                    }
                    if (currentIgnoreR > 0) {
                        currentIgnoreR--;    
                    }
                }
            }
          
            return ForwardCourseCorrection();
            break;
        case waitForLeftTurn:
            // continue moving forward until s3 goes on black
            // if we are turning left already
                // wait until s5 || s6 are on black
                // return stop
            if (!firstTurnIteration) {
                if (!s3) {
                    firstTurnIteration = 1;
                    return Stop;    
                }
                if (s3) {
                    //return Backward;    
                }
            }
            
            if (turnFinishedFlag) {
                if (!s3 && !s5 && !s6) {
                    // we are correctly on the line so go forward until S4 on white
                    return ForwardCourseCorrection();
                }
                if (s5 || s6) {
                    return SpinCourseCorrection();
                }
                if (forwardBuffer <= 10) {
                    return ForwardCourseCorrection();    

                } else {
                    forwardBuffer = 100;
                    if (s3) {
                        turnFinishedFlag = 0;
                        MoveToNextInstruction();   
                        return Stop;  
                    }
                }
            }
            
            
            if (currentDirection == Stop) {
                // We should be facing a different direction now so we move to the next instruction.    
                if (stopBuffer <= 25) {
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
                if (!turnStartedFlag && !s3 && (!s5 || !s6)) {
                    turnFinishedFlag = 1;
                    firstTurnIteration = 0;
                    return ForwardAfterTurn;
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
            if (!firstTurnIteration) {
                if (!s4) {
                    firstTurnIteration = 1; 
                    return Stop;
                }
                if (s4) {
                    //return Backward;    
                }
            }

            if (turnFinishedFlag) {
                if (!s4 && !s5 && !s6) {
                    // we are correctly on the line so go forward until S4 on white
                    return ForwardCourseCorrection();
                }
                if (s5 || s6) {
                    return SpinCourseCorrection();
                }
                if (forwardBuffer <= 10) {
                    return ForwardCourseCorrection();    

                } else {
                    forwardBuffer = 100;
                    if (s4) {
                        firstTurnIteration = 0;
                        turnFinishedFlag = 0;
                        MoveToNextInstruction();   
                        return Stop;  
                    }
                }
            }
            
            if (currentDirection == Stop) {
                // We should be facing a different direction now so we move to the next instruction.    
                if (stopBuffer <= 25) {
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
                if (!turnStartedFlag && !s4 && (!s5 || !s6)) {
                    turnFinishedFlag = 1;
                    return ForwardAfterTurn;
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

                //blockSizeTotal = CalculateDistanceToTravel(blocksize);
                int blocksToTarget = currentInstruction.distanceToTarget;
                blockSizeTotal = blocksize * blocksToTarget;
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
            
            if (currentDirection == Stop) {
                if (stopBuffer <= 1) {
                    return Stop;    
                } else {
                    stopBuffer = 250;
                }
            }
            
            // RESET EVERYTHING AND FIND NEW PATH FOR NEXT FOOD LIST
            // Make sure index doesn't go over cap
            // Stop at final location
            if (currentFoodListIndex == 4) {
                return Stop;    
            }
            
            currentFoodListIndex++;
            instructionIndex = 0;
            instructionList = findPathNewOrientation(map, food_list, currentFoodListIndex, currentInstruction.expectedOrientation);
            numSteps = instructionsListLength();
    
            currentInstruction = GetInstructionAtIndex();

            currentIgnoreL = instructionList[instructionIndex].ignoreL;
            currentIgnoreR = instructionList[instructionIndex].ignoreR;
            
            return Stop;
            break;
        case uTurn:
            if (currentDirection == Stop) {
                if (stopBuffer <= 50) {
                    return Stop;    
                } else {
                    stopBuffer = 100;    
                }
            }
            
            
            if (!uTurnStartedFlag) {
                uTurnStartedFlag = 1;
                spinCourseCorrectionStarted = 0;
                return Spin180;
            } 
            
            if (uTurnFinishedFlag) {
                // MOVE TO NEXT INSTRUCTION
                MoveToNextInstruction();
                return Stop;    
            }
            
            // Repeat SpinCourseCorrection until both or one on black
            if (uTurnStartedFlag) {
                if (!s5 && !s6) {
                    totalLineCount = 0;
                    lineCount = 0;
                    tooEarlyFlag = 0;
                    tooLateFlag = 0;
                    uTurnFinishedFlag = 1;    
                }    
            }
            
            return SpinCourseCorrection();
            break;
        default:
            return Stop;
            break;
    }
    return Stop;
}

// get next instruction
void MoveToNextInstruction() {
    currentIgnoreL = 0;
    currentIgnoreR = 0;
    instructionIndex++;
    currentIgnoreL = instructionList[instructionIndex].ignoreL;
    currentIgnoreR = instructionList[instructionIndex].ignoreR;

}

Instruction GetInstructionAtIndex() {
    Instruction nextInstruction;
    // input is list of instructions and robot will react accordingly
    nextInstruction.direction = instructionList[instructionIndex].direction;        
    nextInstruction.expectedOrientation = instructionList[instructionIndex].expectedOrientation;
    nextInstruction.distanceToTarget = instructionList[instructionIndex].distanceToTarget;
    return nextInstruction;
}

void RotateClockwise180Degrees() {
    uint16 count1 = (32767 + (6553 * 0.9));
    uint16 count2 = (32767 + (6553 * 0.9));
    PWM_1_WriteCompare(count1);
    PWM_2_WriteCompare(count2);
    
    int quadPulseCount = 0;
    QuadDec_M1_SetCounter(0);
    // Before 180, save initial configuration
    if (!s3) {
        totalLineCount++;    
    }
    if (!s4) {
        totalLineCount++;    
    }
    if (!s5 || !s6) {
        totalLineCount++;    
    }
    
    // During 180
    while(quadPulseCount < 210) {
        quadPulseCount = QuadDec_M1_GetCounter();
        if (!s3) {
            onLineFlag = 1;    
        }
        
        if (onLineFlag) {
            lineCount++;    
        }
        
        if (!onLineFlag) {
            if (s3) {
                onLineFlag = 0;    
            }
        }
    }
    
    // After 180
    if (totalLineCount == 0) {
        // we are on no lines
        tooEarlyFlag = 1;
        QuadDec_M1_SetCounter(0);
        return;
    }
    else if (lineCount > totalLineCount) {
        tooLateFlag = 1;    
    }
    else if (lineCount < totalLineCount) {
        tooEarlyFlag = 1;    
    }
    QuadDec_M1_SetCounter(0);
}

// Sets robot movement direction state according to currentDirection which is set by Check
void SetRobotMovement() {
    //MoveToNextInstruction(); // debug purposes
    currentInstruction = GetInstructionAtIndex(); // get current/ next instruction
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
        case Unknown:
            break;  
    }
}

