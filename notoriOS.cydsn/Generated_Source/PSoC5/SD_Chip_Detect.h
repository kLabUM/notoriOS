/*******************************************************************************
* File Name: SD_Chip_Detect.h  
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

#if !defined(CY_PINS_SD_Chip_Detect_H) /* Pins SD_Chip_Detect_H */
#define CY_PINS_SD_Chip_Detect_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "SD_Chip_Detect_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 SD_Chip_Detect__PORT == 15 && ((SD_Chip_Detect__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    SD_Chip_Detect_Write(uint8 value);
void    SD_Chip_Detect_SetDriveMode(uint8 mode);
uint8   SD_Chip_Detect_ReadDataReg(void);
uint8   SD_Chip_Detect_Read(void);
void    SD_Chip_Detect_SetInterruptMode(uint16 position, uint16 mode);
uint8   SD_Chip_Detect_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the SD_Chip_Detect_SetDriveMode() function.
     *  @{
     */
        #define SD_Chip_Detect_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define SD_Chip_Detect_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define SD_Chip_Detect_DM_RES_UP          PIN_DM_RES_UP
        #define SD_Chip_Detect_DM_RES_DWN         PIN_DM_RES_DWN
        #define SD_Chip_Detect_DM_OD_LO           PIN_DM_OD_LO
        #define SD_Chip_Detect_DM_OD_HI           PIN_DM_OD_HI
        #define SD_Chip_Detect_DM_STRONG          PIN_DM_STRONG
        #define SD_Chip_Detect_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define SD_Chip_Detect_MASK               SD_Chip_Detect__MASK
#define SD_Chip_Detect_SHIFT              SD_Chip_Detect__SHIFT
#define SD_Chip_Detect_WIDTH              1u

/* Interrupt constants */
#if defined(SD_Chip_Detect__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in SD_Chip_Detect_SetInterruptMode() function.
     *  @{
     */
        #define SD_Chip_Detect_INTR_NONE      (uint16)(0x0000u)
        #define SD_Chip_Detect_INTR_RISING    (uint16)(0x0001u)
        #define SD_Chip_Detect_INTR_FALLING   (uint16)(0x0002u)
        #define SD_Chip_Detect_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define SD_Chip_Detect_INTR_MASK      (0x01u) 
#endif /* (SD_Chip_Detect__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define SD_Chip_Detect_PS                     (* (reg8 *) SD_Chip_Detect__PS)
/* Data Register */
#define SD_Chip_Detect_DR                     (* (reg8 *) SD_Chip_Detect__DR)
/* Port Number */
#define SD_Chip_Detect_PRT_NUM                (* (reg8 *) SD_Chip_Detect__PRT) 
/* Connect to Analog Globals */                                                  
#define SD_Chip_Detect_AG                     (* (reg8 *) SD_Chip_Detect__AG)                       
/* Analog MUX bux enable */
#define SD_Chip_Detect_AMUX                   (* (reg8 *) SD_Chip_Detect__AMUX) 
/* Bidirectional Enable */                                                        
#define SD_Chip_Detect_BIE                    (* (reg8 *) SD_Chip_Detect__BIE)
/* Bit-mask for Aliased Register Access */
#define SD_Chip_Detect_BIT_MASK               (* (reg8 *) SD_Chip_Detect__BIT_MASK)
/* Bypass Enable */
#define SD_Chip_Detect_BYP                    (* (reg8 *) SD_Chip_Detect__BYP)
/* Port wide control signals */                                                   
#define SD_Chip_Detect_CTL                    (* (reg8 *) SD_Chip_Detect__CTL)
/* Drive Modes */
#define SD_Chip_Detect_DM0                    (* (reg8 *) SD_Chip_Detect__DM0) 
#define SD_Chip_Detect_DM1                    (* (reg8 *) SD_Chip_Detect__DM1)
#define SD_Chip_Detect_DM2                    (* (reg8 *) SD_Chip_Detect__DM2) 
/* Input Buffer Disable Override */
#define SD_Chip_Detect_INP_DIS                (* (reg8 *) SD_Chip_Detect__INP_DIS)
/* LCD Common or Segment Drive */
#define SD_Chip_Detect_LCD_COM_SEG            (* (reg8 *) SD_Chip_Detect__LCD_COM_SEG)
/* Enable Segment LCD */
#define SD_Chip_Detect_LCD_EN                 (* (reg8 *) SD_Chip_Detect__LCD_EN)
/* Slew Rate Control */
#define SD_Chip_Detect_SLW                    (* (reg8 *) SD_Chip_Detect__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define SD_Chip_Detect_PRTDSI__CAPS_SEL       (* (reg8 *) SD_Chip_Detect__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define SD_Chip_Detect_PRTDSI__DBL_SYNC_IN    (* (reg8 *) SD_Chip_Detect__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define SD_Chip_Detect_PRTDSI__OE_SEL0        (* (reg8 *) SD_Chip_Detect__PRTDSI__OE_SEL0) 
#define SD_Chip_Detect_PRTDSI__OE_SEL1        (* (reg8 *) SD_Chip_Detect__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define SD_Chip_Detect_PRTDSI__OUT_SEL0       (* (reg8 *) SD_Chip_Detect__PRTDSI__OUT_SEL0) 
#define SD_Chip_Detect_PRTDSI__OUT_SEL1       (* (reg8 *) SD_Chip_Detect__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define SD_Chip_Detect_PRTDSI__SYNC_OUT       (* (reg8 *) SD_Chip_Detect__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(SD_Chip_Detect__SIO_CFG)
    #define SD_Chip_Detect_SIO_HYST_EN        (* (reg8 *) SD_Chip_Detect__SIO_HYST_EN)
    #define SD_Chip_Detect_SIO_REG_HIFREQ     (* (reg8 *) SD_Chip_Detect__SIO_REG_HIFREQ)
    #define SD_Chip_Detect_SIO_CFG            (* (reg8 *) SD_Chip_Detect__SIO_CFG)
    #define SD_Chip_Detect_SIO_DIFF           (* (reg8 *) SD_Chip_Detect__SIO_DIFF)
#endif /* (SD_Chip_Detect__SIO_CFG) */

/* Interrupt Registers */
#if defined(SD_Chip_Detect__INTSTAT)
    #define SD_Chip_Detect_INTSTAT            (* (reg8 *) SD_Chip_Detect__INTSTAT)
    #define SD_Chip_Detect_SNAP               (* (reg8 *) SD_Chip_Detect__SNAP)
    
	#define SD_Chip_Detect_0_INTTYPE_REG 		(* (reg8 *) SD_Chip_Detect__0__INTTYPE)
#endif /* (SD_Chip_Detect__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_SD_Chip_Detect_H */


/* [] END OF FILE */
