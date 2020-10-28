/*******************************************************************************
* File Name: Battery_Voltage_Enable.h  
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

#if !defined(CY_PINS_Battery_Voltage_Enable_H) /* Pins Battery_Voltage_Enable_H */
#define CY_PINS_Battery_Voltage_Enable_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Battery_Voltage_Enable_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Battery_Voltage_Enable__PORT == 15 && ((Battery_Voltage_Enable__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Battery_Voltage_Enable_Write(uint8 value);
void    Battery_Voltage_Enable_SetDriveMode(uint8 mode);
uint8   Battery_Voltage_Enable_ReadDataReg(void);
uint8   Battery_Voltage_Enable_Read(void);
void    Battery_Voltage_Enable_SetInterruptMode(uint16 position, uint16 mode);
uint8   Battery_Voltage_Enable_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Battery_Voltage_Enable_SetDriveMode() function.
     *  @{
     */
        #define Battery_Voltage_Enable_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Battery_Voltage_Enable_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Battery_Voltage_Enable_DM_RES_UP          PIN_DM_RES_UP
        #define Battery_Voltage_Enable_DM_RES_DWN         PIN_DM_RES_DWN
        #define Battery_Voltage_Enable_DM_OD_LO           PIN_DM_OD_LO
        #define Battery_Voltage_Enable_DM_OD_HI           PIN_DM_OD_HI
        #define Battery_Voltage_Enable_DM_STRONG          PIN_DM_STRONG
        #define Battery_Voltage_Enable_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Battery_Voltage_Enable_MASK               Battery_Voltage_Enable__MASK
#define Battery_Voltage_Enable_SHIFT              Battery_Voltage_Enable__SHIFT
#define Battery_Voltage_Enable_WIDTH              1u

/* Interrupt constants */
#if defined(Battery_Voltage_Enable__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Battery_Voltage_Enable_SetInterruptMode() function.
     *  @{
     */
        #define Battery_Voltage_Enable_INTR_NONE      (uint16)(0x0000u)
        #define Battery_Voltage_Enable_INTR_RISING    (uint16)(0x0001u)
        #define Battery_Voltage_Enable_INTR_FALLING   (uint16)(0x0002u)
        #define Battery_Voltage_Enable_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Battery_Voltage_Enable_INTR_MASK      (0x01u) 
#endif /* (Battery_Voltage_Enable__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Battery_Voltage_Enable_PS                     (* (reg8 *) Battery_Voltage_Enable__PS)
/* Data Register */
#define Battery_Voltage_Enable_DR                     (* (reg8 *) Battery_Voltage_Enable__DR)
/* Port Number */
#define Battery_Voltage_Enable_PRT_NUM                (* (reg8 *) Battery_Voltage_Enable__PRT) 
/* Connect to Analog Globals */                                                  
#define Battery_Voltage_Enable_AG                     (* (reg8 *) Battery_Voltage_Enable__AG)                       
/* Analog MUX bux enable */
#define Battery_Voltage_Enable_AMUX                   (* (reg8 *) Battery_Voltage_Enable__AMUX) 
/* Bidirectional Enable */                                                        
#define Battery_Voltage_Enable_BIE                    (* (reg8 *) Battery_Voltage_Enable__BIE)
/* Bit-mask for Aliased Register Access */
#define Battery_Voltage_Enable_BIT_MASK               (* (reg8 *) Battery_Voltage_Enable__BIT_MASK)
/* Bypass Enable */
#define Battery_Voltage_Enable_BYP                    (* (reg8 *) Battery_Voltage_Enable__BYP)
/* Port wide control signals */                                                   
#define Battery_Voltage_Enable_CTL                    (* (reg8 *) Battery_Voltage_Enable__CTL)
/* Drive Modes */
#define Battery_Voltage_Enable_DM0                    (* (reg8 *) Battery_Voltage_Enable__DM0) 
#define Battery_Voltage_Enable_DM1                    (* (reg8 *) Battery_Voltage_Enable__DM1)
#define Battery_Voltage_Enable_DM2                    (* (reg8 *) Battery_Voltage_Enable__DM2) 
/* Input Buffer Disable Override */
#define Battery_Voltage_Enable_INP_DIS                (* (reg8 *) Battery_Voltage_Enable__INP_DIS)
/* LCD Common or Segment Drive */
#define Battery_Voltage_Enable_LCD_COM_SEG            (* (reg8 *) Battery_Voltage_Enable__LCD_COM_SEG)
/* Enable Segment LCD */
#define Battery_Voltage_Enable_LCD_EN                 (* (reg8 *) Battery_Voltage_Enable__LCD_EN)
/* Slew Rate Control */
#define Battery_Voltage_Enable_SLW                    (* (reg8 *) Battery_Voltage_Enable__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Battery_Voltage_Enable_PRTDSI__CAPS_SEL       (* (reg8 *) Battery_Voltage_Enable__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Battery_Voltage_Enable_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Battery_Voltage_Enable__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Battery_Voltage_Enable_PRTDSI__OE_SEL0        (* (reg8 *) Battery_Voltage_Enable__PRTDSI__OE_SEL0) 
#define Battery_Voltage_Enable_PRTDSI__OE_SEL1        (* (reg8 *) Battery_Voltage_Enable__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Battery_Voltage_Enable_PRTDSI__OUT_SEL0       (* (reg8 *) Battery_Voltage_Enable__PRTDSI__OUT_SEL0) 
#define Battery_Voltage_Enable_PRTDSI__OUT_SEL1       (* (reg8 *) Battery_Voltage_Enable__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Battery_Voltage_Enable_PRTDSI__SYNC_OUT       (* (reg8 *) Battery_Voltage_Enable__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Battery_Voltage_Enable__SIO_CFG)
    #define Battery_Voltage_Enable_SIO_HYST_EN        (* (reg8 *) Battery_Voltage_Enable__SIO_HYST_EN)
    #define Battery_Voltage_Enable_SIO_REG_HIFREQ     (* (reg8 *) Battery_Voltage_Enable__SIO_REG_HIFREQ)
    #define Battery_Voltage_Enable_SIO_CFG            (* (reg8 *) Battery_Voltage_Enable__SIO_CFG)
    #define Battery_Voltage_Enable_SIO_DIFF           (* (reg8 *) Battery_Voltage_Enable__SIO_DIFF)
#endif /* (Battery_Voltage_Enable__SIO_CFG) */

/* Interrupt Registers */
#if defined(Battery_Voltage_Enable__INTSTAT)
    #define Battery_Voltage_Enable_INTSTAT            (* (reg8 *) Battery_Voltage_Enable__INTSTAT)
    #define Battery_Voltage_Enable_SNAP               (* (reg8 *) Battery_Voltage_Enable__SNAP)
    
	#define Battery_Voltage_Enable_0_INTTYPE_REG 		(* (reg8 *) Battery_Voltage_Enable__0__INTTYPE)
#endif /* (Battery_Voltage_Enable__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Battery_Voltage_Enable_H */


/* [] END OF FILE */
