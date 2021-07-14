/*******************************************************************************
* File Name: Valve_OPEN_CTRL.h  
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

#if !defined(CY_PINS_Valve_OPEN_CTRL_H) /* Pins Valve_OPEN_CTRL_H */
#define CY_PINS_Valve_OPEN_CTRL_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Valve_OPEN_CTRL_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Valve_OPEN_CTRL__PORT == 15 && ((Valve_OPEN_CTRL__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Valve_OPEN_CTRL_Write(uint8 value);
void    Valve_OPEN_CTRL_SetDriveMode(uint8 mode);
uint8   Valve_OPEN_CTRL_ReadDataReg(void);
uint8   Valve_OPEN_CTRL_Read(void);
void    Valve_OPEN_CTRL_SetInterruptMode(uint16 position, uint16 mode);
uint8   Valve_OPEN_CTRL_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Valve_OPEN_CTRL_SetDriveMode() function.
     *  @{
     */
        #define Valve_OPEN_CTRL_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Valve_OPEN_CTRL_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Valve_OPEN_CTRL_DM_RES_UP          PIN_DM_RES_UP
        #define Valve_OPEN_CTRL_DM_RES_DWN         PIN_DM_RES_DWN
        #define Valve_OPEN_CTRL_DM_OD_LO           PIN_DM_OD_LO
        #define Valve_OPEN_CTRL_DM_OD_HI           PIN_DM_OD_HI
        #define Valve_OPEN_CTRL_DM_STRONG          PIN_DM_STRONG
        #define Valve_OPEN_CTRL_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Valve_OPEN_CTRL_MASK               Valve_OPEN_CTRL__MASK
#define Valve_OPEN_CTRL_SHIFT              Valve_OPEN_CTRL__SHIFT
#define Valve_OPEN_CTRL_WIDTH              1u

/* Interrupt constants */
#if defined(Valve_OPEN_CTRL__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Valve_OPEN_CTRL_SetInterruptMode() function.
     *  @{
     */
        #define Valve_OPEN_CTRL_INTR_NONE      (uint16)(0x0000u)
        #define Valve_OPEN_CTRL_INTR_RISING    (uint16)(0x0001u)
        #define Valve_OPEN_CTRL_INTR_FALLING   (uint16)(0x0002u)
        #define Valve_OPEN_CTRL_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Valve_OPEN_CTRL_INTR_MASK      (0x01u) 
#endif /* (Valve_OPEN_CTRL__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Valve_OPEN_CTRL_PS                     (* (reg8 *) Valve_OPEN_CTRL__PS)
/* Data Register */
#define Valve_OPEN_CTRL_DR                     (* (reg8 *) Valve_OPEN_CTRL__DR)
/* Port Number */
#define Valve_OPEN_CTRL_PRT_NUM                (* (reg8 *) Valve_OPEN_CTRL__PRT) 
/* Connect to Analog Globals */                                                  
#define Valve_OPEN_CTRL_AG                     (* (reg8 *) Valve_OPEN_CTRL__AG)                       
/* Analog MUX bux enable */
#define Valve_OPEN_CTRL_AMUX                   (* (reg8 *) Valve_OPEN_CTRL__AMUX) 
/* Bidirectional Enable */                                                        
#define Valve_OPEN_CTRL_BIE                    (* (reg8 *) Valve_OPEN_CTRL__BIE)
/* Bit-mask for Aliased Register Access */
#define Valve_OPEN_CTRL_BIT_MASK               (* (reg8 *) Valve_OPEN_CTRL__BIT_MASK)
/* Bypass Enable */
#define Valve_OPEN_CTRL_BYP                    (* (reg8 *) Valve_OPEN_CTRL__BYP)
/* Port wide control signals */                                                   
#define Valve_OPEN_CTRL_CTL                    (* (reg8 *) Valve_OPEN_CTRL__CTL)
/* Drive Modes */
#define Valve_OPEN_CTRL_DM0                    (* (reg8 *) Valve_OPEN_CTRL__DM0) 
#define Valve_OPEN_CTRL_DM1                    (* (reg8 *) Valve_OPEN_CTRL__DM1)
#define Valve_OPEN_CTRL_DM2                    (* (reg8 *) Valve_OPEN_CTRL__DM2) 
/* Input Buffer Disable Override */
#define Valve_OPEN_CTRL_INP_DIS                (* (reg8 *) Valve_OPEN_CTRL__INP_DIS)
/* LCD Common or Segment Drive */
#define Valve_OPEN_CTRL_LCD_COM_SEG            (* (reg8 *) Valve_OPEN_CTRL__LCD_COM_SEG)
/* Enable Segment LCD */
#define Valve_OPEN_CTRL_LCD_EN                 (* (reg8 *) Valve_OPEN_CTRL__LCD_EN)
/* Slew Rate Control */
#define Valve_OPEN_CTRL_SLW                    (* (reg8 *) Valve_OPEN_CTRL__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Valve_OPEN_CTRL_PRTDSI__CAPS_SEL       (* (reg8 *) Valve_OPEN_CTRL__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Valve_OPEN_CTRL_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Valve_OPEN_CTRL__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Valve_OPEN_CTRL_PRTDSI__OE_SEL0        (* (reg8 *) Valve_OPEN_CTRL__PRTDSI__OE_SEL0) 
#define Valve_OPEN_CTRL_PRTDSI__OE_SEL1        (* (reg8 *) Valve_OPEN_CTRL__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Valve_OPEN_CTRL_PRTDSI__OUT_SEL0       (* (reg8 *) Valve_OPEN_CTRL__PRTDSI__OUT_SEL0) 
#define Valve_OPEN_CTRL_PRTDSI__OUT_SEL1       (* (reg8 *) Valve_OPEN_CTRL__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Valve_OPEN_CTRL_PRTDSI__SYNC_OUT       (* (reg8 *) Valve_OPEN_CTRL__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Valve_OPEN_CTRL__SIO_CFG)
    #define Valve_OPEN_CTRL_SIO_HYST_EN        (* (reg8 *) Valve_OPEN_CTRL__SIO_HYST_EN)
    #define Valve_OPEN_CTRL_SIO_REG_HIFREQ     (* (reg8 *) Valve_OPEN_CTRL__SIO_REG_HIFREQ)
    #define Valve_OPEN_CTRL_SIO_CFG            (* (reg8 *) Valve_OPEN_CTRL__SIO_CFG)
    #define Valve_OPEN_CTRL_SIO_DIFF           (* (reg8 *) Valve_OPEN_CTRL__SIO_DIFF)
#endif /* (Valve_OPEN_CTRL__SIO_CFG) */

/* Interrupt Registers */
#if defined(Valve_OPEN_CTRL__INTSTAT)
    #define Valve_OPEN_CTRL_INTSTAT            (* (reg8 *) Valve_OPEN_CTRL__INTSTAT)
    #define Valve_OPEN_CTRL_SNAP               (* (reg8 *) Valve_OPEN_CTRL__SNAP)
    
	#define Valve_OPEN_CTRL_0_INTTYPE_REG 		(* (reg8 *) Valve_OPEN_CTRL__0__INTTYPE)
#endif /* (Valve_OPEN_CTRL__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Valve_OPEN_CTRL_H */


/* [] END OF FILE */
