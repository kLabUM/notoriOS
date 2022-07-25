/*******************************************************************************
* File Name: FastClock.h
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

#if !defined(CY_CLOCK_FastClock_H)
#define CY_CLOCK_FastClock_H

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

void FastClock_Start(void) ;
void FastClock_Stop(void) ;

#if(CY_PSOC3 || CY_PSOC5LP)
void FastClock_StopBlock(void) ;
#endif /* (CY_PSOC3 || CY_PSOC5LP) */

void FastClock_StandbyPower(uint8 state) ;
void FastClock_SetDividerRegister(uint16 clkDivider, uint8 restart) 
                                ;
uint16 FastClock_GetDividerRegister(void) ;
void FastClock_SetModeRegister(uint8 modeBitMask) ;
void FastClock_ClearModeRegister(uint8 modeBitMask) ;
uint8 FastClock_GetModeRegister(void) ;
void FastClock_SetSourceRegister(uint8 clkSource) ;
uint8 FastClock_GetSourceRegister(void) ;
#if defined(FastClock__CFG3)
void FastClock_SetPhaseRegister(uint8 clkPhase) ;
uint8 FastClock_GetPhaseRegister(void) ;
#endif /* defined(FastClock__CFG3) */

#define FastClock_Enable()                       FastClock_Start()
#define FastClock_Disable()                      FastClock_Stop()
#define FastClock_SetDivider(clkDivider)         FastClock_SetDividerRegister(clkDivider, 1u)
#define FastClock_SetDividerValue(clkDivider)    FastClock_SetDividerRegister((clkDivider) - 1u, 1u)
#define FastClock_SetMode(clkMode)               FastClock_SetModeRegister(clkMode)
#define FastClock_SetSource(clkSource)           FastClock_SetSourceRegister(clkSource)
#if defined(FastClock__CFG3)
#define FastClock_SetPhase(clkPhase)             FastClock_SetPhaseRegister(clkPhase)
#define FastClock_SetPhaseValue(clkPhase)        FastClock_SetPhaseRegister((clkPhase) + 1u)
#endif /* defined(FastClock__CFG3) */


/***************************************
*             Registers
***************************************/

/* Register to enable or disable the clock */
#define FastClock_CLKEN              (* (reg8 *) FastClock__PM_ACT_CFG)
#define FastClock_CLKEN_PTR          ((reg8 *) FastClock__PM_ACT_CFG)

/* Register to enable or disable the clock */
#define FastClock_CLKSTBY            (* (reg8 *) FastClock__PM_STBY_CFG)
#define FastClock_CLKSTBY_PTR        ((reg8 *) FastClock__PM_STBY_CFG)

/* Clock LSB divider configuration register. */
#define FastClock_DIV_LSB            (* (reg8 *) FastClock__CFG0)
#define FastClock_DIV_LSB_PTR        ((reg8 *) FastClock__CFG0)
#define FastClock_DIV_PTR            ((reg16 *) FastClock__CFG0)

/* Clock MSB divider configuration register. */
#define FastClock_DIV_MSB            (* (reg8 *) FastClock__CFG1)
#define FastClock_DIV_MSB_PTR        ((reg8 *) FastClock__CFG1)

/* Mode and source configuration register */
#define FastClock_MOD_SRC            (* (reg8 *) FastClock__CFG2)
#define FastClock_MOD_SRC_PTR        ((reg8 *) FastClock__CFG2)

#if defined(FastClock__CFG3)
/* Analog clock phase configuration register */
#define FastClock_PHASE              (* (reg8 *) FastClock__CFG3)
#define FastClock_PHASE_PTR          ((reg8 *) FastClock__CFG3)
#endif /* defined(FastClock__CFG3) */


/**************************************
*       Register Constants
**************************************/

/* Power manager register masks */
#define FastClock_CLKEN_MASK         FastClock__PM_ACT_MSK
#define FastClock_CLKSTBY_MASK       FastClock__PM_STBY_MSK

/* CFG2 field masks */
#define FastClock_SRC_SEL_MSK        FastClock__CFG2_SRC_SEL_MASK
#define FastClock_MODE_MASK          (~(FastClock_SRC_SEL_MSK))

#if defined(FastClock__CFG3)
/* CFG3 phase mask */
#define FastClock_PHASE_MASK         FastClock__CFG3_PHASE_DLY_MASK
#endif /* defined(FastClock__CFG3) */

#endif /* CY_CLOCK_FastClock_H */


/* [] END OF FILE */
