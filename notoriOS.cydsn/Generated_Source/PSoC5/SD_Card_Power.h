/*******************************************************************************
* File Name: SD_Card_Power.h  
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

#if !defined(CY_PINS_SD_Card_Power_H) /* Pins SD_Card_Power_H */
#define CY_PINS_SD_Card_Power_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "SD_Card_Power_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 SD_Card_Power__PORT == 15 && ((SD_Card_Power__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    SD_Card_Power_Write(uint8 value);
void    SD_Card_Power_SetDriveMode(uint8 mode);
uint8   SD_Card_Power_ReadDataReg(void);
uint8   SD_Card_Power_Read(void);
void    SD_Card_Power_SetInterruptMode(uint16 position, uint16 mode);
uint8   SD_Card_Power_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the SD_Card_Power_SetDriveMode() function.
     *  @{
     */
        #define SD_Card_Power_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define SD_Card_Power_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define SD_Card_Power_DM_RES_UP          PIN_DM_RES_UP
        #define SD_Card_Power_DM_RES_DWN         PIN_DM_RES_DWN
        #define SD_Card_Power_DM_OD_LO           PIN_DM_OD_LO
        #define SD_Card_Power_DM_OD_HI           PIN_DM_OD_HI
        #define SD_Card_Power_DM_STRONG          PIN_DM_STRONG
        #define SD_Card_Power_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define SD_Card_Power_MASK               SD_Card_Power__MASK
#define SD_Card_Power_SHIFT              SD_Card_Power__SHIFT
#define SD_Card_Power_WIDTH              1u

/* Interrupt constants */
#if defined(SD_Card_Power__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in SD_Card_Power_SetInterruptMode() function.
     *  @{
     */
        #define SD_Card_Power_INTR_NONE      (uint16)(0x0000u)
        #define SD_Card_Power_INTR_RISING    (uint16)(0x0001u)
        #define SD_Card_Power_INTR_FALLING   (uint16)(0x0002u)
        #define SD_Card_Power_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define SD_Card_Power_INTR_MASK      (0x01u) 
#endif /* (SD_Card_Power__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define SD_Card_Power_PS                     (* (reg8 *) SD_Card_Power__PS)
/* Data Register */
#define SD_Card_Power_DR                     (* (reg8 *) SD_Card_Power__DR)
/* Port Number */
#define SD_Card_Power_PRT_NUM                (* (reg8 *) SD_Card_Power__PRT) 
/* Connect to Analog Globals */                                                  
#define SD_Card_Power_AG                     (* (reg8 *) SD_Card_Power__AG)                       
/* Analog MUX bux enable */
#define SD_Card_Power_AMUX                   (* (reg8 *) SD_Card_Power__AMUX) 
/* Bidirectional Enable */                                                        
#define SD_Card_Power_BIE                    (* (reg8 *) SD_Card_Power__BIE)
/* Bit-mask for Aliased Register Access */
#define SD_Card_Power_BIT_MASK               (* (reg8 *) SD_Card_Power__BIT_MASK)
/* Bypass Enable */
#define SD_Card_Power_BYP                    (* (reg8 *) SD_Card_Power__BYP)
/* Port wide control signals */                                                   
#define SD_Card_Power_CTL                    (* (reg8 *) SD_Card_Power__CTL)
/* Drive Modes */
#define SD_Card_Power_DM0                    (* (reg8 *) SD_Card_Power__DM0) 
#define SD_Card_Power_DM1                    (* (reg8 *) SD_Card_Power__DM1)
#define SD_Card_Power_DM2                    (* (reg8 *) SD_Card_Power__DM2) 
/* Input Buffer Disable Override */
#define SD_Card_Power_INP_DIS                (* (reg8 *) SD_Card_Power__INP_DIS)
/* LCD Common or Segment Drive */
#define SD_Card_Power_LCD_COM_SEG            (* (reg8 *) SD_Card_Power__LCD_COM_SEG)
/* Enable Segment LCD */
#define SD_Card_Power_LCD_EN                 (* (reg8 *) SD_Card_Power__LCD_EN)
/* Slew Rate Control */
#define SD_Card_Power_SLW                    (* (reg8 *) SD_Card_Power__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define SD_Card_Power_PRTDSI__CAPS_SEL       (* (reg8 *) SD_Card_Power__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define SD_Card_Power_PRTDSI__DBL_SYNC_IN    (* (reg8 *) SD_Card_Power__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define SD_Card_Power_PRTDSI__OE_SEL0        (* (reg8 *) SD_Card_Power__PRTDSI__OE_SEL0) 
#define SD_Card_Power_PRTDSI__OE_SEL1        (* (reg8 *) SD_Card_Power__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define SD_Card_Power_PRTDSI__OUT_SEL0       (* (reg8 *) SD_Card_Power__PRTDSI__OUT_SEL0) 
#define SD_Card_Power_PRTDSI__OUT_SEL1       (* (reg8 *) SD_Card_Power__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define SD_Card_Power_PRTDSI__SYNC_OUT       (* (reg8 *) SD_Card_Power__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(SD_Card_Power__SIO_CFG)
    #define SD_Card_Power_SIO_HYST_EN        (* (reg8 *) SD_Card_Power__SIO_HYST_EN)
    #define SD_Card_Power_SIO_REG_HIFREQ     (* (reg8 *) SD_Card_Power__SIO_REG_HIFREQ)
    #define SD_Card_Power_SIO_CFG            (* (reg8 *) SD_Card_Power__SIO_CFG)
    #define SD_Card_Power_SIO_DIFF           (* (reg8 *) SD_Card_Power__SIO_DIFF)
#endif /* (SD_Card_Power__SIO_CFG) */

/* Interrupt Registers */
#if defined(SD_Card_Power__INTSTAT)
    #define SD_Card_Power_INTSTAT            (* (reg8 *) SD_Card_Power__INTSTAT)
    #define SD_Card_Power_SNAP               (* (reg8 *) SD_Card_Power__SNAP)
    
	#define SD_Card_Power_0_INTTYPE_REG 		(* (reg8 *) SD_Card_Power__0__INTTYPE)
#endif /* (SD_Card_Power__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_SD_Card_Power_H */


/* [] END OF FILE */
