/*******************************************************************************
* File Name: Sensors_UART_IntClock.h
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

#if !defined(CY_CLOCK_Sensors_UART_IntClock_H)
#define CY_CLOCK_Sensors_UART_IntClock_H

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

void Sensors_UART_IntClock_Start(void) ;
void Sensors_UART_IntClock_Stop(void) ;

#if(CY_PSOC3 || CY_PSOC5LP)
void Sensors_UART_IntClock_StopBlock(void) ;
#endif /* (CY_PSOC3 || CY_PSOC5LP) */

void Sensors_UART_IntClock_StandbyPower(uint8 state) ;
void Sensors_UART_IntClock_SetDividerRegister(uint16 clkDivider, uint8 restart) 
                                ;
uint16 Sensors_UART_IntClock_GetDividerRegister(void) ;
void Sensors_UART_IntClock_SetModeRegister(uint8 modeBitMask) ;
void Sensors_UART_IntClock_ClearModeRegister(uint8 modeBitMask) ;
uint8 Sensors_UART_IntClock_GetModeRegister(void) ;
void Sensors_UART_IntClock_SetSourceRegister(uint8 clkSource) ;
uint8 Sensors_UART_IntClock_GetSourceRegister(void) ;
#if defined(Sensors_UART_IntClock__CFG3)
void Sensors_UART_IntClock_SetPhaseRegister(uint8 clkPhase) ;
uint8 Sensors_UART_IntClock_GetPhaseRegister(void) ;
#endif /* defined(Sensors_UART_IntClock__CFG3) */

#define Sensors_UART_IntClock_Enable()                       Sensors_UART_IntClock_Start()
#define Sensors_UART_IntClock_Disable()                      Sensors_UART_IntClock_Stop()
#define Sensors_UART_IntClock_SetDivider(clkDivider)         Sensors_UART_IntClock_SetDividerRegister(clkDivider, 1u)
#define Sensors_UART_IntClock_SetDividerValue(clkDivider)    Sensors_UART_IntClock_SetDividerRegister((clkDivider) - 1u, 1u)
#define Sensors_UART_IntClock_SetMode(clkMode)               Sensors_UART_IntClock_SetModeRegister(clkMode)
#define Sensors_UART_IntClock_SetSource(clkSource)           Sensors_UART_IntClock_SetSourceRegister(clkSource)
#if defined(Sensors_UART_IntClock__CFG3)
#define Sensors_UART_IntClock_SetPhase(clkPhase)             Sensors_UART_IntClock_SetPhaseRegister(clkPhase)
#define Sensors_UART_IntClock_SetPhaseValue(clkPhase)        Sensors_UART_IntClock_SetPhaseRegister((clkPhase) + 1u)
#endif /* defined(Sensors_UART_IntClock__CFG3) */


/***************************************
*             Registers
***************************************/

/* Register to enable or disable the clock */
#define Sensors_UART_IntClock_CLKEN              (* (reg8 *) Sensors_UART_IntClock__PM_ACT_CFG)
#define Sensors_UART_IntClock_CLKEN_PTR          ((reg8 *) Sensors_UART_IntClock__PM_ACT_CFG)

/* Register to enable or disable the clock */
#define Sensors_UART_IntClock_CLKSTBY            (* (reg8 *) Sensors_UART_IntClock__PM_STBY_CFG)
#define Sensors_UART_IntClock_CLKSTBY_PTR        ((reg8 *) Sensors_UART_IntClock__PM_STBY_CFG)

/* Clock LSB divider configuration register. */
#define Sensors_UART_IntClock_DIV_LSB            (* (reg8 *) Sensors_UART_IntClock__CFG0)
#define Sensors_UART_IntClock_DIV_LSB_PTR        ((reg8 *) Sensors_UART_IntClock__CFG0)
#define Sensors_UART_IntClock_DIV_PTR            ((reg16 *) Sensors_UART_IntClock__CFG0)

/* Clock MSB divider configuration register. */
#define Sensors_UART_IntClock_DIV_MSB            (* (reg8 *) Sensors_UART_IntClock__CFG1)
#define Sensors_UART_IntClock_DIV_MSB_PTR        ((reg8 *) Sensors_UART_IntClock__CFG1)

/* Mode and source configuration register */
#define Sensors_UART_IntClock_MOD_SRC            (* (reg8 *) Sensors_UART_IntClock__CFG2)
#define Sensors_UART_IntClock_MOD_SRC_PTR        ((reg8 *) Sensors_UART_IntClock__CFG2)

#if defined(Sensors_UART_IntClock__CFG3)
/* Analog clock phase configuration register */
#define Sensors_UART_IntClock_PHASE              (* (reg8 *) Sensors_UART_IntClock__CFG3)
#define Sensors_UART_IntClock_PHASE_PTR          ((reg8 *) Sensors_UART_IntClock__CFG3)
#endif /* defined(Sensors_UART_IntClock__CFG3) */


/**************************************
*       Register Constants
**************************************/

/* Power manager register masks */
#define Sensors_UART_IntClock_CLKEN_MASK         Sensors_UART_IntClock__PM_ACT_MSK
#define Sensors_UART_IntClock_CLKSTBY_MASK       Sensors_UART_IntClock__PM_STBY_MSK

/* CFG2 field masks */
#define Sensors_UART_IntClock_SRC_SEL_MSK        Sensors_UART_IntClock__CFG2_SRC_SEL_MASK
#define Sensors_UART_IntClock_MODE_MASK          (~(Sensors_UART_IntClock_SRC_SEL_MSK))

#if defined(Sensors_UART_IntClock__CFG3)
/* CFG3 phase mask */
#define Sensors_UART_IntClock_PHASE_MASK         Sensors_UART_IntClock__CFG3_PHASE_DLY_MASK
#endif /* defined(Sensors_UART_IntClock__CFG3) */

#endif /* CY_CLOCK_Sensors_UART_IntClock_H */


/* [] END OF FILE */
