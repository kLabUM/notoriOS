/*******************************************************************************
* File Name: Downstream_Clock_IMO.h
* Version 2.20
*
*  Description:
*   Provides the function and constant definitions for the clock component.
*
*  Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_CLOCK_Downstream_Clock_IMO_H)
#define CY_CLOCK_Downstream_Clock_IMO_H

#include <cytypes.h>
#include <cyfitter.h>


/***************************************
* Conditional Compilation Parameters
***************************************/

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component cy_clock_v2_20 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */


/***************************************
*        Function Prototypes
***************************************/

void Downstream_Clock_IMO_Start(void) ;
void Downstream_Clock_IMO_Stop(void) ;

#if(CY_PSOC3 || CY_PSOC5LP)
void Downstream_Clock_IMO_StopBlock(void) ;
#endif /* (CY_PSOC3 || CY_PSOC5LP) */

void Downstream_Clock_IMO_StandbyPower(uint8 state) ;
void Downstream_Clock_IMO_SetDividerRegister(uint16 clkDivider, uint8 restart) 
                                ;
uint16 Downstream_Clock_IMO_GetDividerRegister(void) ;
void Downstream_Clock_IMO_SetModeRegister(uint8 modeBitMask) ;
void Downstream_Clock_IMO_ClearModeRegister(uint8 modeBitMask) ;
uint8 Downstream_Clock_IMO_GetModeRegister(void) ;
void Downstream_Clock_IMO_SetSourceRegister(uint8 clkSource) ;
uint8 Downstream_Clock_IMO_GetSourceRegister(void) ;
#if defined(Downstream_Clock_IMO__CFG3)
void Downstream_Clock_IMO_SetPhaseRegister(uint8 clkPhase) ;
uint8 Downstream_Clock_IMO_GetPhaseRegister(void) ;
#endif /* defined(Downstream_Clock_IMO__CFG3) */

#define Downstream_Clock_IMO_Enable()                       Downstream_Clock_IMO_Start()
#define Downstream_Clock_IMO_Disable()                      Downstream_Clock_IMO_Stop()
#define Downstream_Clock_IMO_SetDivider(clkDivider)         Downstream_Clock_IMO_SetDividerRegister(clkDivider, 1u)
#define Downstream_Clock_IMO_SetDividerValue(clkDivider)    Downstream_Clock_IMO_SetDividerRegister((clkDivider) - 1u, 1u)
#define Downstream_Clock_IMO_SetMode(clkMode)               Downstream_Clock_IMO_SetModeRegister(clkMode)
#define Downstream_Clock_IMO_SetSource(clkSource)           Downstream_Clock_IMO_SetSourceRegister(clkSource)
#if defined(Downstream_Clock_IMO__CFG3)
#define Downstream_Clock_IMO_SetPhase(clkPhase)             Downstream_Clock_IMO_SetPhaseRegister(clkPhase)
#define Downstream_Clock_IMO_SetPhaseValue(clkPhase)        Downstream_Clock_IMO_SetPhaseRegister((clkPhase) + 1u)
#endif /* defined(Downstream_Clock_IMO__CFG3) */


/***************************************
*             Registers
***************************************/

/* Register to enable or disable the clock */
#define Downstream_Clock_IMO_CLKEN              (* (reg8 *) Downstream_Clock_IMO__PM_ACT_CFG)
#define Downstream_Clock_IMO_CLKEN_PTR          ((reg8 *) Downstream_Clock_IMO__PM_ACT_CFG)

/* Register to enable or disable the clock */
#define Downstream_Clock_IMO_CLKSTBY            (* (reg8 *) Downstream_Clock_IMO__PM_STBY_CFG)
#define Downstream_Clock_IMO_CLKSTBY_PTR        ((reg8 *) Downstream_Clock_IMO__PM_STBY_CFG)

/* Clock LSB divider configuration register. */
#define Downstream_Clock_IMO_DIV_LSB            (* (reg8 *) Downstream_Clock_IMO__CFG0)
#define Downstream_Clock_IMO_DIV_LSB_PTR        ((reg8 *) Downstream_Clock_IMO__CFG0)
#define Downstream_Clock_IMO_DIV_PTR            ((reg16 *) Downstream_Clock_IMO__CFG0)

/* Clock MSB divider configuration register. */
#define Downstream_Clock_IMO_DIV_MSB            (* (reg8 *) Downstream_Clock_IMO__CFG1)
#define Downstream_Clock_IMO_DIV_MSB_PTR        ((reg8 *) Downstream_Clock_IMO__CFG1)

/* Mode and source configuration register */
#define Downstream_Clock_IMO_MOD_SRC            (* (reg8 *) Downstream_Clock_IMO__CFG2)
#define Downstream_Clock_IMO_MOD_SRC_PTR        ((reg8 *) Downstream_Clock_IMO__CFG2)

#if defined(Downstream_Clock_IMO__CFG3)
/* Analog clock phase configuration register */
#define Downstream_Clock_IMO_PHASE              (* (reg8 *) Downstream_Clock_IMO__CFG3)
#define Downstream_Clock_IMO_PHASE_PTR          ((reg8 *) Downstream_Clock_IMO__CFG3)
#endif /* defined(Downstream_Clock_IMO__CFG3) */


/**************************************
*       Register Constants
**************************************/

/* Power manager register masks */
#define Downstream_Clock_IMO_CLKEN_MASK         Downstream_Clock_IMO__PM_ACT_MSK
#define Downstream_Clock_IMO_CLKSTBY_MASK       Downstream_Clock_IMO__PM_STBY_MSK

/* CFG2 field masks */
#define Downstream_Clock_IMO_SRC_SEL_MSK        Downstream_Clock_IMO__CFG2_SRC_SEL_MASK
#define Downstream_Clock_IMO_MODE_MASK          (~(Downstream_Clock_IMO_SRC_SEL_MSK))

#if defined(Downstream_Clock_IMO__CFG3)
/* CFG3 phase mask */
#define Downstream_Clock_IMO_PHASE_MASK         Downstream_Clock_IMO__CFG3_PHASE_DLY_MASK
#endif /* defined(Downstream_Clock_IMO__CFG3) */

#endif /* CY_CLOCK_Downstream_Clock_IMO_H */


/* [] END OF FILE */
