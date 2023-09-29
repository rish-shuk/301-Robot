/*******************************************************************************
* File Name: Sout.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_Sout_H) /* Pins Sout_H */
#define CY_PINS_Sout_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Sout_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Sout__PORT == 15 && ((Sout__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Sout_Write(uint8 value);
void    Sout_SetDriveMode(uint8 mode);
uint8   Sout_ReadDataReg(void);
uint8   Sout_Read(void);
void    Sout_SetInterruptMode(uint16 position, uint16 mode);
uint8   Sout_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Sout_SetDriveMode() function.
     *  @{
     */
        #define Sout_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Sout_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Sout_DM_RES_UP          PIN_DM_RES_UP
        #define Sout_DM_RES_DWN         PIN_DM_RES_DWN
        #define Sout_DM_OD_LO           PIN_DM_OD_LO
        #define Sout_DM_OD_HI           PIN_DM_OD_HI
        #define Sout_DM_STRONG          PIN_DM_STRONG
        #define Sout_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Sout_MASK               Sout__MASK
#define Sout_SHIFT              Sout__SHIFT
#define Sout_WIDTH              1u

/* Interrupt constants */
#if defined(Sout__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Sout_SetInterruptMode() function.
     *  @{
     */
        #define Sout_INTR_NONE      (uint16)(0x0000u)
        #define Sout_INTR_RISING    (uint16)(0x0001u)
        #define Sout_INTR_FALLING   (uint16)(0x0002u)
        #define Sout_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Sout_INTR_MASK      (0x01u) 
#endif /* (Sout__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Sout_PS                     (* (reg8 *) Sout__PS)
/* Data Register */
#define Sout_DR                     (* (reg8 *) Sout__DR)
/* Port Number */
#define Sout_PRT_NUM                (* (reg8 *) Sout__PRT) 
/* Connect to Analog Globals */                                                  
#define Sout_AG                     (* (reg8 *) Sout__AG)                       
/* Analog MUX bux enable */
#define Sout_AMUX                   (* (reg8 *) Sout__AMUX) 
/* Bidirectional Enable */                                                        
#define Sout_BIE                    (* (reg8 *) Sout__BIE)
/* Bit-mask for Aliased Register Access */
#define Sout_BIT_MASK               (* (reg8 *) Sout__BIT_MASK)
/* Bypass Enable */
#define Sout_BYP                    (* (reg8 *) Sout__BYP)
/* Port wide control signals */                                                   
#define Sout_CTL                    (* (reg8 *) Sout__CTL)
/* Drive Modes */
#define Sout_DM0                    (* (reg8 *) Sout__DM0) 
#define Sout_DM1                    (* (reg8 *) Sout__DM1)
#define Sout_DM2                    (* (reg8 *) Sout__DM2) 
/* Input Buffer Disable Override */
#define Sout_INP_DIS                (* (reg8 *) Sout__INP_DIS)
/* LCD Common or Segment Drive */
#define Sout_LCD_COM_SEG            (* (reg8 *) Sout__LCD_COM_SEG)
/* Enable Segment LCD */
#define Sout_LCD_EN                 (* (reg8 *) Sout__LCD_EN)
/* Slew Rate Control */
#define Sout_SLW                    (* (reg8 *) Sout__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Sout_PRTDSI__CAPS_SEL       (* (reg8 *) Sout__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Sout_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Sout__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Sout_PRTDSI__OE_SEL0        (* (reg8 *) Sout__PRTDSI__OE_SEL0) 
#define Sout_PRTDSI__OE_SEL1        (* (reg8 *) Sout__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Sout_PRTDSI__OUT_SEL0       (* (reg8 *) Sout__PRTDSI__OUT_SEL0) 
#define Sout_PRTDSI__OUT_SEL1       (* (reg8 *) Sout__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Sout_PRTDSI__SYNC_OUT       (* (reg8 *) Sout__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Sout__SIO_CFG)
    #define Sout_SIO_HYST_EN        (* (reg8 *) Sout__SIO_HYST_EN)
    #define Sout_SIO_REG_HIFREQ     (* (reg8 *) Sout__SIO_REG_HIFREQ)
    #define Sout_SIO_CFG            (* (reg8 *) Sout__SIO_CFG)
    #define Sout_SIO_DIFF           (* (reg8 *) Sout__SIO_DIFF)
#endif /* (Sout__SIO_CFG) */

/* Interrupt Registers */
#if defined(Sout__INTSTAT)
    #define Sout_INTSTAT            (* (reg8 *) Sout__INTSTAT)
    #define Sout_SNAP               (* (reg8 *) Sout__SNAP)
    
	#define Sout_0_INTTYPE_REG 		(* (reg8 *) Sout__0__INTTYPE)
#endif /* (Sout__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Sout_H */


/* [] END OF FILE */
