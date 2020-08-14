/*******************************************************************************
* File Name: AN_VBAT.h  
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

#if !defined(CY_PINS_AN_VBAT_H) /* Pins AN_VBAT_H */
#define CY_PINS_AN_VBAT_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "AN_VBAT_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 AN_VBAT__PORT == 15 && ((AN_VBAT__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    AN_VBAT_Write(uint8 value);
void    AN_VBAT_SetDriveMode(uint8 mode);
uint8   AN_VBAT_ReadDataReg(void);
uint8   AN_VBAT_Read(void);
void    AN_VBAT_SetInterruptMode(uint16 position, uint16 mode);
uint8   AN_VBAT_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the AN_VBAT_SetDriveMode() function.
     *  @{
     */
        #define AN_VBAT_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define AN_VBAT_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define AN_VBAT_DM_RES_UP          PIN_DM_RES_UP
        #define AN_VBAT_DM_RES_DWN         PIN_DM_RES_DWN
        #define AN_VBAT_DM_OD_LO           PIN_DM_OD_LO
        #define AN_VBAT_DM_OD_HI           PIN_DM_OD_HI
        #define AN_VBAT_DM_STRONG          PIN_DM_STRONG
        #define AN_VBAT_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define AN_VBAT_MASK               AN_VBAT__MASK
#define AN_VBAT_SHIFT              AN_VBAT__SHIFT
#define AN_VBAT_WIDTH              1u

/* Interrupt constants */
#if defined(AN_VBAT__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in AN_VBAT_SetInterruptMode() function.
     *  @{
     */
        #define AN_VBAT_INTR_NONE      (uint16)(0x0000u)
        #define AN_VBAT_INTR_RISING    (uint16)(0x0001u)
        #define AN_VBAT_INTR_FALLING   (uint16)(0x0002u)
        #define AN_VBAT_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define AN_VBAT_INTR_MASK      (0x01u) 
#endif /* (AN_VBAT__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define AN_VBAT_PS                     (* (reg8 *) AN_VBAT__PS)
/* Data Register */
#define AN_VBAT_DR                     (* (reg8 *) AN_VBAT__DR)
/* Port Number */
#define AN_VBAT_PRT_NUM                (* (reg8 *) AN_VBAT__PRT) 
/* Connect to Analog Globals */                                                  
#define AN_VBAT_AG                     (* (reg8 *) AN_VBAT__AG)                       
/* Analog MUX bux enable */
#define AN_VBAT_AMUX                   (* (reg8 *) AN_VBAT__AMUX) 
/* Bidirectional Enable */                                                        
#define AN_VBAT_BIE                    (* (reg8 *) AN_VBAT__BIE)
/* Bit-mask for Aliased Register Access */
#define AN_VBAT_BIT_MASK               (* (reg8 *) AN_VBAT__BIT_MASK)
/* Bypass Enable */
#define AN_VBAT_BYP                    (* (reg8 *) AN_VBAT__BYP)
/* Port wide control signals */                                                   
#define AN_VBAT_CTL                    (* (reg8 *) AN_VBAT__CTL)
/* Drive Modes */
#define AN_VBAT_DM0                    (* (reg8 *) AN_VBAT__DM0) 
#define AN_VBAT_DM1                    (* (reg8 *) AN_VBAT__DM1)
#define AN_VBAT_DM2                    (* (reg8 *) AN_VBAT__DM2) 
/* Input Buffer Disable Override */
#define AN_VBAT_INP_DIS                (* (reg8 *) AN_VBAT__INP_DIS)
/* LCD Common or Segment Drive */
#define AN_VBAT_LCD_COM_SEG            (* (reg8 *) AN_VBAT__LCD_COM_SEG)
/* Enable Segment LCD */
#define AN_VBAT_LCD_EN                 (* (reg8 *) AN_VBAT__LCD_EN)
/* Slew Rate Control */
#define AN_VBAT_SLW                    (* (reg8 *) AN_VBAT__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define AN_VBAT_PRTDSI__CAPS_SEL       (* (reg8 *) AN_VBAT__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define AN_VBAT_PRTDSI__DBL_SYNC_IN    (* (reg8 *) AN_VBAT__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define AN_VBAT_PRTDSI__OE_SEL0        (* (reg8 *) AN_VBAT__PRTDSI__OE_SEL0) 
#define AN_VBAT_PRTDSI__OE_SEL1        (* (reg8 *) AN_VBAT__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define AN_VBAT_PRTDSI__OUT_SEL0       (* (reg8 *) AN_VBAT__PRTDSI__OUT_SEL0) 
#define AN_VBAT_PRTDSI__OUT_SEL1       (* (reg8 *) AN_VBAT__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define AN_VBAT_PRTDSI__SYNC_OUT       (* (reg8 *) AN_VBAT__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(AN_VBAT__SIO_CFG)
    #define AN_VBAT_SIO_HYST_EN        (* (reg8 *) AN_VBAT__SIO_HYST_EN)
    #define AN_VBAT_SIO_REG_HIFREQ     (* (reg8 *) AN_VBAT__SIO_REG_HIFREQ)
    #define AN_VBAT_SIO_CFG            (* (reg8 *) AN_VBAT__SIO_CFG)
    #define AN_VBAT_SIO_DIFF           (* (reg8 *) AN_VBAT__SIO_DIFF)
#endif /* (AN_VBAT__SIO_CFG) */

/* Interrupt Registers */
#if defined(AN_VBAT__INTSTAT)
    #define AN_VBAT_INTSTAT            (* (reg8 *) AN_VBAT__INTSTAT)
    #define AN_VBAT_SNAP               (* (reg8 *) AN_VBAT__SNAP)
    
	#define AN_VBAT_0_INTTYPE_REG 		(* (reg8 *) AN_VBAT__0__INTTYPE)
#endif /* (AN_VBAT__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_AN_VBAT_H */


/* [] END OF FILE */
