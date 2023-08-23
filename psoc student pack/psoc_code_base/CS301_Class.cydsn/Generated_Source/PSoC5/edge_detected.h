/*******************************************************************************
* File Name: edge_detected.h
* Version 1.71
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
#if !defined(CY_ISR_edge_detected_H)
#define CY_ISR_edge_detected_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void edge_detected_Start(void);
void edge_detected_StartEx(cyisraddress address);
void edge_detected_Stop(void);

CY_ISR_PROTO(edge_detected_Interrupt);

void edge_detected_SetVector(cyisraddress address);
cyisraddress edge_detected_GetVector(void);

void edge_detected_SetPriority(uint8 priority);
uint8 edge_detected_GetPriority(void);

void edge_detected_Enable(void);
uint8 edge_detected_GetState(void);
void edge_detected_Disable(void);

void edge_detected_SetPending(void);
void edge_detected_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the edge_detected ISR. */
#define edge_detected_INTC_VECTOR            ((reg32 *) edge_detected__INTC_VECT)

/* Address of the edge_detected ISR priority. */
#define edge_detected_INTC_PRIOR             ((reg8 *) edge_detected__INTC_PRIOR_REG)

/* Priority of the edge_detected interrupt. */
#define edge_detected_INTC_PRIOR_NUMBER      edge_detected__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable edge_detected interrupt. */
#define edge_detected_INTC_SET_EN            ((reg32 *) edge_detected__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the edge_detected interrupt. */
#define edge_detected_INTC_CLR_EN            ((reg32 *) edge_detected__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the edge_detected interrupt state to pending. */
#define edge_detected_INTC_SET_PD            ((reg32 *) edge_detected__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the edge_detected interrupt. */
#define edge_detected_INTC_CLR_PD            ((reg32 *) edge_detected__INTC_CLR_PD_REG)


#endif /* CY_ISR_edge_detected_H */


/* [] END OF FILE */
