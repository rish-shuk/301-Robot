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
#ifndef USBUART_H
#define USBUART_H

#include "project.h"
#include "defines.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
    
    
void usbPutString(char *s);
void usbPutChar(char c);
void changeDutyCycle(float percentage);
void handle_usb();


#endif // USBUART_H
