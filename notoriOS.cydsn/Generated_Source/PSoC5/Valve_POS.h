/*******************************************************************************
* File Name: Valve_POS.h  
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

#if !defined(CY_PINS_Valve_POS_H) /* Pins Valve_POS_H */
#define CY_PINS_Valve_POS_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Valve_POS_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Valve_POS__PORT == 15 && ((Valve_POS__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Valve_POS_Write(uint8 value);
void    Valve_POS_SetDriveMode(uint8 mode);
uint8   Valve_POS_ReadDataReg(void);
uint8   Valve_POS_Read(void);
void    Valve_POS_SetInterruptMode(uint16 position, uint16 mode);
uint8   Valve_POS_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Valve_POS_SetDriveMode() function.
     *  @{
     */
        #define Valve_POS_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Valve_POS_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Valve_POS_DM_RES_UP          PIN_DM_RES_UP
        #define Valve_POS_DM_RES_DWN         PIN_DM_RES_DWN
        #define Valve_POS_DM_OD_LO           PIN_DM_OD_LO
        #define Valve_POS_DM_OD_HI           PIN_DM_OD_HI
        #define Valve_POS_DM_STRONG          PIN_DM_STRONG
        #define Valve_POS_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Valve_POS_MASK               Valve_POS__MASK
#define Valve_POS_SHIFT              Valve_POS__SHIFT
#define Valve_POS_WIDTH              1u

/* Interrupt constants */
#if defined(Valve_POS__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Valve_POS_SetInterruptMode() function.
     *  @{
     */
        #define Valve_POS_INTR_NONE      (uint16)(0x0000u)
        #define Valve_POS_INTR_RISING    (uint16)(0x0001u)
        #define Valve_POS_INTR_FALLING   (uint16)(0x0002u)
        #define Valve_POS_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Valve_POS_INTR_MASK      (0x01u) 
#endif /* (Valve_POS__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Valve_POS_PS                     (* (reg8 *) Valve_POS__PS)
/* Data Register */
#define Valve_POS_DR                     (* (reg8 *) Valve_POS__DR)
/* Port Number */
#define Valve_POS_PRT_NUM                (* (reg8 *) Valve_POS__PRT) 
/* Connect to Analog Globals */                                                  
#define Valve_POS_AG                     (* (reg8 *) Valve_POS__AG)                       
/* Analog MUX bux enable */
#define Valve_POS_AMUX                   (* (reg8 *) Valve_POS__AMUX) 
/* Bidirectional Enable */                                                        
#define Valve_POS_BIE                    (* (reg8 *) Valve_POS__BIE)
/* Bit-mask for Aliased Register Access */
#define Valve_POS_BIT_MASK               (* (reg8 *) Valve_POS__BIT_MASK)
/* Bypass Enable */
#define Valve_POS_BYP                    (* (reg8 *) Valve_POS__BYP)
/* Port wide control signals */                                                   
#define Valve_POS_CTL                    (* (reg8 *) Valve_POS__CTL)
/* Drive Modes */
#define Valve_POS_DM0                    (* (reg8 *) Valve_POS__DM0) 
#define Valve_POS_DM1                    (* (reg8 *) Valve_POS__DM1)
#define Valve_POS_DM2                    (* (reg8 *) Valve_POS__DM2) 
/* Input Buffer Disable Override */
#define Valve_POS_INP_DIS                (* (reg8 *) Valve_POS__INP_DIS)
/* LCD Common or Segment Drive */
#define Valve_POS_LCD_COM_SEG            (* (reg8 *) Valve_POS__LCD_COM_SEG)
/* Enable Segment LCD */
#define Valve_POS_LCD_EN                 (* (reg8 *) Valve_POS__LCD_EN)
/* Slew Rate Control */
#define Valve_POS_SLW                    (* (reg8 *) Valve_POS__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Valve_POS_PRTDSI__CAPS_SEL       (* (reg8 *) Valve_POS__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Valve_POS_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Valve_POS__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Valve_POS_PRTDSI__OE_SEL0        (* (reg8 *) Valve_POS__PRTDSI__OE_SEL0) 
#define Valve_POS_PRTDSI__OE_SEL1        (* (reg8 *) Valve_POS__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Valve_POS_PRTDSI__OUT_SEL0       (* (reg8 *) Valve_POS__PRTDSI__OUT_SEL0) 
#define Valve_POS_PRTDSI__OUT_SEL1       (* (reg8 *) Valve_POS__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Valve_POS_PRTDSI__SYNC_OUT       (* (reg8 *) Valve_POS__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Valve_POS__SIO_CFG)
    #define Valve_POS_SIO_HYST_EN        (* (reg8 *) Valve_POS__SIO_HYST_EN)
    #define Valve_POS_SIO_REG_HIFREQ     (* (reg8 *) Valve_POS__SIO_REG_HIFREQ)
    #define Valve_POS_SIO_CFG            (* (reg8 *) Valve_POS__SIO_CFG)
    #define Valve_POS_SIO_DIFF           (* (reg8 *) Valve_POS__SIO_DIFF)
#endif /* (Valve_POS__SIO_CFG) */

/* Interrupt Registers */
#if defined(Valve_POS__INTSTAT)
    #define Valve_POS_INTSTAT            (* (reg8 *) Valve_POS__INTSTAT)
    #define Valve_POS_SNAP               (* (reg8 *) Valve_POS__SNAP)
    
	#define Valve_POS_0_INTTYPE_REG 		(* (reg8 *) Valve_POS__0__INTTYPE)
#endif /* (Valve_POS__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Valve_POS_H */


/* [] END OF FILE */
