/*******************************************************************************
* File Name: Power_VDD3.h  
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

#if !defined(CY_PINS_Power_VDD3_H) /* Pins Power_VDD3_H */
#define CY_PINS_Power_VDD3_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Power_VDD3_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Power_VDD3__PORT == 15 && ((Power_VDD3__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Power_VDD3_Write(uint8 value);
void    Power_VDD3_SetDriveMode(uint8 mode);
uint8   Power_VDD3_ReadDataReg(void);
uint8   Power_VDD3_Read(void);
void    Power_VDD3_SetInterruptMode(uint16 position, uint16 mode);
uint8   Power_VDD3_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Power_VDD3_SetDriveMode() function.
     *  @{
     */
        #define Power_VDD3_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Power_VDD3_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Power_VDD3_DM_RES_UP          PIN_DM_RES_UP
        #define Power_VDD3_DM_RES_DWN         PIN_DM_RES_DWN
        #define Power_VDD3_DM_OD_LO           PIN_DM_OD_LO
        #define Power_VDD3_DM_OD_HI           PIN_DM_OD_HI
        #define Power_VDD3_DM_STRONG          PIN_DM_STRONG
        #define Power_VDD3_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Power_VDD3_MASK               Power_VDD3__MASK
#define Power_VDD3_SHIFT              Power_VDD3__SHIFT
#define Power_VDD3_WIDTH              1u

/* Interrupt constants */
#if defined(Power_VDD3__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Power_VDD3_SetInterruptMode() function.
     *  @{
     */
        #define Power_VDD3_INTR_NONE      (uint16)(0x0000u)
        #define Power_VDD3_INTR_RISING    (uint16)(0x0001u)
        #define Power_VDD3_INTR_FALLING   (uint16)(0x0002u)
        #define Power_VDD3_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Power_VDD3_INTR_MASK      (0x01u) 
#endif /* (Power_VDD3__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Power_VDD3_PS                     (* (reg8 *) Power_VDD3__PS)
/* Data Register */
#define Power_VDD3_DR                     (* (reg8 *) Power_VDD3__DR)
/* Port Number */
#define Power_VDD3_PRT_NUM                (* (reg8 *) Power_VDD3__PRT) 
/* Connect to Analog Globals */                                                  
#define Power_VDD3_AG                     (* (reg8 *) Power_VDD3__AG)                       
/* Analog MUX bux enable */
#define Power_VDD3_AMUX                   (* (reg8 *) Power_VDD3__AMUX) 
/* Bidirectional Enable */                                                        
#define Power_VDD3_BIE                    (* (reg8 *) Power_VDD3__BIE)
/* Bit-mask for Aliased Register Access */
#define Power_VDD3_BIT_MASK               (* (reg8 *) Power_VDD3__BIT_MASK)
/* Bypass Enable */
#define Power_VDD3_BYP                    (* (reg8 *) Power_VDD3__BYP)
/* Port wide control signals */                                                   
#define Power_VDD3_CTL                    (* (reg8 *) Power_VDD3__CTL)
/* Drive Modes */
#define Power_VDD3_DM0                    (* (reg8 *) Power_VDD3__DM0) 
#define Power_VDD3_DM1                    (* (reg8 *) Power_VDD3__DM1)
#define Power_VDD3_DM2                    (* (reg8 *) Power_VDD3__DM2) 
/* Input Buffer Disable Override */
#define Power_VDD3_INP_DIS                (* (reg8 *) Power_VDD3__INP_DIS)
/* LCD Common or Segment Drive */
#define Power_VDD3_LCD_COM_SEG            (* (reg8 *) Power_VDD3__LCD_COM_SEG)
/* Enable Segment LCD */
#define Power_VDD3_LCD_EN                 (* (reg8 *) Power_VDD3__LCD_EN)
/* Slew Rate Control */
#define Power_VDD3_SLW                    (* (reg8 *) Power_VDD3__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Power_VDD3_PRTDSI__CAPS_SEL       (* (reg8 *) Power_VDD3__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Power_VDD3_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Power_VDD3__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Power_VDD3_PRTDSI__OE_SEL0        (* (reg8 *) Power_VDD3__PRTDSI__OE_SEL0) 
#define Power_VDD3_PRTDSI__OE_SEL1        (* (reg8 *) Power_VDD3__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Power_VDD3_PRTDSI__OUT_SEL0       (* (reg8 *) Power_VDD3__PRTDSI__OUT_SEL0) 
#define Power_VDD3_PRTDSI__OUT_SEL1       (* (reg8 *) Power_VDD3__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Power_VDD3_PRTDSI__SYNC_OUT       (* (reg8 *) Power_VDD3__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Power_VDD3__SIO_CFG)
    #define Power_VDD3_SIO_HYST_EN        (* (reg8 *) Power_VDD3__SIO_HYST_EN)
    #define Power_VDD3_SIO_REG_HIFREQ     (* (reg8 *) Power_VDD3__SIO_REG_HIFREQ)
    #define Power_VDD3_SIO_CFG            (* (reg8 *) Power_VDD3__SIO_CFG)
    #define Power_VDD3_SIO_DIFF           (* (reg8 *) Power_VDD3__SIO_DIFF)
#endif /* (Power_VDD3__SIO_CFG) */

/* Interrupt Registers */
#if defined(Power_VDD3__INTSTAT)
    #define Power_VDD3_INTSTAT            (* (reg8 *) Power_VDD3__INTSTAT)
    #define Power_VDD3_SNAP               (* (reg8 *) Power_VDD3__SNAP)
    
	#define Power_VDD3_0_INTTYPE_REG 		(* (reg8 *) Power_VDD3__0__INTTYPE)
#endif /* (Power_VDD3__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Power_VDD3_H */


/* [] END OF FILE */
