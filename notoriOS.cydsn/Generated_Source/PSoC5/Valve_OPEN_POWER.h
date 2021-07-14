/*******************************************************************************
* File Name: Valve_OPEN_POWER.h  
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

#if !defined(CY_PINS_Valve_OPEN_POWER_H) /* Pins Valve_OPEN_POWER_H */
#define CY_PINS_Valve_OPEN_POWER_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Valve_OPEN_POWER_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Valve_OPEN_POWER__PORT == 15 && ((Valve_OPEN_POWER__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Valve_OPEN_POWER_Write(uint8 value);
void    Valve_OPEN_POWER_SetDriveMode(uint8 mode);
uint8   Valve_OPEN_POWER_ReadDataReg(void);
uint8   Valve_OPEN_POWER_Read(void);
void    Valve_OPEN_POWER_SetInterruptMode(uint16 position, uint16 mode);
uint8   Valve_OPEN_POWER_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Valve_OPEN_POWER_SetDriveMode() function.
     *  @{
     */
        #define Valve_OPEN_POWER_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Valve_OPEN_POWER_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Valve_OPEN_POWER_DM_RES_UP          PIN_DM_RES_UP
        #define Valve_OPEN_POWER_DM_RES_DWN         PIN_DM_RES_DWN
        #define Valve_OPEN_POWER_DM_OD_LO           PIN_DM_OD_LO
        #define Valve_OPEN_POWER_DM_OD_HI           PIN_DM_OD_HI
        #define Valve_OPEN_POWER_DM_STRONG          PIN_DM_STRONG
        #define Valve_OPEN_POWER_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Valve_OPEN_POWER_MASK               Valve_OPEN_POWER__MASK
#define Valve_OPEN_POWER_SHIFT              Valve_OPEN_POWER__SHIFT
#define Valve_OPEN_POWER_WIDTH              1u

/* Interrupt constants */
#if defined(Valve_OPEN_POWER__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Valve_OPEN_POWER_SetInterruptMode() function.
     *  @{
     */
        #define Valve_OPEN_POWER_INTR_NONE      (uint16)(0x0000u)
        #define Valve_OPEN_POWER_INTR_RISING    (uint16)(0x0001u)
        #define Valve_OPEN_POWER_INTR_FALLING   (uint16)(0x0002u)
        #define Valve_OPEN_POWER_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Valve_OPEN_POWER_INTR_MASK      (0x01u) 
#endif /* (Valve_OPEN_POWER__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Valve_OPEN_POWER_PS                     (* (reg8 *) Valve_OPEN_POWER__PS)
/* Data Register */
#define Valve_OPEN_POWER_DR                     (* (reg8 *) Valve_OPEN_POWER__DR)
/* Port Number */
#define Valve_OPEN_POWER_PRT_NUM                (* (reg8 *) Valve_OPEN_POWER__PRT) 
/* Connect to Analog Globals */                                                  
#define Valve_OPEN_POWER_AG                     (* (reg8 *) Valve_OPEN_POWER__AG)                       
/* Analog MUX bux enable */
#define Valve_OPEN_POWER_AMUX                   (* (reg8 *) Valve_OPEN_POWER__AMUX) 
/* Bidirectional Enable */                                                        
#define Valve_OPEN_POWER_BIE                    (* (reg8 *) Valve_OPEN_POWER__BIE)
/* Bit-mask for Aliased Register Access */
#define Valve_OPEN_POWER_BIT_MASK               (* (reg8 *) Valve_OPEN_POWER__BIT_MASK)
/* Bypass Enable */
#define Valve_OPEN_POWER_BYP                    (* (reg8 *) Valve_OPEN_POWER__BYP)
/* Port wide control signals */                                                   
#define Valve_OPEN_POWER_CTL                    (* (reg8 *) Valve_OPEN_POWER__CTL)
/* Drive Modes */
#define Valve_OPEN_POWER_DM0                    (* (reg8 *) Valve_OPEN_POWER__DM0) 
#define Valve_OPEN_POWER_DM1                    (* (reg8 *) Valve_OPEN_POWER__DM1)
#define Valve_OPEN_POWER_DM2                    (* (reg8 *) Valve_OPEN_POWER__DM2) 
/* Input Buffer Disable Override */
#define Valve_OPEN_POWER_INP_DIS                (* (reg8 *) Valve_OPEN_POWER__INP_DIS)
/* LCD Common or Segment Drive */
#define Valve_OPEN_POWER_LCD_COM_SEG            (* (reg8 *) Valve_OPEN_POWER__LCD_COM_SEG)
/* Enable Segment LCD */
#define Valve_OPEN_POWER_LCD_EN                 (* (reg8 *) Valve_OPEN_POWER__LCD_EN)
/* Slew Rate Control */
#define Valve_OPEN_POWER_SLW                    (* (reg8 *) Valve_OPEN_POWER__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Valve_OPEN_POWER_PRTDSI__CAPS_SEL       (* (reg8 *) Valve_OPEN_POWER__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Valve_OPEN_POWER_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Valve_OPEN_POWER__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Valve_OPEN_POWER_PRTDSI__OE_SEL0        (* (reg8 *) Valve_OPEN_POWER__PRTDSI__OE_SEL0) 
#define Valve_OPEN_POWER_PRTDSI__OE_SEL1        (* (reg8 *) Valve_OPEN_POWER__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Valve_OPEN_POWER_PRTDSI__OUT_SEL0       (* (reg8 *) Valve_OPEN_POWER__PRTDSI__OUT_SEL0) 
#define Valve_OPEN_POWER_PRTDSI__OUT_SEL1       (* (reg8 *) Valve_OPEN_POWER__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Valve_OPEN_POWER_PRTDSI__SYNC_OUT       (* (reg8 *) Valve_OPEN_POWER__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Valve_OPEN_POWER__SIO_CFG)
    #define Valve_OPEN_POWER_SIO_HYST_EN        (* (reg8 *) Valve_OPEN_POWER__SIO_HYST_EN)
    #define Valve_OPEN_POWER_SIO_REG_HIFREQ     (* (reg8 *) Valve_OPEN_POWER__SIO_REG_HIFREQ)
    #define Valve_OPEN_POWER_SIO_CFG            (* (reg8 *) Valve_OPEN_POWER__SIO_CFG)
    #define Valve_OPEN_POWER_SIO_DIFF           (* (reg8 *) Valve_OPEN_POWER__SIO_DIFF)
#endif /* (Valve_OPEN_POWER__SIO_CFG) */

/* Interrupt Registers */
#if defined(Valve_OPEN_POWER__INTSTAT)
    #define Valve_OPEN_POWER_INTSTAT            (* (reg8 *) Valve_OPEN_POWER__INTSTAT)
    #define Valve_OPEN_POWER_SNAP               (* (reg8 *) Valve_OPEN_POWER__SNAP)
    
	#define Valve_OPEN_POWER_0_INTTYPE_REG 		(* (reg8 *) Valve_OPEN_POWER__0__INTTYPE)
#endif /* (Valve_OPEN_POWER__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Valve_OPEN_POWER_H */


/* [] END OF FILE */
