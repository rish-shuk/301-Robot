/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"
#include "defines.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// USBUART Operations
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
    
    // processing for changing duty cycle- look for 'p'' ''xxx'
    if(s[0] == 112 && s[1] == 32) {
        // get percentage and change duty cycle based on user input
        //char percentageArray[3] = {s[2], s[3], s[4]};  
        //float percentage = (uint8) percentageArray[0] * 100 + (uint8) percentageArray[1] * 10 + (uint8) percentageArray[2];
        // concatenate elements as uint8
        //changeDutyCycle(percentage);
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
