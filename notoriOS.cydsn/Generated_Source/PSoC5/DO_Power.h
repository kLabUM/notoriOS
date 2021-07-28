/*******************************************************************************
* File Name: DO_Power.h  
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

#if !defined(CY_PINS_DO_Power_H) /* Pins DO_Power_H */
#define CY_PINS_DO_Power_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "DO_Power_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 DO_Power__PORT == 15 && ((DO_Power__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    DO_Power_Write(uint8 value);
void    DO_Power_SetDriveMode(uint8 mode);
uint8   DO_Power_ReadDataReg(void);
uint8   DO_Power_Read(void);
void    DO_Power_SetInterruptMode(uint16 position, uint16 mode);
uint8   DO_Power_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the DO_Power_SetDriveMode() function.
     *  @{
     */
        #define DO_Power_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define DO_Power_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define DO_Power_DM_RES_UP          PIN_DM_RES_UP
        #define DO_Power_DM_RES_DWN         PIN_DM_RES_DWN
        #define DO_Power_DM_OD_LO           PIN_DM_OD_LO
        #define DO_Power_DM_OD_HI           PIN_DM_OD_HI
        #define DO_Power_DM_STRONG          PIN_DM_STRONG
        #define DO_Power_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define DO_Power_MASK               DO_Power__MASK
#define DO_Power_SHIFT              DO_Power__SHIFT
#define DO_Power_WIDTH              1u

/* Interrupt constants */
#if defined(DO_Power__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in DO_Power_SetInterruptMode() function.
     *  @{
     */
        #define DO_Power_INTR_NONE      (uint16)(0x0000u)
        #define DO_Power_INTR_RISING    (uint16)(0x0001u)
        #define DO_Power_INTR_FALLING   (uint16)(0x0002u)
        #define DO_Power_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define DO_Power_INTR_MASK      (0x01u) 
#endif /* (DO_Power__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define DO_Power_PS                     (* (reg8 *) DO_Power__PS)
/* Data Register */
#define DO_Power_DR                     (* (reg8 *) DO_Power__DR)
/* Port Number */
#define DO_Power_PRT_NUM                (* (reg8 *) DO_Power__PRT) 
/* Connect to Analog Globals */                                                  
#define DO_Power_AG                     (* (reg8 *) DO_Power__AG)                       
/* Analog MUX bux enable */
#define DO_Power_AMUX                   (* (reg8 *) DO_Power__AMUX) 
/* Bidirectional Enable */                                                        
#define DO_Power_BIE                    (* (reg8 *) DO_Power__BIE)
/* Bit-mask for Aliased Register Access */
#define DO_Power_BIT_MASK               (* (reg8 *) DO_Power__BIT_MASK)
/* Bypass Enable */
#define DO_Power_BYP                    (* (reg8 *) DO_Power__BYP)
/* Port wide control signals */                                                   
#define DO_Power_CTL                    (* (reg8 *) DO_Power__CTL)
/* Drive Modes */
#define DO_Power_DM0                    (* (reg8 *) DO_Power__DM0) 
#define DO_Power_DM1                    (* (reg8 *) DO_Power__DM1)
#define DO_Power_DM2                    (* (reg8 *) DO_Power__DM2) 
/* Input Buffer Disable Override */
#define DO_Power_INP_DIS                (* (reg8 *) DO_Power__INP_DIS)
/* LCD Common or Segment Drive */
#define DO_Power_LCD_COM_SEG            (* (reg8 *) DO_Power__LCD_COM_SEG)
/* Enable Segment LCD */
#define DO_Power_LCD_EN                 (* (reg8 *) DO_Power__LCD_EN)
/* Slew Rate Control */
#define DO_Power_SLW                    (* (reg8 *) DO_Power__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define DO_Power_PRTDSI__CAPS_SEL       (* (reg8 *) DO_Power__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define DO_Power_PRTDSI__DBL_SYNC_IN    (* (reg8 *) DO_Power__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define DO_Power_PRTDSI__OE_SEL0        (* (reg8 *) DO_Power__PRTDSI__OE_SEL0) 
#define DO_Power_PRTDSI__OE_SEL1        (* (reg8 *) DO_Power__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define DO_Power_PRTDSI__OUT_SEL0       (* (reg8 *) DO_Power__PRTDSI__OUT_SEL0) 
#define DO_Power_PRTDSI__OUT_SEL1       (* (reg8 *) DO_Power__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define DO_Power_PRTDSI__SYNC_OUT       (* (reg8 *) DO_Power__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(DO_Power__SIO_CFG)
    #define DO_Power_SIO_HYST_EN        (* (reg8 *) DO_Power__SIO_HYST_EN)
    #define DO_Power_SIO_REG_HIFREQ     (* (reg8 *) DO_Power__SIO_REG_HIFREQ)
    #define DO_Power_SIO_CFG            (* (reg8 *) DO_Power__SIO_CFG)
    #define DO_Power_SIO_DIFF           (* (reg8 *) DO_Power__SIO_DIFF)
#endif /* (DO_Power__SIO_CFG) */

/* Interrupt Registers */
#if defined(DO_Power__INTSTAT)
    #define DO_Power_INTSTAT            (* (reg8 *) DO_Power__INTSTAT)
    #define DO_Power_SNAP               (* (reg8 *) DO_Power__SNAP)
    
	#define DO_Power_0_INTTYPE_REG 		(* (reg8 *) DO_Power__0__INTTYPE)
#endif /* (DO_Power__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_DO_Power_H */


/* [] END OF FILE */
