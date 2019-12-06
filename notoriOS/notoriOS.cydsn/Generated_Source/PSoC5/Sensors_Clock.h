/*******************************************************************************
* File Name: Sensors_Clock.h
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

#if !defined(CY_CLOCK_Sensors_Clock_H)
#define CY_CLOCK_Sensors_Clock_H

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

void Sensors_Clock_Start(void) ;
void Sensors_Clock_Stop(void) ;

#if(CY_PSOC3 || CY_PSOC5LP)
void Sensors_Clock_StopBlock(void) ;
#endif /* (CY_PSOC3 || CY_PSOC5LP) */

void Sensors_Clock_StandbyPower(uint8 state) ;
void Sensors_Clock_SetDividerRegister(uint16 clkDivider, uint8 restart) 
                                ;
uint16 Sensors_Clock_GetDividerRegister(void) ;
void Sensors_Clock_SetModeRegister(uint8 modeBitMask) ;
void Sensors_Clock_ClearModeRegister(uint8 modeBitMask) ;
uint8 Sensors_Clock_GetModeRegister(void) ;
void Sensors_Clock_SetSourceRegister(uint8 clkSource) ;
uint8 Sensors_Clock_GetSourceRegister(void) ;
#if defined(Sensors_Clock__CFG3)
void Sensors_Clock_SetPhaseRegister(uint8 clkPhase) ;
uint8 Sensors_Clock_GetPhaseRegister(void) ;
#endif /* defined(Sensors_Clock__CFG3) */

#define Sensors_Clock_Enable()                       Sensors_Clock_Start()
#define Sensors_Clock_Disable()                      Sensors_Clock_Stop()
#define Sensors_Clock_SetDivider(clkDivider)         Sensors_Clock_SetDividerRegister(clkDivider, 1u)
#define Sensors_Clock_SetDividerValue(clkDivider)    Sensors_Clock_SetDividerRegister((clkDivider) - 1u, 1u)
#define Sensors_Clock_SetMode(clkMode)               Sensors_Clock_SetModeRegister(clkMode)
#define Sensors_Clock_SetSource(clkSource)           Sensors_Clock_SetSourceRegister(clkSource)
#if defined(Sensors_Clock__CFG3)
#define Sensors_Clock_SetPhase(clkPhase)             Sensors_Clock_SetPhaseRegister(clkPhase)
#define Sensors_Clock_SetPhaseValue(clkPhase)        Sensors_Clock_SetPhaseRegister((clkPhase) + 1u)
#endif /* defined(Sensors_Clock__CFG3) */


/***************************************
*             Registers
***************************************/

/* Register to enable or disable the clock */
#define Sensors_Clock_CLKEN              (* (reg8 *) Sensors_Clock__PM_ACT_CFG)
#define Sensors_Clock_CLKEN_PTR          ((reg8 *) Sensors_Clock__PM_ACT_CFG)

/* Register to enable or disable the clock */
#define Sensors_Clock_CLKSTBY            (* (reg8 *) Sensors_Clock__PM_STBY_CFG)
#define Sensors_Clock_CLKSTBY_PTR        ((reg8 *) Sensors_Clock__PM_STBY_CFG)

/* Clock LSB divider configuration register. */
#define Sensors_Clock_DIV_LSB            (* (reg8 *) Sensors_Clock__CFG0)
#define Sensors_Clock_DIV_LSB_PTR        ((reg8 *) Sensors_Clock__CFG0)
#define Sensors_Clock_DIV_PTR            ((reg16 *) Sensors_Clock__CFG0)

/* Clock MSB divider configuration register. */
#define Sensors_Clock_DIV_MSB            (* (reg8 *) Sensors_Clock__CFG1)
#define Sensors_Clock_DIV_MSB_PTR        ((reg8 *) Sensors_Clock__CFG1)

/* Mode and source configuration register */
#define Sensors_Clock_MOD_SRC            (* (reg8 *) Sensors_Clock__CFG2)
#define Sensors_Clock_MOD_SRC_PTR        ((reg8 *) Sensors_Clock__CFG2)

#if defined(Sensors_Clock__CFG3)
/* Analog clock phase configuration register */
#define Sensors_Clock_PHASE              (* (reg8 *) Sensors_Clock__CFG3)
#define Sensors_Clock_PHASE_PTR          ((reg8 *) Sensors_Clock__CFG3)
#endif /* defined(Sensors_Clock__CFG3) */


/**************************************
*       Register Constants
**************************************/

/* Power manager register masks */
#define Sensors_Clock_CLKEN_MASK         Sensors_Clock__PM_ACT_MSK
#define Sensors_Clock_CLKSTBY_MASK       Sensors_Clock__PM_STBY_MSK

/* CFG2 field masks */
#define Sensors_Clock_SRC_SEL_MSK        Sensors_Clock__CFG2_SRC_SEL_MASK
#define Sensors_Clock_MODE_MASK          (~(Sensors_Clock_SRC_SEL_MSK))

#if defined(Sensors_Clock__CFG3)
/* CFG3 phase mask */
#define Sensors_Clock_PHASE_MASK         Sensors_Clock__CFG3_PHASE_DLY_MASK
#endif /* defined(Sensors_Clock__CFG3) */

#endif /* CY_CLOCK_Sensors_Clock_H */


/* [] END OF FILE */
