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

//* ========================================
void usbPutString(char *s);
void usbPutChar(char c);
void handle_usb();
void changeDutyCycle(float percentage);
void reverseRightWheel();
void forwardRightWheel();
void forwardLeftWheel();
void reverseLeftWheel();
void rotationClockwise();
void rotationAntiClockwise();
void directionForward();
void directionBackward();
void stopMoving();
//* ========================================
char buffer[69];
int quadDec2Count = 0;
int timerInt = 0;
uint16 turnCount = 0;
uint16 maxTurnCount = 25; // clock counts for one complete 90 deg turn


CY_ISR (speedTimer) {
    timerInt = 1;
    // quadDec_M1 used for turning macros
    quadDec2Count = QuadDec_M2_GetCounter();
    QuadDec_M2_SetCounter(0); // reset count
    QuadDec_M2_Start(); // restart counter
    SpeedTimer_ReadStatusRegister(); // clear interrupt
}
int main()
{
// --------------------------------    
// ----- INITIALIZATIONS ----------
    CYGlobalIntEnable;
    // hardware task 2- start motor
    //ADC_Timer_Start();
    // start motor and PWMs
    Clock_PWM_Start();
    PWM_1_Start();
    PWM_1_WritePeriod(100);
    PWM_2_Start();
    PWM_2_WritePeriod(100);
    
    // start quadrature decoders
    QuadDec_M1_Start();
    QuadDec_M2_Start();
    isr_speed_StartEx(speedTimer);
    SpeedTimer_Start();
    SpeedClock_Start();
    
    
    
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
        rotationClockwise();
        rotationAntiClockwise();
        
        
        if(timerInt == 1) {
            // display RPM of M2
            timerInt = 0; // reset flag
            float cps = quadDec2Count/57.00;
            int16 rpm = (int16)(cps*15);
            sprintf(buffer, "%d", rpm);
            usbPutString(buffer);
            usbPutString("rpm ");
        }
        
        if (flag_KB_string == 1)
        {
            usbPutString(line);
            flag_KB_string = 0;

        }           
    }   
}
//* ========================================
// stop moving
void stopMoving() {
    PWM_1_WriteCompare(50);
    PWM_2_WriteCompare(50);
    //usbPutString("stopped\r\n");
}

// rotate anticlockwise 90deg
void rotationAntiClockwise() {
    PWM_1_WriteCompare(35);
    PWM_2_WriteCompare(35);
    
    int quadPulseCount = 0;
    QuadDec_M1_SetCounter(0);
    while(quadPulseCount > -105) {
        quadPulseCount = QuadDec_M1_GetCounter();  
    }
    QuadDec_M1_SetCounter(0);
    //CyDelay(500);
    //stopMoving(); // stop movement, ready for next instruction
}

// rotate clockwise 90deg
void rotationClockwise() {
    PWM_1_WriteCompare(65);
    PWM_2_WriteCompare(65);
    
    int quadPulseCount = 0;
    QuadDec_M1_SetCounter(0);
    while(quadPulseCount < 110) {
        quadPulseCount = QuadDec_M1_GetCounter();
    }
    QuadDec_M2_SetCounter(0);
    //CyDelay(500); // change to quadrature encoder pulses, rather than time delay
    //stopMoving(); // stop movement, ready for next instruction
}

// move forward
void directionForward() {
    PWM_1_WriteCompare(75);
    PWM_2_WriteCompare(25);
}

// move backward
void directionBackward() {
    PWM_1_WriteCompare(25);
    PWM_2_WriteCompare(75);
}


void usbPutString(char *s)
{
// !! Assumes that *s is a string with allocated space >=64 chars     
//  Since USB implementation retricts data packets to 64 chars, this function truncates the
//  length to 62 char (63rd char is a '!')

#ifdef USE_USB     
    while (USBUART_CDCIsReady() == 0);
    s[63]='\0';
    s[62]='!';
    
    // processing for changing duty cycle- look for 'p'' ''xxx'
    if(s[0] == 112 && s[1] == 32) {
        // get percentage and change duty cycle based on user input
        char percentageArray[3] = {s[2], s[3], s[4]};  
        float percentage = (uint8) percentageArray[0] * 100 + (uint8) percentageArray[1] * 10 + (uint8) percentageArray[2];
        // concatenate elements as uint8
        changeDutyCycle(percentage);
    }
    
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
// command interpreter to alter duty cycle via terminal
void changeDutyCycle(float percentage) 
{
    // perform calculaton
    // get period
    float period = 255;
    // changes the compare value to change duty cycle
    float newPeriod = ((percentage/100.0) * period);
    PWM_1_WriteCompare((uint8)newPeriod); 
   
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


/* [] END OF FILE */