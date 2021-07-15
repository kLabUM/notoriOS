/*******************************************************************************
* File Name: Valve_POS_brown.h  
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

#if !defined(CY_PINS_Valve_POS_brown_H) /* Pins Valve_POS_brown_H */
#define CY_PINS_Valve_POS_brown_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Valve_POS_brown_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Valve_POS_brown__PORT == 15 && ((Valve_POS_brown__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Valve_POS_brown_Write(uint8 value);
void    Valve_POS_brown_SetDriveMode(uint8 mode);
uint8   Valve_POS_brown_ReadDataReg(void);
uint8   Valve_POS_brown_Read(void);
void    Valve_POS_brown_SetInterruptMode(uint16 position, uint16 mode);
uint8   Valve_POS_brown_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Valve_POS_brown_SetDriveMode() function.
     *  @{
     */
        #define Valve_POS_brown_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Valve_POS_brown_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Valve_POS_brown_DM_RES_UP          PIN_DM_RES_UP
        #define Valve_POS_brown_DM_RES_DWN         PIN_DM_RES_DWN
        #define Valve_POS_brown_DM_OD_LO           PIN_DM_OD_LO
        #define Valve_POS_brown_DM_OD_HI           PIN_DM_OD_HI
        #define Valve_POS_brown_DM_STRONG          PIN_DM_STRONG
        #define Valve_POS_brown_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Valve_POS_brown_MASK               Valve_POS_brown__MASK
#define Valve_POS_brown_SHIFT              Valve_POS_brown__SHIFT
#define Valve_POS_brown_WIDTH              1u

/* Interrupt constants */
#if defined(Valve_POS_brown__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Valve_POS_brown_SetInterruptMode() function.
     *  @{
     */
        #define Valve_POS_brown_INTR_NONE      (uint16)(0x0000u)
        #define Valve_POS_brown_INTR_RISING    (uint16)(0x0001u)
        #define Valve_POS_brown_INTR_FALLING   (uint16)(0x0002u)
        #define Valve_POS_brown_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Valve_POS_brown_INTR_MASK      (0x01u) 
#endif /* (Valve_POS_brown__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Valve_POS_brown_PS                     (* (reg8 *) Valve_POS_brown__PS)
/* Data Register */
#define Valve_POS_brown_DR                     (* (reg8 *) Valve_POS_brown__DR)
/* Port Number */
#define Valve_POS_brown_PRT_NUM                (* (reg8 *) Valve_POS_brown__PRT) 
/* Connect to Analog Globals */                                                  
#define Valve_POS_brown_AG                     (* (reg8 *) Valve_POS_brown__AG)                       
/* Analog MUX bux enable */
#define Valve_POS_brown_AMUX                   (* (reg8 *) Valve_POS_brown__AMUX) 
/* Bidirectional Enable */                                                        
#define Valve_POS_brown_BIE                    (* (reg8 *) Valve_POS_brown__BIE)
/* Bit-mask for Aliased Register Access */
#define Valve_POS_brown_BIT_MASK               (* (reg8 *) Valve_POS_brown__BIT_MASK)
/* Bypass Enable */
#define Valve_POS_brown_BYP                    (* (reg8 *) Valve_POS_brown__BYP)
/* Port wide control signals */                                                   
#define Valve_POS_brown_CTL                    (* (reg8 *) Valve_POS_brown__CTL)
/* Drive Modes */
#define Valve_POS_brown_DM0                    (* (reg8 *) Valve_POS_brown__DM0) 
#define Valve_POS_brown_DM1                    (* (reg8 *) Valve_POS_brown__DM1)
#define Valve_POS_brown_DM2                    (* (reg8 *) Valve_POS_brown__DM2) 
/* Input Buffer Disable Override */
#define Valve_POS_brown_INP_DIS                (* (reg8 *) Valve_POS_brown__INP_DIS)
/* LCD Common or Segment Drive */
#define Valve_POS_brown_LCD_COM_SEG            (* (reg8 *) Valve_POS_brown__LCD_COM_SEG)
/* Enable Segment LCD */
#define Valve_POS_brown_LCD_EN                 (* (reg8 *) Valve_POS_brown__LCD_EN)
/* Slew Rate Control */
#define Valve_POS_brown_SLW                    (* (reg8 *) Valve_POS_brown__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Valve_POS_brown_PRTDSI__CAPS_SEL       (* (reg8 *) Valve_POS_brown__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Valve_POS_brown_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Valve_POS_brown__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Valve_POS_brown_PRTDSI__OE_SEL0        (* (reg8 *) Valve_POS_brown__PRTDSI__OE_SEL0) 
#define Valve_POS_brown_PRTDSI__OE_SEL1        (* (reg8 *) Valve_POS_brown__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Valve_POS_brown_PRTDSI__OUT_SEL0       (* (reg8 *) Valve_POS_brown__PRTDSI__OUT_SEL0) 
#define Valve_POS_brown_PRTDSI__OUT_SEL1       (* (reg8 *) Valve_POS_brown__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Valve_POS_brown_PRTDSI__SYNC_OUT       (* (reg8 *) Valve_POS_brown__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Valve_POS_brown__SIO_CFG)
    #define Valve_POS_brown_SIO_HYST_EN        (* (reg8 *) Valve_POS_brown__SIO_HYST_EN)
    #define Valve_POS_brown_SIO_REG_HIFREQ     (* (reg8 *) Valve_POS_brown__SIO_REG_HIFREQ)
    #define Valve_POS_brown_SIO_CFG            (* (reg8 *) Valve_POS_brown__SIO_CFG)
    #define Valve_POS_brown_SIO_DIFF           (* (reg8 *) Valve_POS_brown__SIO_DIFF)
#endif /* (Valve_POS_brown__SIO_CFG) */

/* Interrupt Registers */
#if defined(Valve_POS_brown__INTSTAT)
    #define Valve_POS_brown_INTSTAT            (* (reg8 *) Valve_POS_brown__INTSTAT)
    #define Valve_POS_brown_SNAP               (* (reg8 *) Valve_POS_brown__SNAP)
    
	#define Valve_POS_brown_0_INTTYPE_REG 		(* (reg8 *) Valve_POS_brown__0__INTTYPE)
#endif /* (Valve_POS_brown__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Valve_POS_brown_H */


/* [] END OF FILE */
