/*******************************************************************************
* File Name: SpeedClock.h
* Version 2.20
*
*  Description:
*   Provides the function and constant definitions for the clock component.
*
*  Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_CLOCK_SpeedClock_H)
#define CY_CLOCK_SpeedClock_H

#include <cytypes.h>
#include <cyfitter.h>


/***************************************
* Conditional Compilation Parameters
***************************************/

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component cy_clock_v2_20 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */


/***************************************
*        Function Prototypes
***************************************/

void SpeedClock_Start(void) ;
void SpeedClock_Stop(void) ;

#if(CY_PSOC3 || CY_PSOC5LP)
void SpeedClock_StopBlock(void) ;
#endif /* (CY_PSOC3 || CY_PSOC5LP) */

void SpeedClock_StandbyPower(uint8 state) ;
void SpeedClock_SetDividerRegister(uint16 clkDivider, uint8 restart) 
                                ;
uint16 SpeedClock_GetDividerRegister(void) ;
void SpeedClock_SetModeRegister(uint8 modeBitMask) ;
void SpeedClock_ClearModeRegister(uint8 modeBitMask) ;
uint8 SpeedClock_GetModeRegister(void) ;
void SpeedClock_SetSourceRegister(uint8 clkSource) ;
uint8 SpeedClock_GetSourceRegister(void) ;
#if defined(SpeedClock__CFG3)
void SpeedClock_SetPhaseRegister(uint8 clkPhase) ;
uint8 SpeedClock_GetPhaseRegister(void) ;
#endif /* defined(SpeedClock__CFG3) */

#define SpeedClock_Enable()                       SpeedClock_Start()
#define SpeedClock_Disable()                      SpeedClock_Stop()
#define SpeedClock_SetDivider(clkDivider)         SpeedClock_SetDividerRegister(clkDivider, 1u)
#define SpeedClock_SetDividerValue(clkDivider)    SpeedClock_SetDividerRegister((clkDivider) - 1u, 1u)
#define SpeedClock_SetMode(clkMode)               SpeedClock_SetModeRegister(clkMode)
#define SpeedClock_SetSource(clkSource)           SpeedClock_SetSourceRegister(clkSource)
#if defined(SpeedClock__CFG3)
#define SpeedClock_SetPhase(clkPhase)             SpeedClock_SetPhaseRegister(clkPhase)
#define SpeedClock_SetPhaseValue(clkPhase)        SpeedClock_SetPhaseRegister((clkPhase) + 1u)
#endif /* defined(SpeedClock__CFG3) */


/***************************************
*             Registers
***************************************/

/* Register to enable or disable the clock */
#define SpeedClock_CLKEN              (* (reg8 *) SpeedClock__PM_ACT_CFG)
#define SpeedClock_CLKEN_PTR          ((reg8 *) SpeedClock__PM_ACT_CFG)

/* Register to enable or disable the clock */
#define SpeedClock_CLKSTBY            (* (reg8 *) SpeedClock__PM_STBY_CFG)
#define SpeedClock_CLKSTBY_PTR        ((reg8 *) SpeedClock__PM_STBY_CFG)

/* Clock LSB divider configuration register. */
#define SpeedClock_DIV_LSB            (* (reg8 *) SpeedClock__CFG0)
#define SpeedClock_DIV_LSB_PTR        ((reg8 *) SpeedClock__CFG0)
#define SpeedClock_DIV_PTR            ((reg16 *) SpeedClock__CFG0)

/* Clock MSB divider configuration register. */
#define SpeedClock_DIV_MSB            (* (reg8 *) SpeedClock__CFG1)
#define SpeedClock_DIV_MSB_PTR        ((reg8 *) SpeedClock__CFG1)

/* Mode and source configuration register */
#define SpeedClock_MOD_SRC            (* (reg8 *) SpeedClock__CFG2)
#define SpeedClock_MOD_SRC_PTR        ((reg8 *) SpeedClock__CFG2)

#if defined(SpeedClock__CFG3)
/* Analog clock phase configuration register */
#define SpeedClock_PHASE              (* (reg8 *) SpeedClock__CFG3)
#define SpeedClock_PHASE_PTR          ((reg8 *) SpeedClock__CFG3)
#endif /* defined(SpeedClock__CFG3) */


/**************************************
*       Register Constants
**************************************/

/* Power manager register masks */
#define SpeedClock_CLKEN_MASK         SpeedClock__PM_ACT_MSK
#define SpeedClock_CLKSTBY_MASK       SpeedClock__PM_STBY_MSK

/* CFG2 field masks */
#define SpeedClock_SRC_SEL_MSK        SpeedClock__CFG2_SRC_SEL_MASK
#define SpeedClock_MODE_MASK          (~(SpeedClock_SRC_SEL_MSK))

#if defined(SpeedClock__CFG3)
/* CFG3 phase mask */
#define SpeedClock_PHASE_MASK         SpeedClock__CFG3_PHASE_DLY_MASK
#endif /* defined(SpeedClock__CFG3) */

#endif /* CY_CLOCK_SpeedClock_H */


/* [] END OF FILE */
