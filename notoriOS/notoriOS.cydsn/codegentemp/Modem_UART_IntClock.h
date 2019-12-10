/*******************************************************************************
* File Name: Modem_UART_IntClock.h
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

#if !defined(CY_CLOCK_Modem_UART_IntClock_H)
#define CY_CLOCK_Modem_UART_IntClock_H

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

void Modem_UART_IntClock_Start(void) ;
void Modem_UART_IntClock_Stop(void) ;

#if(CY_PSOC3 || CY_PSOC5LP)
void Modem_UART_IntClock_StopBlock(void) ;
#endif /* (CY_PSOC3 || CY_PSOC5LP) */

void Modem_UART_IntClock_StandbyPower(uint8 state) ;
void Modem_UART_IntClock_SetDividerRegister(uint16 clkDivider, uint8 restart) 
                                ;
uint16 Modem_UART_IntClock_GetDividerRegister(void) ;
void Modem_UART_IntClock_SetModeRegister(uint8 modeBitMask) ;
void Modem_UART_IntClock_ClearModeRegister(uint8 modeBitMask) ;
uint8 Modem_UART_IntClock_GetModeRegister(void) ;
void Modem_UART_IntClock_SetSourceRegister(uint8 clkSource) ;
uint8 Modem_UART_IntClock_GetSourceRegister(void) ;
#if defined(Modem_UART_IntClock__CFG3)
void Modem_UART_IntClock_SetPhaseRegister(uint8 clkPhase) ;
uint8 Modem_UART_IntClock_GetPhaseRegister(void) ;
#endif /* defined(Modem_UART_IntClock__CFG3) */

#define Modem_UART_IntClock_Enable()                       Modem_UART_IntClock_Start()
#define Modem_UART_IntClock_Disable()                      Modem_UART_IntClock_Stop()
#define Modem_UART_IntClock_SetDivider(clkDivider)         Modem_UART_IntClock_SetDividerRegister(clkDivider, 1u)
#define Modem_UART_IntClock_SetDividerValue(clkDivider)    Modem_UART_IntClock_SetDividerRegister((clkDivider) - 1u, 1u)
#define Modem_UART_IntClock_SetMode(clkMode)               Modem_UART_IntClock_SetModeRegister(clkMode)
#define Modem_UART_IntClock_SetSource(clkSource)           Modem_UART_IntClock_SetSourceRegister(clkSource)
#if defined(Modem_UART_IntClock__CFG3)
#define Modem_UART_IntClock_SetPhase(clkPhase)             Modem_UART_IntClock_SetPhaseRegister(clkPhase)
#define Modem_UART_IntClock_SetPhaseValue(clkPhase)        Modem_UART_IntClock_SetPhaseRegister((clkPhase) + 1u)
#endif /* defined(Modem_UART_IntClock__CFG3) */


/***************************************
*             Registers
***************************************/

/* Register to enable or disable the clock */
#define Modem_UART_IntClock_CLKEN              (* (reg8 *) Modem_UART_IntClock__PM_ACT_CFG)
#define Modem_UART_IntClock_CLKEN_PTR          ((reg8 *) Modem_UART_IntClock__PM_ACT_CFG)

/* Register to enable or disable the clock */
#define Modem_UART_IntClock_CLKSTBY            (* (reg8 *) Modem_UART_IntClock__PM_STBY_CFG)
#define Modem_UART_IntClock_CLKSTBY_PTR        ((reg8 *) Modem_UART_IntClock__PM_STBY_CFG)

/* Clock LSB divider configuration register. */
#define Modem_UART_IntClock_DIV_LSB            (* (reg8 *) Modem_UART_IntClock__CFG0)
#define Modem_UART_IntClock_DIV_LSB_PTR        ((reg8 *) Modem_UART_IntClock__CFG0)
#define Modem_UART_IntClock_DIV_PTR            ((reg16 *) Modem_UART_IntClock__CFG0)

/* Clock MSB divider configuration register. */
#define Modem_UART_IntClock_DIV_MSB            (* (reg8 *) Modem_UART_IntClock__CFG1)
#define Modem_UART_IntClock_DIV_MSB_PTR        ((reg8 *) Modem_UART_IntClock__CFG1)

/* Mode and source configuration register */
#define Modem_UART_IntClock_MOD_SRC            (* (reg8 *) Modem_UART_IntClock__CFG2)
#define Modem_UART_IntClock_MOD_SRC_PTR        ((reg8 *) Modem_UART_IntClock__CFG2)

#if defined(Modem_UART_IntClock__CFG3)
/* Analog clock phase configuration register */
#define Modem_UART_IntClock_PHASE              (* (reg8 *) Modem_UART_IntClock__CFG3)
#define Modem_UART_IntClock_PHASE_PTR          ((reg8 *) Modem_UART_IntClock__CFG3)
#endif /* defined(Modem_UART_IntClock__CFG3) */


/**************************************
*       Register Constants
**************************************/

/* Power manager register masks */
#define Modem_UART_IntClock_CLKEN_MASK         Modem_UART_IntClock__PM_ACT_MSK
#define Modem_UART_IntClock_CLKSTBY_MASK       Modem_UART_IntClock__PM_STBY_MSK

/* CFG2 field masks */
#define Modem_UART_IntClock_SRC_SEL_MSK        Modem_UART_IntClock__CFG2_SRC_SEL_MASK
#define Modem_UART_IntClock_MODE_MASK          (~(Modem_UART_IntClock_SRC_SEL_MSK))

#if defined(Modem_UART_IntClock__CFG3)
/* CFG3 phase mask */
#define Modem_UART_IntClock_PHASE_MASK         Modem_UART_IntClock__CFG3_PHASE_DLY_MASK
#endif /* defined(Modem_UART_IntClock__CFG3) */

#endif /* CY_CLOCK_Modem_UART_IntClock_H */


/* [] END OF FILE */
