/*******************************************************************************
* File Name: AN_PRTRANS.h  
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

#if !defined(CY_PINS_AN_PRTRANS_H) /* Pins AN_PRTRANS_H */
#define CY_PINS_AN_PRTRANS_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "AN_PRTRANS_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 AN_PRTRANS__PORT == 15 && ((AN_PRTRANS__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    AN_PRTRANS_Write(uint8 value);
void    AN_PRTRANS_SetDriveMode(uint8 mode);
uint8   AN_PRTRANS_ReadDataReg(void);
uint8   AN_PRTRANS_Read(void);
void    AN_PRTRANS_SetInterruptMode(uint16 position, uint16 mode);
uint8   AN_PRTRANS_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the AN_PRTRANS_SetDriveMode() function.
     *  @{
     */
        #define AN_PRTRANS_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define AN_PRTRANS_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define AN_PRTRANS_DM_RES_UP          PIN_DM_RES_UP
        #define AN_PRTRANS_DM_RES_DWN         PIN_DM_RES_DWN
        #define AN_PRTRANS_DM_OD_LO           PIN_DM_OD_LO
        #define AN_PRTRANS_DM_OD_HI           PIN_DM_OD_HI
        #define AN_PRTRANS_DM_STRONG          PIN_DM_STRONG
        #define AN_PRTRANS_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define AN_PRTRANS_MASK               AN_PRTRANS__MASK
#define AN_PRTRANS_SHIFT              AN_PRTRANS__SHIFT
#define AN_PRTRANS_WIDTH              1u

/* Interrupt constants */
#if defined(AN_PRTRANS__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in AN_PRTRANS_SetInterruptMode() function.
     *  @{
     */
        #define AN_PRTRANS_INTR_NONE      (uint16)(0x0000u)
        #define AN_PRTRANS_INTR_RISING    (uint16)(0x0001u)
        #define AN_PRTRANS_INTR_FALLING   (uint16)(0x0002u)
        #define AN_PRTRANS_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define AN_PRTRANS_INTR_MASK      (0x01u) 
#endif /* (AN_PRTRANS__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define AN_PRTRANS_PS                     (* (reg8 *) AN_PRTRANS__PS)
/* Data Register */
#define AN_PRTRANS_DR                     (* (reg8 *) AN_PRTRANS__DR)
/* Port Number */
#define AN_PRTRANS_PRT_NUM                (* (reg8 *) AN_PRTRANS__PRT) 
/* Connect to Analog Globals */                                                  
#define AN_PRTRANS_AG                     (* (reg8 *) AN_PRTRANS__AG)                       
/* Analog MUX bux enable */
#define AN_PRTRANS_AMUX                   (* (reg8 *) AN_PRTRANS__AMUX) 
/* Bidirectional Enable */                                                        
#define AN_PRTRANS_BIE                    (* (reg8 *) AN_PRTRANS__BIE)
/* Bit-mask for Aliased Register Access */
#define AN_PRTRANS_BIT_MASK               (* (reg8 *) AN_PRTRANS__BIT_MASK)
/* Bypass Enable */
#define AN_PRTRANS_BYP                    (* (reg8 *) AN_PRTRANS__BYP)
/* Port wide control signals */                                                   
#define AN_PRTRANS_CTL                    (* (reg8 *) AN_PRTRANS__CTL)
/* Drive Modes */
#define AN_PRTRANS_DM0                    (* (reg8 *) AN_PRTRANS__DM0) 
#define AN_PRTRANS_DM1                    (* (reg8 *) AN_PRTRANS__DM1)
#define AN_PRTRANS_DM2                    (* (reg8 *) AN_PRTRANS__DM2) 
/* Input Buffer Disable Override */
#define AN_PRTRANS_INP_DIS                (* (reg8 *) AN_PRTRANS__INP_DIS)
/* LCD Common or Segment Drive */
#define AN_PRTRANS_LCD_COM_SEG            (* (reg8 *) AN_PRTRANS__LCD_COM_SEG)
/* Enable Segment LCD */
#define AN_PRTRANS_LCD_EN                 (* (reg8 *) AN_PRTRANS__LCD_EN)
/* Slew Rate Control */
#define AN_PRTRANS_SLW                    (* (reg8 *) AN_PRTRANS__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define AN_PRTRANS_PRTDSI__CAPS_SEL       (* (reg8 *) AN_PRTRANS__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define AN_PRTRANS_PRTDSI__DBL_SYNC_IN    (* (reg8 *) AN_PRTRANS__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define AN_PRTRANS_PRTDSI__OE_SEL0        (* (reg8 *) AN_PRTRANS__PRTDSI__OE_SEL0) 
#define AN_PRTRANS_PRTDSI__OE_SEL1        (* (reg8 *) AN_PRTRANS__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define AN_PRTRANS_PRTDSI__OUT_SEL0       (* (reg8 *) AN_PRTRANS__PRTDSI__OUT_SEL0) 
#define AN_PRTRANS_PRTDSI__OUT_SEL1       (* (reg8 *) AN_PRTRANS__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define AN_PRTRANS_PRTDSI__SYNC_OUT       (* (reg8 *) AN_PRTRANS__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(AN_PRTRANS__SIO_CFG)
    #define AN_PRTRANS_SIO_HYST_EN        (* (reg8 *) AN_PRTRANS__SIO_HYST_EN)
    #define AN_PRTRANS_SIO_REG_HIFREQ     (* (reg8 *) AN_PRTRANS__SIO_REG_HIFREQ)
    #define AN_PRTRANS_SIO_CFG            (* (reg8 *) AN_PRTRANS__SIO_CFG)
    #define AN_PRTRANS_SIO_DIFF           (* (reg8 *) AN_PRTRANS__SIO_DIFF)
#endif /* (AN_PRTRANS__SIO_CFG) */

/* Interrupt Registers */
#if defined(AN_PRTRANS__INTSTAT)
    #define AN_PRTRANS_INTSTAT            (* (reg8 *) AN_PRTRANS__INTSTAT)
    #define AN_PRTRANS_SNAP               (* (reg8 *) AN_PRTRANS__SNAP)
    
	#define AN_PRTRANS_0_INTTYPE_REG 		(* (reg8 *) AN_PRTRANS__0__INTTYPE)
#endif /* (AN_PRTRANS__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_AN_PRTRANS_H */


/* [] END OF FILE */
