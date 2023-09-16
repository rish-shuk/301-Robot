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
//#include "usbUART.h"
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
enum DirectionState {Forward, TurnRight, TurnLeft, AdjustToTheLeft, AdjustToTheRight, Stop, Unknown, HardForward};
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
    LED_Write(1u);
}

CY_ISR(S5_DETECTED) {
    // Sensor has detected WHITE
    s5 = 1; // , Black = 0, White = 1
    LED_Write(1u);
}

CY_ISR(S6_DETECTED) {
    // Sensor has detected WHITE
    s6 = 1; // , Black = 0, White = 1
    LED_Write(1u);
}

CY_ISR(TIMER_FINISH) {
    // Reset Sensor Flags for Next rising Eddge
    // (s1 = 0, s2 = 0... etc.)
    LED_Write(0u);
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
    RF_BT_SELECT_Write(0);
    
    usbPutString("Initialised UART");
    for(;;)
    {
        //traverseMap(map);
        //rotationAntiClockwise();
        //rotationClockwise();
        
        
        
        if(timerInt == 1) {
            timerInt = 0;
            // calculate RPM of M2
            quadCountToRPM(quadDec2Count);
            sprintf(buffer, "%lu", totalDistance);
            usbPutString(buffer);
            usbPutString(" ");
        }
        handle_usb();
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
    previousDirection = currentDirection;    
    
    // forward 111100
    if (s1 && s2 && s3 && s4 && !s5 && !s6) {
        directionState = Forward;
        return directionState;   
    }

    /* COURSE CORRECTION COURSE CORRECTION COURSE CORRETION */
    // Only need to course correct when direction state is forward
    /*
    if (previousDirection == Forward || previousDirection == AdjustToTheLeft || previousDirection == AdjustToTheRight) {
        // If robot is deviating to the left where top right sensor and bottom left sensor is on black
        // we turn right until all sensors are on white again
        if (!s1 && s2 && s5 && !s6) {
            directionState = AdjustToTheRight;
            return directionState;
        }  
        
        // If robot is deviating to the right where top left sensor and bottom right sensor is on black
        // we turn left until all sensors are on white again
        if (s1 && !s2 && !s5 && s6) {
            directionState = AdjustToTheLeft;
            return directionState;
        }
        
        // OLD COURSE CORRECTION ^^^^^^^^^^
        // NEW COURSE CORRECTION vvvvvvvvvv
    
        if (!s1 || !s6) {
            directionState = AdjustToTheRight;
            return directionState;
        }
    
        if (!s2 || !s5) {
            directionState = AdjustToTheLeft;
            return directionState;
        }
    }
    */
    /* COURSE CORRECTION COURSE CORRECTION COURSE CORRETION */
    
    // Left sensor is on black and right sensor is on white
    
    //turn left
    /*
    if (s1 && s2 && !s3 && s4 && s5 && s6) 
        directionState = TurnLeft;
        return directionState;
    }*/
    
    // Right sensor is on white and right sensor is on black
    // everything else is on white
    // turn right if 111011
    if (s1 && s2 && s3 && !s4 && s5 && s6) {
        directionState = TurnRight;
        return directionState;
    }
    
    
    // ====== After Initial turn ======
    // -- This accounts for the transition period between turning at an intersection --
    if (previousDirection == TurnRight || previousDirection == TurnLeft) {
        if ((s1 && s2 && s3 && !s4 && !s5 && !s6) ||
        (s1 && s2 && !s3 && s4 && !s5 && !s6)) {
            directionState = HardForward;
            return directionState;
        }
    }
    
    
    // if all sensors are on black -- we are currently in darkness so don't move
    // OR, all sensors are on white.
    // 111111 || 000000 stop
    if (!(s1 && s2 && s3 && s4 && s5 && s6) ||
        (s1 && s2 && s3 && s4 && s5 && s6)) {
        directionState = Stop;
        return directionState;
    }
        
    
    // If the code gets up to this point then no conditions have been met
    // The sensors are in a configuration that has not been covered thus
    // the currentDirection to turn into is unknown.
        
    // If currentDirection is Unknown, we continue with the previous direction.
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
            rotationClockwise();
            break;
        case TurnLeft:
            rotationAntiClockwise();
            break;
        case AdjustToTheRight:
            keepRotatingClockwise();
            break;
        case AdjustToTheLeft:
            keepRotatingAntiClockwise();
            break;
        case Stop:
            stopMoving();
            break;
        case HardForward:
            moveForwardForSpecifiedCount();
            break;
        case Unknown:
            // UNKNOWN CONFIGURATION
            break;  
    }
}

// ======================================
// USBUART
//* ========================================
    void usbPutString(char *s)
{
// !! Assumes that *s is a string with allocated space >=64 chars     
//  Since USB implementation retricts data packets to 64 chars, this function truncates the
//  length to 62 char (63rd char is a '!')

#ifdef USE_USB     
    while (USBUART_CDCIsReady() == 0);
    s[63]='\0';
    s[62]='!';
    USBUART_PutData((uint8*)s,strlen(s));
#endif
}
//* ========================================
void usbPutChar(char c)
{
#ifdef USE_USB     
    while (USBUART_CDCIsReady() == 0);
    USBUART_PutChar(c);
#endif    
}
//* ========================================
void handle_usb()
{
    // handles input at terminal, echos it back to the terminal
    // turn echo OFF, key emulation: only CR
    // entered string is made available in 'line' and 'flag_KB_string' is set
    
    static uint8 usbStarted = FALSE;
    static uint16 usbBufCount = 0;
    uint8 c; 
    

    if (!usbStarted)
    {
        if (USBUART_GetConfiguration())
        {
            USBUART_CDC_Init();
            usbStarted = TRUE;
        }
    }
    else
    {
        if (USBUART_DataIsReady() != 0)
        {  
            c = USBUART_GetChar();

            if ((c == 13) || (c == 10))
            {
//                if (usbBufCount > 0)
                {
                    entry[usbBufCount]= '\0';
                    strcpy(line,entry);
                    usbBufCount = 0;
                    flag_KB_string = 1;
                }
            }
            else 
            {
                if (((c == CHAR_BACKSP) || (c == CHAR_DEL) ) && (usbBufCount > 0) )
                    usbBufCount--;
                else
                {
                    if (usbBufCount > (BUF_SIZE-2) ) // one less else strtok triggers a crash
                    {
                       USBUART_PutChar('!');        
                    }
                    else
                        entry[usbBufCount++] = c;  
                }  
            }
        }
    }    
}



