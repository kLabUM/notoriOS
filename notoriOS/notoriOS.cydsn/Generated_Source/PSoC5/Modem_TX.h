/*******************************************************************************
* File Name: Modem_TX.h  
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

#if !defined(CY_PINS_Modem_TX_H) /* Pins Modem_TX_H */
#define CY_PINS_Modem_TX_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Modem_TX_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Modem_TX__PORT == 15 && ((Modem_TX__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Modem_TX_Write(uint8 value);
void    Modem_TX_SetDriveMode(uint8 mode);
uint8   Modem_TX_ReadDataReg(void);
uint8   Modem_TX_Read(void);
void    Modem_TX_SetInterruptMode(uint16 position, uint16 mode);
uint8   Modem_TX_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Modem_TX_SetDriveMode() function.
     *  @{
     */
        #define Modem_TX_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Modem_TX_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Modem_TX_DM_RES_UP          PIN_DM_RES_UP
        #define Modem_TX_DM_RES_DWN         PIN_DM_RES_DWN
        #define Modem_TX_DM_OD_LO           PIN_DM_OD_LO
        #define Modem_TX_DM_OD_HI           PIN_DM_OD_HI
        #define Modem_TX_DM_STRONG          PIN_DM_STRONG
        #define Modem_TX_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Modem_TX_MASK               Modem_TX__MASK
#define Modem_TX_SHIFT              Modem_TX__SHIFT
#define Modem_TX_WIDTH              1u

/* Interrupt constants */
#if defined(Modem_TX__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Modem_TX_SetInterruptMode() function.
     *  @{
     */
        #define Modem_TX_INTR_NONE      (uint16)(0x0000u)
        #define Modem_TX_INTR_RISING    (uint16)(0x0001u)
        #define Modem_TX_INTR_FALLING   (uint16)(0x0002u)
        #define Modem_TX_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Modem_TX_INTR_MASK      (0x01u) 
#endif /* (Modem_TX__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Modem_TX_PS                     (* (reg8 *) Modem_TX__PS)
/* Data Register */
#define Modem_TX_DR                     (* (reg8 *) Modem_TX__DR)
/* Port Number */
#define Modem_TX_PRT_NUM                (* (reg8 *) Modem_TX__PRT) 
/* Connect to Analog Globals */                                                  
#define Modem_TX_AG                     (* (reg8 *) Modem_TX__AG)                       
/* Analog MUX bux enable */
#define Modem_TX_AMUX                   (* (reg8 *) Modem_TX__AMUX) 
/* Bidirectional Enable */                                                        
#define Modem_TX_BIE                    (* (reg8 *) Modem_TX__BIE)
/* Bit-mask for Aliased Register Access */
#define Modem_TX_BIT_MASK               (* (reg8 *) Modem_TX__BIT_MASK)
/* Bypass Enable */
#define Modem_TX_BYP                    (* (reg8 *) Modem_TX__BYP)
/* Port wide control signals */                                                   
#define Modem_TX_CTL                    (* (reg8 *) Modem_TX__CTL)
/* Drive Modes */
#define Modem_TX_DM0                    (* (reg8 *) Modem_TX__DM0) 
#define Modem_TX_DM1                    (* (reg8 *) Modem_TX__DM1)
#define Modem_TX_DM2                    (* (reg8 *) Modem_TX__DM2) 
/* Input Buffer Disable Override */
#define Modem_TX_INP_DIS                (* (reg8 *) Modem_TX__INP_DIS)
/* LCD Common or Segment Drive */
#define Modem_TX_LCD_COM_SEG            (* (reg8 *) Modem_TX__LCD_COM_SEG)
/* Enable Segment LCD */
#define Modem_TX_LCD_EN                 (* (reg8 *) Modem_TX__LCD_EN)
/* Slew Rate Control */
#define Modem_TX_SLW                    (* (reg8 *) Modem_TX__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Modem_TX_PRTDSI__CAPS_SEL       (* (reg8 *) Modem_TX__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Modem_TX_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Modem_TX__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Modem_TX_PRTDSI__OE_SEL0        (* (reg8 *) Modem_TX__PRTDSI__OE_SEL0) 
#define Modem_TX_PRTDSI__OE_SEL1        (* (reg8 *) Modem_TX__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Modem_TX_PRTDSI__OUT_SEL0       (* (reg8 *) Modem_TX__PRTDSI__OUT_SEL0) 
#define Modem_TX_PRTDSI__OUT_SEL1       (* (reg8 *) Modem_TX__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Modem_TX_PRTDSI__SYNC_OUT       (* (reg8 *) Modem_TX__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Modem_TX__SIO_CFG)
    #define Modem_TX_SIO_HYST_EN        (* (reg8 *) Modem_TX__SIO_HYST_EN)
    #define Modem_TX_SIO_REG_HIFREQ     (* (reg8 *) Modem_TX__SIO_REG_HIFREQ)
    #define Modem_TX_SIO_CFG            (* (reg8 *) Modem_TX__SIO_CFG)
    #define Modem_TX_SIO_DIFF           (* (reg8 *) Modem_TX__SIO_DIFF)
#endif /* (Modem_TX__SIO_CFG) */

/* Interrupt Registers */
#if defined(Modem_TX__INTSTAT)
    #define Modem_TX_INTSTAT            (* (reg8 *) Modem_TX__INTSTAT)
    #define Modem_TX_SNAP               (* (reg8 *) Modem_TX__SNAP)
    
	#define Modem_TX_0_INTTYPE_REG 		(* (reg8 *) Modem_TX__0__INTTYPE)
#endif /* (Modem_TX__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Modem_TX_H */


/* [] END OF FILE */
