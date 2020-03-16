/*******************************************************************************
* File Name: AN_VSOL.h  
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

#if !defined(CY_PINS_AN_VSOL_H) /* Pins AN_VSOL_H */
#define CY_PINS_AN_VSOL_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "AN_VSOL_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 AN_VSOL__PORT == 15 && ((AN_VSOL__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    AN_VSOL_Write(uint8 value);
void    AN_VSOL_SetDriveMode(uint8 mode);
uint8   AN_VSOL_ReadDataReg(void);
uint8   AN_VSOL_Read(void);
void    AN_VSOL_SetInterruptMode(uint16 position, uint16 mode);
uint8   AN_VSOL_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the AN_VSOL_SetDriveMode() function.
     *  @{
     */
        #define AN_VSOL_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define AN_VSOL_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define AN_VSOL_DM_RES_UP          PIN_DM_RES_UP
        #define AN_VSOL_DM_RES_DWN         PIN_DM_RES_DWN
        #define AN_VSOL_DM_OD_LO           PIN_DM_OD_LO
        #define AN_VSOL_DM_OD_HI           PIN_DM_OD_HI
        #define AN_VSOL_DM_STRONG          PIN_DM_STRONG
        #define AN_VSOL_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define AN_VSOL_MASK               AN_VSOL__MASK
#define AN_VSOL_SHIFT              AN_VSOL__SHIFT
#define AN_VSOL_WIDTH              1u

/* Interrupt constants */
#if defined(AN_VSOL__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in AN_VSOL_SetInterruptMode() function.
     *  @{
     */
        #define AN_VSOL_INTR_NONE      (uint16)(0x0000u)
        #define AN_VSOL_INTR_RISING    (uint16)(0x0001u)
        #define AN_VSOL_INTR_FALLING   (uint16)(0x0002u)
        #define AN_VSOL_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define AN_VSOL_INTR_MASK      (0x01u) 
#endif /* (AN_VSOL__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define AN_VSOL_PS                     (* (reg8 *) AN_VSOL__PS)
/* Data Register */
#define AN_VSOL_DR                     (* (reg8 *) AN_VSOL__DR)
/* Port Number */
#define AN_VSOL_PRT_NUM                (* (reg8 *) AN_VSOL__PRT) 
/* Connect to Analog Globals */                                                  
#define AN_VSOL_AG                     (* (reg8 *) AN_VSOL__AG)                       
/* Analog MUX bux enable */
#define AN_VSOL_AMUX                   (* (reg8 *) AN_VSOL__AMUX) 
/* Bidirectional Enable */                                                        
#define AN_VSOL_BIE                    (* (reg8 *) AN_VSOL__BIE)
/* Bit-mask for Aliased Register Access */
#define AN_VSOL_BIT_MASK               (* (reg8 *) AN_VSOL__BIT_MASK)
/* Bypass Enable */
#define AN_VSOL_BYP                    (* (reg8 *) AN_VSOL__BYP)
/* Port wide control signals */                                                   
#define AN_VSOL_CTL                    (* (reg8 *) AN_VSOL__CTL)
/* Drive Modes */
#define AN_VSOL_DM0                    (* (reg8 *) AN_VSOL__DM0) 
#define AN_VSOL_DM1                    (* (reg8 *) AN_VSOL__DM1)
#define AN_VSOL_DM2                    (* (reg8 *) AN_VSOL__DM2) 
/* Input Buffer Disable Override */
#define AN_VSOL_INP_DIS                (* (reg8 *) AN_VSOL__INP_DIS)
/* LCD Common or Segment Drive */
#define AN_VSOL_LCD_COM_SEG            (* (reg8 *) AN_VSOL__LCD_COM_SEG)
/* Enable Segment LCD */
#define AN_VSOL_LCD_EN                 (* (reg8 *) AN_VSOL__LCD_EN)
/* Slew Rate Control */
#define AN_VSOL_SLW                    (* (reg8 *) AN_VSOL__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define AN_VSOL_PRTDSI__CAPS_SEL       (* (reg8 *) AN_VSOL__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define AN_VSOL_PRTDSI__DBL_SYNC_IN    (* (reg8 *) AN_VSOL__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define AN_VSOL_PRTDSI__OE_SEL0        (* (reg8 *) AN_VSOL__PRTDSI__OE_SEL0) 
#define AN_VSOL_PRTDSI__OE_SEL1        (* (reg8 *) AN_VSOL__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define AN_VSOL_PRTDSI__OUT_SEL0       (* (reg8 *) AN_VSOL__PRTDSI__OUT_SEL0) 
#define AN_VSOL_PRTDSI__OUT_SEL1       (* (reg8 *) AN_VSOL__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define AN_VSOL_PRTDSI__SYNC_OUT       (* (reg8 *) AN_VSOL__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(AN_VSOL__SIO_CFG)
    #define AN_VSOL_SIO_HYST_EN        (* (reg8 *) AN_VSOL__SIO_HYST_EN)
    #define AN_VSOL_SIO_REG_HIFREQ     (* (reg8 *) AN_VSOL__SIO_REG_HIFREQ)
    #define AN_VSOL_SIO_CFG            (* (reg8 *) AN_VSOL__SIO_CFG)
    #define AN_VSOL_SIO_DIFF           (* (reg8 *) AN_VSOL__SIO_DIFF)
#endif /* (AN_VSOL__SIO_CFG) */

/* Interrupt Registers */
#if defined(AN_VSOL__INTSTAT)
    #define AN_VSOL_INTSTAT            (* (reg8 *) AN_VSOL__INTSTAT)
    #define AN_VSOL_SNAP               (* (reg8 *) AN_VSOL__SNAP)
    
	#define AN_VSOL_0_INTTYPE_REG 		(* (reg8 *) AN_VSOL__0__INTTYPE)
#endif /* (AN_VSOL__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_AN_VSOL_H */


/* [] END OF FILE */
