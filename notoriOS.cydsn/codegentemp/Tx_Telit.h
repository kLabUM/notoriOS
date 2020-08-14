/*******************************************************************************
* File Name: Tx_Telit.h  
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

#if !defined(CY_PINS_Tx_Telit_H) /* Pins Tx_Telit_H */
#define CY_PINS_Tx_Telit_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Tx_Telit_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Tx_Telit__PORT == 15 && ((Tx_Telit__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Tx_Telit_Write(uint8 value);
void    Tx_Telit_SetDriveMode(uint8 mode);
uint8   Tx_Telit_ReadDataReg(void);
uint8   Tx_Telit_Read(void);
void    Tx_Telit_SetInterruptMode(uint16 position, uint16 mode);
uint8   Tx_Telit_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Tx_Telit_SetDriveMode() function.
     *  @{
     */
        #define Tx_Telit_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Tx_Telit_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Tx_Telit_DM_RES_UP          PIN_DM_RES_UP
        #define Tx_Telit_DM_RES_DWN         PIN_DM_RES_DWN
        #define Tx_Telit_DM_OD_LO           PIN_DM_OD_LO
        #define Tx_Telit_DM_OD_HI           PIN_DM_OD_HI
        #define Tx_Telit_DM_STRONG          PIN_DM_STRONG
        #define Tx_Telit_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Tx_Telit_MASK               Tx_Telit__MASK
#define Tx_Telit_SHIFT              Tx_Telit__SHIFT
#define Tx_Telit_WIDTH              1u

/* Interrupt constants */
#if defined(Tx_Telit__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Tx_Telit_SetInterruptMode() function.
     *  @{
     */
        #define Tx_Telit_INTR_NONE      (uint16)(0x0000u)
        #define Tx_Telit_INTR_RISING    (uint16)(0x0001u)
        #define Tx_Telit_INTR_FALLING   (uint16)(0x0002u)
        #define Tx_Telit_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Tx_Telit_INTR_MASK      (0x01u) 
#endif /* (Tx_Telit__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Tx_Telit_PS                     (* (reg8 *) Tx_Telit__PS)
/* Data Register */
#define Tx_Telit_DR                     (* (reg8 *) Tx_Telit__DR)
/* Port Number */
#define Tx_Telit_PRT_NUM                (* (reg8 *) Tx_Telit__PRT) 
/* Connect to Analog Globals */                                                  
#define Tx_Telit_AG                     (* (reg8 *) Tx_Telit__AG)                       
/* Analog MUX bux enable */
#define Tx_Telit_AMUX                   (* (reg8 *) Tx_Telit__AMUX) 
/* Bidirectional Enable */                                                        
#define Tx_Telit_BIE                    (* (reg8 *) Tx_Telit__BIE)
/* Bit-mask for Aliased Register Access */
#define Tx_Telit_BIT_MASK               (* (reg8 *) Tx_Telit__BIT_MASK)
/* Bypass Enable */
#define Tx_Telit_BYP                    (* (reg8 *) Tx_Telit__BYP)
/* Port wide control signals */                                                   
#define Tx_Telit_CTL                    (* (reg8 *) Tx_Telit__CTL)
/* Drive Modes */
#define Tx_Telit_DM0                    (* (reg8 *) Tx_Telit__DM0) 
#define Tx_Telit_DM1                    (* (reg8 *) Tx_Telit__DM1)
#define Tx_Telit_DM2                    (* (reg8 *) Tx_Telit__DM2) 
/* Input Buffer Disable Override */
#define Tx_Telit_INP_DIS                (* (reg8 *) Tx_Telit__INP_DIS)
/* LCD Common or Segment Drive */
#define Tx_Telit_LCD_COM_SEG            (* (reg8 *) Tx_Telit__LCD_COM_SEG)
/* Enable Segment LCD */
#define Tx_Telit_LCD_EN                 (* (reg8 *) Tx_Telit__LCD_EN)
/* Slew Rate Control */
#define Tx_Telit_SLW                    (* (reg8 *) Tx_Telit__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Tx_Telit_PRTDSI__CAPS_SEL       (* (reg8 *) Tx_Telit__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Tx_Telit_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Tx_Telit__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Tx_Telit_PRTDSI__OE_SEL0        (* (reg8 *) Tx_Telit__PRTDSI__OE_SEL0) 
#define Tx_Telit_PRTDSI__OE_SEL1        (* (reg8 *) Tx_Telit__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Tx_Telit_PRTDSI__OUT_SEL0       (* (reg8 *) Tx_Telit__PRTDSI__OUT_SEL0) 
#define Tx_Telit_PRTDSI__OUT_SEL1       (* (reg8 *) Tx_Telit__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Tx_Telit_PRTDSI__SYNC_OUT       (* (reg8 *) Tx_Telit__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Tx_Telit__SIO_CFG)
    #define Tx_Telit_SIO_HYST_EN        (* (reg8 *) Tx_Telit__SIO_HYST_EN)
    #define Tx_Telit_SIO_REG_HIFREQ     (* (reg8 *) Tx_Telit__SIO_REG_HIFREQ)
    #define Tx_Telit_SIO_CFG            (* (reg8 *) Tx_Telit__SIO_CFG)
    #define Tx_Telit_SIO_DIFF           (* (reg8 *) Tx_Telit__SIO_DIFF)
#endif /* (Tx_Telit__SIO_CFG) */

/* Interrupt Registers */
#if defined(Tx_Telit__INTSTAT)
    #define Tx_Telit_INTSTAT            (* (reg8 *) Tx_Telit__INTSTAT)
    #define Tx_Telit_SNAP               (* (reg8 *) Tx_Telit__SNAP)
    
	#define Tx_Telit_0_INTTYPE_REG 		(* (reg8 *) Tx_Telit__0__INTTYPE)
#endif /* (Tx_Telit__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Tx_Telit_H */


/* [] END OF FILE */
