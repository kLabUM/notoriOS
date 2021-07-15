/*******************************************************************************
* File Name: Valve_POS_blue.h  
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

#if !defined(CY_PINS_Valve_POS_blue_H) /* Pins Valve_POS_blue_H */
#define CY_PINS_Valve_POS_blue_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Valve_POS_blue_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Valve_POS_blue__PORT == 15 && ((Valve_POS_blue__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Valve_POS_blue_Write(uint8 value);
void    Valve_POS_blue_SetDriveMode(uint8 mode);
uint8   Valve_POS_blue_ReadDataReg(void);
uint8   Valve_POS_blue_Read(void);
void    Valve_POS_blue_SetInterruptMode(uint16 position, uint16 mode);
uint8   Valve_POS_blue_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Valve_POS_blue_SetDriveMode() function.
     *  @{
     */
        #define Valve_POS_blue_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Valve_POS_blue_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Valve_POS_blue_DM_RES_UP          PIN_DM_RES_UP
        #define Valve_POS_blue_DM_RES_DWN         PIN_DM_RES_DWN
        #define Valve_POS_blue_DM_OD_LO           PIN_DM_OD_LO
        #define Valve_POS_blue_DM_OD_HI           PIN_DM_OD_HI
        #define Valve_POS_blue_DM_STRONG          PIN_DM_STRONG
        #define Valve_POS_blue_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Valve_POS_blue_MASK               Valve_POS_blue__MASK
#define Valve_POS_blue_SHIFT              Valve_POS_blue__SHIFT
#define Valve_POS_blue_WIDTH              1u

/* Interrupt constants */
#if defined(Valve_POS_blue__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Valve_POS_blue_SetInterruptMode() function.
     *  @{
     */
        #define Valve_POS_blue_INTR_NONE      (uint16)(0x0000u)
        #define Valve_POS_blue_INTR_RISING    (uint16)(0x0001u)
        #define Valve_POS_blue_INTR_FALLING   (uint16)(0x0002u)
        #define Valve_POS_blue_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Valve_POS_blue_INTR_MASK      (0x01u) 
#endif /* (Valve_POS_blue__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Valve_POS_blue_PS                     (* (reg8 *) Valve_POS_blue__PS)
/* Data Register */
#define Valve_POS_blue_DR                     (* (reg8 *) Valve_POS_blue__DR)
/* Port Number */
#define Valve_POS_blue_PRT_NUM                (* (reg8 *) Valve_POS_blue__PRT) 
/* Connect to Analog Globals */                                                  
#define Valve_POS_blue_AG                     (* (reg8 *) Valve_POS_blue__AG)                       
/* Analog MUX bux enable */
#define Valve_POS_blue_AMUX                   (* (reg8 *) Valve_POS_blue__AMUX) 
/* Bidirectional Enable */                                                        
#define Valve_POS_blue_BIE                    (* (reg8 *) Valve_POS_blue__BIE)
/* Bit-mask for Aliased Register Access */
#define Valve_POS_blue_BIT_MASK               (* (reg8 *) Valve_POS_blue__BIT_MASK)
/* Bypass Enable */
#define Valve_POS_blue_BYP                    (* (reg8 *) Valve_POS_blue__BYP)
/* Port wide control signals */                                                   
#define Valve_POS_blue_CTL                    (* (reg8 *) Valve_POS_blue__CTL)
/* Drive Modes */
#define Valve_POS_blue_DM0                    (* (reg8 *) Valve_POS_blue__DM0) 
#define Valve_POS_blue_DM1                    (* (reg8 *) Valve_POS_blue__DM1)
#define Valve_POS_blue_DM2                    (* (reg8 *) Valve_POS_blue__DM2) 
/* Input Buffer Disable Override */
#define Valve_POS_blue_INP_DIS                (* (reg8 *) Valve_POS_blue__INP_DIS)
/* LCD Common or Segment Drive */
#define Valve_POS_blue_LCD_COM_SEG            (* (reg8 *) Valve_POS_blue__LCD_COM_SEG)
/* Enable Segment LCD */
#define Valve_POS_blue_LCD_EN                 (* (reg8 *) Valve_POS_blue__LCD_EN)
/* Slew Rate Control */
#define Valve_POS_blue_SLW                    (* (reg8 *) Valve_POS_blue__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Valve_POS_blue_PRTDSI__CAPS_SEL       (* (reg8 *) Valve_POS_blue__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Valve_POS_blue_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Valve_POS_blue__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Valve_POS_blue_PRTDSI__OE_SEL0        (* (reg8 *) Valve_POS_blue__PRTDSI__OE_SEL0) 
#define Valve_POS_blue_PRTDSI__OE_SEL1        (* (reg8 *) Valve_POS_blue__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Valve_POS_blue_PRTDSI__OUT_SEL0       (* (reg8 *) Valve_POS_blue__PRTDSI__OUT_SEL0) 
#define Valve_POS_blue_PRTDSI__OUT_SEL1       (* (reg8 *) Valve_POS_blue__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Valve_POS_blue_PRTDSI__SYNC_OUT       (* (reg8 *) Valve_POS_blue__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Valve_POS_blue__SIO_CFG)
    #define Valve_POS_blue_SIO_HYST_EN        (* (reg8 *) Valve_POS_blue__SIO_HYST_EN)
    #define Valve_POS_blue_SIO_REG_HIFREQ     (* (reg8 *) Valve_POS_blue__SIO_REG_HIFREQ)
    #define Valve_POS_blue_SIO_CFG            (* (reg8 *) Valve_POS_blue__SIO_CFG)
    #define Valve_POS_blue_SIO_DIFF           (* (reg8 *) Valve_POS_blue__SIO_DIFF)
#endif /* (Valve_POS_blue__SIO_CFG) */

/* Interrupt Registers */
#if defined(Valve_POS_blue__INTSTAT)
    #define Valve_POS_blue_INTSTAT            (* (reg8 *) Valve_POS_blue__INTSTAT)
    #define Valve_POS_blue_SNAP               (* (reg8 *) Valve_POS_blue__SNAP)
    
	#define Valve_POS_blue_0_INTTYPE_REG 		(* (reg8 *) Valve_POS_blue__0__INTTYPE)
#endif /* (Valve_POS_blue__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Valve_POS_blue_H */


/* [] END OF FILE */
