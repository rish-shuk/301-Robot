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
#ifndef MY_HEADER_FILE_H
#define MY_HEADER_FILE_H

#include "project.h"
#include "defines.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
    
#ifdef USE_USB

void usbPutString(char *s);
void usbPutChar(char c);
void changeDutyCycle(float percentage);
void handle_usb();

#endif // USE_USB

#endif // MY_HEADER_FILE_H
