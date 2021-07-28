/*******************************************************************************
* File Name: DO_UART_IntClock.h
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

#if !defined(CY_CLOCK_DO_UART_IntClock_H)
#define CY_CLOCK_DO_UART_IntClock_H

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

void DO_UART_IntClock_Start(void) ;
void DO_UART_IntClock_Stop(void) ;

#if(CY_PSOC3 || CY_PSOC5LP)
void DO_UART_IntClock_StopBlock(void) ;
#endif /* (CY_PSOC3 || CY_PSOC5LP) */

void DO_UART_IntClock_StandbyPower(uint8 state) ;
void DO_UART_IntClock_SetDividerRegister(uint16 clkDivider, uint8 restart) 
                                ;
uint16 DO_UART_IntClock_GetDividerRegister(void) ;
void DO_UART_IntClock_SetModeRegister(uint8 modeBitMask) ;
void DO_UART_IntClock_ClearModeRegister(uint8 modeBitMask) ;
uint8 DO_UART_IntClock_GetModeRegister(void) ;
void DO_UART_IntClock_SetSourceRegister(uint8 clkSource) ;
uint8 DO_UART_IntClock_GetSourceRegister(void) ;
#if defined(DO_UART_IntClock__CFG3)
void DO_UART_IntClock_SetPhaseRegister(uint8 clkPhase) ;
uint8 DO_UART_IntClock_GetPhaseRegister(void) ;
#endif /* defined(DO_UART_IntClock__CFG3) */

#define DO_UART_IntClock_Enable()                       DO_UART_IntClock_Start()
#define DO_UART_IntClock_Disable()                      DO_UART_IntClock_Stop()
#define DO_UART_IntClock_SetDivider(clkDivider)         DO_UART_IntClock_SetDividerRegister(clkDivider, 1u)
#define DO_UART_IntClock_SetDividerValue(clkDivider)    DO_UART_IntClock_SetDividerRegister((clkDivider) - 1u, 1u)
#define DO_UART_IntClock_SetMode(clkMode)               DO_UART_IntClock_SetModeRegister(clkMode)
#define DO_UART_IntClock_SetSource(clkSource)           DO_UART_IntClock_SetSourceRegister(clkSource)
#if defined(DO_UART_IntClock__CFG3)
#define DO_UART_IntClock_SetPhase(clkPhase)             DO_UART_IntClock_SetPhaseRegister(clkPhase)
#define DO_UART_IntClock_SetPhaseValue(clkPhase)        DO_UART_IntClock_SetPhaseRegister((clkPhase) + 1u)
#endif /* defined(DO_UART_IntClock__CFG3) */


/***************************************
*             Registers
***************************************/

/* Register to enable or disable the clock */
#define DO_UART_IntClock_CLKEN              (* (reg8 *) DO_UART_IntClock__PM_ACT_CFG)
#define DO_UART_IntClock_CLKEN_PTR          ((reg8 *) DO_UART_IntClock__PM_ACT_CFG)

/* Register to enable or disable the clock */
#define DO_UART_IntClock_CLKSTBY            (* (reg8 *) DO_UART_IntClock__PM_STBY_CFG)
#define DO_UART_IntClock_CLKSTBY_PTR        ((reg8 *) DO_UART_IntClock__PM_STBY_CFG)

/* Clock LSB divider configuration register. */
#define DO_UART_IntClock_DIV_LSB            (* (reg8 *) DO_UART_IntClock__CFG0)
#define DO_UART_IntClock_DIV_LSB_PTR        ((reg8 *) DO_UART_IntClock__CFG0)
#define DO_UART_IntClock_DIV_PTR            ((reg16 *) DO_UART_IntClock__CFG0)

/* Clock MSB divider configuration register. */
#define DO_UART_IntClock_DIV_MSB            (* (reg8 *) DO_UART_IntClock__CFG1)
#define DO_UART_IntClock_DIV_MSB_PTR        ((reg8 *) DO_UART_IntClock__CFG1)

/* Mode and source configuration register */
#define DO_UART_IntClock_MOD_SRC            (* (reg8 *) DO_UART_IntClock__CFG2)
#define DO_UART_IntClock_MOD_SRC_PTR        ((reg8 *) DO_UART_IntClock__CFG2)

#if defined(DO_UART_IntClock__CFG3)
/* Analog clock phase configuration register */
#define DO_UART_IntClock_PHASE              (* (reg8 *) DO_UART_IntClock__CFG3)
#define DO_UART_IntClock_PHASE_PTR          ((reg8 *) DO_UART_IntClock__CFG3)
#endif /* defined(DO_UART_IntClock__CFG3) */


/**************************************
*       Register Constants
**************************************/

/* Power manager register masks */
#define DO_UART_IntClock_CLKEN_MASK         DO_UART_IntClock__PM_ACT_MSK
#define DO_UART_IntClock_CLKSTBY_MASK       DO_UART_IntClock__PM_STBY_MSK

/* CFG2 field masks */
#define DO_UART_IntClock_SRC_SEL_MSK        DO_UART_IntClock__CFG2_SRC_SEL_MASK
#define DO_UART_IntClock_MODE_MASK          (~(DO_UART_IntClock_SRC_SEL_MSK))

#if defined(DO_UART_IntClock__CFG3)
/* CFG3 phase mask */
#define DO_UART_IntClock_PHASE_MASK         DO_UART_IntClock__CFG3_PHASE_DLY_MASK
#endif /* defined(DO_UART_IntClock__CFG3) */

#endif /* CY_CLOCK_DO_UART_IntClock_H */


/* [] END OF FILE */
