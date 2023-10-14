/*******************************************************************************
* File Name: S3_detected.h
* Version 1.70
*
*  Description:
*   Provides the function definitions for the Interrupt Controller.
*
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/
#if !defined(CY_ISR_S3_detected_H)
#define CY_ISR_S3_detected_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void S3_detected_Start(void);
void S3_detected_StartEx(cyisraddress address);
void S3_detected_Stop(void);

CY_ISR_PROTO(S3_detected_Interrupt);

void S3_detected_SetVector(cyisraddress address);
cyisraddress S3_detected_GetVector(void);

void S3_detected_SetPriority(uint8 priority);
uint8 S3_detected_GetPriority(void);

void S3_detected_Enable(void);
uint8 S3_detected_GetState(void);
void S3_detected_Disable(void);

void S3_detected_SetPending(void);
void S3_detected_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the S3_detected ISR. */
#define S3_detected_INTC_VECTOR            ((reg32 *) S3_detected__INTC_VECT)

/* Address of the S3_detected ISR priority. */
#define S3_detected_INTC_PRIOR             ((reg8 *) S3_detected__INTC_PRIOR_REG)

/* Priority of the S3_detected interrupt. */
#define S3_detected_INTC_PRIOR_NUMBER      S3_detected__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable S3_detected interrupt. */
#define S3_detected_INTC_SET_EN            ((reg32 *) S3_detected__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the S3_detected interrupt. */
#define S3_detected_INTC_CLR_EN            ((reg32 *) S3_detected__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the S3_detected interrupt state to pending. */
#define S3_detected_INTC_SET_PD            ((reg32 *) S3_detected__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the S3_detected interrupt. */
#define S3_detected_INTC_CLR_PD            ((reg32 *) S3_detected__INTC_CLR_PD_REG)


#endif /* CY_ISR_S3_detected_H */


/* [] END OF FILE */
