/*******************************************************************************
* File Name: Downstream_Level_Sensor_Power.h  
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

#if !defined(CY_PINS_Downstream_Level_Sensor_Power_H) /* Pins Downstream_Level_Sensor_Power_H */
#define CY_PINS_Downstream_Level_Sensor_Power_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Downstream_Level_Sensor_Power_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Downstream_Level_Sensor_Power__PORT == 15 && ((Downstream_Level_Sensor_Power__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Downstream_Level_Sensor_Power_Write(uint8 value);
void    Downstream_Level_Sensor_Power_SetDriveMode(uint8 mode);
uint8   Downstream_Level_Sensor_Power_ReadDataReg(void);
uint8   Downstream_Level_Sensor_Power_Read(void);
void    Downstream_Level_Sensor_Power_SetInterruptMode(uint16 position, uint16 mode);
uint8   Downstream_Level_Sensor_Power_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Downstream_Level_Sensor_Power_SetDriveMode() function.
     *  @{
     */
        #define Downstream_Level_Sensor_Power_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Downstream_Level_Sensor_Power_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Downstream_Level_Sensor_Power_DM_RES_UP          PIN_DM_RES_UP
        #define Downstream_Level_Sensor_Power_DM_RES_DWN         PIN_DM_RES_DWN
        #define Downstream_Level_Sensor_Power_DM_OD_LO           PIN_DM_OD_LO
        #define Downstream_Level_Sensor_Power_DM_OD_HI           PIN_DM_OD_HI
        #define Downstream_Level_Sensor_Power_DM_STRONG          PIN_DM_STRONG
        #define Downstream_Level_Sensor_Power_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Downstream_Level_Sensor_Power_MASK               Downstream_Level_Sensor_Power__MASK
#define Downstream_Level_Sensor_Power_SHIFT              Downstream_Level_Sensor_Power__SHIFT
#define Downstream_Level_Sensor_Power_WIDTH              1u

/* Interrupt constants */
#if defined(Downstream_Level_Sensor_Power__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Downstream_Level_Sensor_Power_SetInterruptMode() function.
     *  @{
     */
        #define Downstream_Level_Sensor_Power_INTR_NONE      (uint16)(0x0000u)
        #define Downstream_Level_Sensor_Power_INTR_RISING    (uint16)(0x0001u)
        #define Downstream_Level_Sensor_Power_INTR_FALLING   (uint16)(0x0002u)
        #define Downstream_Level_Sensor_Power_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Downstream_Level_Sensor_Power_INTR_MASK      (0x01u) 
#endif /* (Downstream_Level_Sensor_Power__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Downstream_Level_Sensor_Power_PS                     (* (reg8 *) Downstream_Level_Sensor_Power__PS)
/* Data Register */
#define Downstream_Level_Sensor_Power_DR                     (* (reg8 *) Downstream_Level_Sensor_Power__DR)
/* Port Number */
#define Downstream_Level_Sensor_Power_PRT_NUM                (* (reg8 *) Downstream_Level_Sensor_Power__PRT) 
/* Connect to Analog Globals */                                                  
#define Downstream_Level_Sensor_Power_AG                     (* (reg8 *) Downstream_Level_Sensor_Power__AG)                       
/* Analog MUX bux enable */
#define Downstream_Level_Sensor_Power_AMUX                   (* (reg8 *) Downstream_Level_Sensor_Power__AMUX) 
/* Bidirectional Enable */                                                        
#define Downstream_Level_Sensor_Power_BIE                    (* (reg8 *) Downstream_Level_Sensor_Power__BIE)
/* Bit-mask for Aliased Register Access */
#define Downstream_Level_Sensor_Power_BIT_MASK               (* (reg8 *) Downstream_Level_Sensor_Power__BIT_MASK)
/* Bypass Enable */
#define Downstream_Level_Sensor_Power_BYP                    (* (reg8 *) Downstream_Level_Sensor_Power__BYP)
/* Port wide control signals */                                                   
#define Downstream_Level_Sensor_Power_CTL                    (* (reg8 *) Downstream_Level_Sensor_Power__CTL)
/* Drive Modes */
#define Downstream_Level_Sensor_Power_DM0                    (* (reg8 *) Downstream_Level_Sensor_Power__DM0) 
#define Downstream_Level_Sensor_Power_DM1                    (* (reg8 *) Downstream_Level_Sensor_Power__DM1)
#define Downstream_Level_Sensor_Power_DM2                    (* (reg8 *) Downstream_Level_Sensor_Power__DM2) 
/* Input Buffer Disable Override */
#define Downstream_Level_Sensor_Power_INP_DIS                (* (reg8 *) Downstream_Level_Sensor_Power__INP_DIS)
/* LCD Common or Segment Drive */
#define Downstream_Level_Sensor_Power_LCD_COM_SEG            (* (reg8 *) Downstream_Level_Sensor_Power__LCD_COM_SEG)
/* Enable Segment LCD */
#define Downstream_Level_Sensor_Power_LCD_EN                 (* (reg8 *) Downstream_Level_Sensor_Power__LCD_EN)
/* Slew Rate Control */
#define Downstream_Level_Sensor_Power_SLW                    (* (reg8 *) Downstream_Level_Sensor_Power__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Downstream_Level_Sensor_Power_PRTDSI__CAPS_SEL       (* (reg8 *) Downstream_Level_Sensor_Power__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Downstream_Level_Sensor_Power_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Downstream_Level_Sensor_Power__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Downstream_Level_Sensor_Power_PRTDSI__OE_SEL0        (* (reg8 *) Downstream_Level_Sensor_Power__PRTDSI__OE_SEL0) 
#define Downstream_Level_Sensor_Power_PRTDSI__OE_SEL1        (* (reg8 *) Downstream_Level_Sensor_Power__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Downstream_Level_Sensor_Power_PRTDSI__OUT_SEL0       (* (reg8 *) Downstream_Level_Sensor_Power__PRTDSI__OUT_SEL0) 
#define Downstream_Level_Sensor_Power_PRTDSI__OUT_SEL1       (* (reg8 *) Downstream_Level_Sensor_Power__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Downstream_Level_Sensor_Power_PRTDSI__SYNC_OUT       (* (reg8 *) Downstream_Level_Sensor_Power__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Downstream_Level_Sensor_Power__SIO_CFG)
    #define Downstream_Level_Sensor_Power_SIO_HYST_EN        (* (reg8 *) Downstream_Level_Sensor_Power__SIO_HYST_EN)
    #define Downstream_Level_Sensor_Power_SIO_REG_HIFREQ     (* (reg8 *) Downstream_Level_Sensor_Power__SIO_REG_HIFREQ)
    #define Downstream_Level_Sensor_Power_SIO_CFG            (* (reg8 *) Downstream_Level_Sensor_Power__SIO_CFG)
    #define Downstream_Level_Sensor_Power_SIO_DIFF           (* (reg8 *) Downstream_Level_Sensor_Power__SIO_DIFF)
#endif /* (Downstream_Level_Sensor_Power__SIO_CFG) */

/* Interrupt Registers */
#if defined(Downstream_Level_Sensor_Power__INTSTAT)
    #define Downstream_Level_Sensor_Power_INTSTAT            (* (reg8 *) Downstream_Level_Sensor_Power__INTSTAT)
    #define Downstream_Level_Sensor_Power_SNAP               (* (reg8 *) Downstream_Level_Sensor_Power__SNAP)
    
	#define Downstream_Level_Sensor_Power_0_INTTYPE_REG 		(* (reg8 *) Downstream_Level_Sensor_Power__0__INTTYPE)
#endif /* (Downstream_Level_Sensor_Power__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Downstream_Level_Sensor_Power_H */


/* [] END OF FILE */
