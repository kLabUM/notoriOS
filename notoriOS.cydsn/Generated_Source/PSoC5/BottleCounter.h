/*******************************************************************************
* File Name: BottleCounter.h  
* Version 3.0
*
*  Description:
*   Contains the function prototypes and constants available to the counter
*   user module.
*
*   Note:
*    None
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/
    
#if !defined(CY_COUNTER_BottleCounter_H)
#define CY_COUNTER_BottleCounter_H

#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h" /* For CyEnterCriticalSection() and CyExitCriticalSection() functions */

extern uint8 BottleCounter_initVar;

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component Counter_v3_0 requires cy_boot v3.0 or later
#endif /* (CY_ PSOC5LP) */

/* Error message for removed BottleCounter_CounterUDB_sCTRLReg_ctrlreg through optimization */
#ifdef BottleCounter_CounterUDB_sCTRLReg_ctrlreg__REMOVED
    #error Counter_v3_0 detected with a constant 0 for the enable, a \
                                constant 0 for the count or constant 1 for \
                                the reset. This will prevent the component from\
                                operating.
#endif /* BottleCounter_CounterUDB_sCTRLReg_ctrlreg__REMOVED */


/**************************************
*           Parameter Defaults        
**************************************/

#define BottleCounter_Resolution            16u
#define BottleCounter_UsingFixedFunction    0u
#define BottleCounter_ControlRegRemoved     0u
#define BottleCounter_COMPARE_MODE_SOFTWARE 0u
#define BottleCounter_CAPTURE_MODE_SOFTWARE 0u
#define BottleCounter_RunModeUsed           0u


/***************************************
*       Type defines
***************************************/


/**************************************************************************
 * Sleep Mode API Support
 * Backup structure for Sleep Wake up operations
 *************************************************************************/
typedef struct
{
    uint8 CounterEnableState; 
    uint16 CounterUdb;         /* Current Counter Value */

    #if (!BottleCounter_ControlRegRemoved)
        uint8 CounterControlRegister;               /* Counter Control Register */
    #endif /* (!BottleCounter_ControlRegRemoved) */

}BottleCounter_backupStruct;


/**************************************
 *  Function Prototypes
 *************************************/
void    BottleCounter_Start(void) ;
void    BottleCounter_Stop(void) ;
void    BottleCounter_SetInterruptMode(uint8 interruptsMask) ;
uint8   BottleCounter_ReadStatusRegister(void) ;
#define BottleCounter_GetInterruptSource() BottleCounter_ReadStatusRegister()
#if(!BottleCounter_ControlRegRemoved)
    uint8   BottleCounter_ReadControlRegister(void) ;
    void    BottleCounter_WriteControlRegister(uint8 control) \
        ;
#endif /* (!BottleCounter_ControlRegRemoved) */
#if (!(BottleCounter_UsingFixedFunction && (CY_PSOC5A)))
    void    BottleCounter_WriteCounter(uint16 counter) \
            ; 
#endif /* (!(BottleCounter_UsingFixedFunction && (CY_PSOC5A))) */
uint16  BottleCounter_ReadCounter(void) ;
uint16  BottleCounter_ReadCapture(void) ;
void    BottleCounter_WritePeriod(uint16 period) \
    ;
uint16  BottleCounter_ReadPeriod( void ) ;
#if (!BottleCounter_UsingFixedFunction)
    void    BottleCounter_WriteCompare(uint16 compare) \
        ;
    uint16  BottleCounter_ReadCompare( void ) \
        ;
#endif /* (!BottleCounter_UsingFixedFunction) */

#if (BottleCounter_COMPARE_MODE_SOFTWARE)
    void    BottleCounter_SetCompareMode(uint8 compareMode) ;
#endif /* (BottleCounter_COMPARE_MODE_SOFTWARE) */
#if (BottleCounter_CAPTURE_MODE_SOFTWARE)
    void    BottleCounter_SetCaptureMode(uint8 captureMode) ;
#endif /* (BottleCounter_CAPTURE_MODE_SOFTWARE) */
void BottleCounter_ClearFIFO(void)     ;
void BottleCounter_Init(void)          ;
void BottleCounter_Enable(void)        ;
void BottleCounter_SaveConfig(void)    ;
void BottleCounter_RestoreConfig(void) ;
void BottleCounter_Sleep(void)         ;
void BottleCounter_Wakeup(void)        ;


/***************************************
*   Enumerated Types and Parameters
***************************************/

/* Enumerated Type B_Counter__CompareModes, Used in Compare Mode retained for backward compatibility of tests*/
#define BottleCounter__B_COUNTER__LESS_THAN 1
#define BottleCounter__B_COUNTER__LESS_THAN_OR_EQUAL 2
#define BottleCounter__B_COUNTER__EQUAL 0
#define BottleCounter__B_COUNTER__GREATER_THAN 3
#define BottleCounter__B_COUNTER__GREATER_THAN_OR_EQUAL 4
#define BottleCounter__B_COUNTER__SOFTWARE 5

/* Enumerated Type Counter_CompareModes */
#define BottleCounter_CMP_MODE_LT 1u
#define BottleCounter_CMP_MODE_LTE 2u
#define BottleCounter_CMP_MODE_EQ 0u
#define BottleCounter_CMP_MODE_GT 3u
#define BottleCounter_CMP_MODE_GTE 4u
#define BottleCounter_CMP_MODE_SOFTWARE_CONTROLLED 5u

/* Enumerated Type B_Counter__CaptureModes, Used in Capture Mode retained for backward compatibility of tests*/
#define BottleCounter__B_COUNTER__NONE 0
#define BottleCounter__B_COUNTER__RISING_EDGE 1
#define BottleCounter__B_COUNTER__FALLING_EDGE 2
#define BottleCounter__B_COUNTER__EITHER_EDGE 3
#define BottleCounter__B_COUNTER__SOFTWARE_CONTROL 4

/* Enumerated Type Counter_CompareModes */
#define BottleCounter_CAP_MODE_NONE 0u
#define BottleCounter_CAP_MODE_RISE 1u
#define BottleCounter_CAP_MODE_FALL 2u
#define BottleCounter_CAP_MODE_BOTH 3u
#define BottleCounter_CAP_MODE_SOFTWARE_CONTROLLED 4u


/***************************************
 *  Initialization Values
 **************************************/
#define BottleCounter_CAPTURE_MODE_CONF       0u
#define BottleCounter_INIT_PERIOD_VALUE       65534u
#define BottleCounter_INIT_COUNTER_VALUE      0u
#if (BottleCounter_UsingFixedFunction)
#define BottleCounter_INIT_INTERRUPTS_MASK    ((uint8)((uint8)0u << BottleCounter_STATUS_ZERO_INT_EN_MASK_SHIFT))
#else
#define BottleCounter_INIT_COMPARE_VALUE      128u
#define BottleCounter_INIT_INTERRUPTS_MASK ((uint8)((uint8)0u << BottleCounter_STATUS_ZERO_INT_EN_MASK_SHIFT) | \
        ((uint8)((uint8)0u << BottleCounter_STATUS_CAPTURE_INT_EN_MASK_SHIFT)) | \
        ((uint8)((uint8)0u << BottleCounter_STATUS_CMP_INT_EN_MASK_SHIFT)) | \
        ((uint8)((uint8)0u << BottleCounter_STATUS_OVERFLOW_INT_EN_MASK_SHIFT)) | \
        ((uint8)((uint8)0u << BottleCounter_STATUS_UNDERFLOW_INT_EN_MASK_SHIFT)))
#define BottleCounter_DEFAULT_COMPARE_MODE    1u

#if( 0 != BottleCounter_CAPTURE_MODE_CONF)
    #define BottleCounter_DEFAULT_CAPTURE_MODE    ((uint8)((uint8)0u << BottleCounter_CTRL_CAPMODE0_SHIFT))
#else    
    #define BottleCounter_DEFAULT_CAPTURE_MODE    (0u )
#endif /* ( 0 != BottleCounter_CAPTURE_MODE_CONF) */

#endif /* (BottleCounter_UsingFixedFunction) */


/**************************************
 *  Registers
 *************************************/
#if (BottleCounter_UsingFixedFunction)
    #define BottleCounter_STATICCOUNT_LSB     (*(reg16 *) BottleCounter_CounterHW__CAP0 )
    #define BottleCounter_STATICCOUNT_LSB_PTR ( (reg16 *) BottleCounter_CounterHW__CAP0 )
    #define BottleCounter_PERIOD_LSB          (*(reg16 *) BottleCounter_CounterHW__PER0 )
    #define BottleCounter_PERIOD_LSB_PTR      ( (reg16 *) BottleCounter_CounterHW__PER0 )
    /* MODE must be set to 1 to set the compare value */
    #define BottleCounter_COMPARE_LSB         (*(reg16 *) BottleCounter_CounterHW__CNT_CMP0 )
    #define BottleCounter_COMPARE_LSB_PTR     ( (reg16 *) BottleCounter_CounterHW__CNT_CMP0 )
    /* MODE must be set to 0 to get the count */
    #define BottleCounter_COUNTER_LSB         (*(reg16 *) BottleCounter_CounterHW__CNT_CMP0 )
    #define BottleCounter_COUNTER_LSB_PTR     ( (reg16 *) BottleCounter_CounterHW__CNT_CMP0 )
    #define BottleCounter_RT1                 (*(reg8 *) BottleCounter_CounterHW__RT1)
    #define BottleCounter_RT1_PTR             ( (reg8 *) BottleCounter_CounterHW__RT1)
#else
    
    #if (BottleCounter_Resolution <= 8u) /* 8-bit Counter */
    
        #define BottleCounter_STATICCOUNT_LSB     (*(reg8 *) \
            BottleCounter_CounterUDB_sC16_counterdp_u0__F0_REG )
        #define BottleCounter_STATICCOUNT_LSB_PTR ( (reg8 *) \
            BottleCounter_CounterUDB_sC16_counterdp_u0__F0_REG )
        #define BottleCounter_PERIOD_LSB          (*(reg8 *) \
            BottleCounter_CounterUDB_sC16_counterdp_u0__D0_REG )
        #define BottleCounter_PERIOD_LSB_PTR      ( (reg8 *) \
            BottleCounter_CounterUDB_sC16_counterdp_u0__D0_REG )
        #define BottleCounter_COMPARE_LSB         (*(reg8 *) \
            BottleCounter_CounterUDB_sC16_counterdp_u0__D1_REG )
        #define BottleCounter_COMPARE_LSB_PTR     ( (reg8 *) \
            BottleCounter_CounterUDB_sC16_counterdp_u0__D1_REG )
        #define BottleCounter_COUNTER_LSB         (*(reg8 *) \
            BottleCounter_CounterUDB_sC16_counterdp_u0__A0_REG )  
        #define BottleCounter_COUNTER_LSB_PTR     ( (reg8 *)\
            BottleCounter_CounterUDB_sC16_counterdp_u0__A0_REG )
    
    #elif(BottleCounter_Resolution <= 16u) /* 16-bit Counter */
        #if(CY_PSOC3) /* 8-bit address space */ 
            #define BottleCounter_STATICCOUNT_LSB     (*(reg16 *) \
                BottleCounter_CounterUDB_sC16_counterdp_u0__F0_REG )
            #define BottleCounter_STATICCOUNT_LSB_PTR ( (reg16 *) \
                BottleCounter_CounterUDB_sC16_counterdp_u0__F0_REG )
            #define BottleCounter_PERIOD_LSB          (*(reg16 *) \
                BottleCounter_CounterUDB_sC16_counterdp_u0__D0_REG )
            #define BottleCounter_PERIOD_LSB_PTR      ( (reg16 *) \
                BottleCounter_CounterUDB_sC16_counterdp_u0__D0_REG )
            #define BottleCounter_COMPARE_LSB         (*(reg16 *) \
                BottleCounter_CounterUDB_sC16_counterdp_u0__D1_REG )
            #define BottleCounter_COMPARE_LSB_PTR     ( (reg16 *) \
                BottleCounter_CounterUDB_sC16_counterdp_u0__D1_REG )
            #define BottleCounter_COUNTER_LSB         (*(reg16 *) \
                BottleCounter_CounterUDB_sC16_counterdp_u0__A0_REG )  
            #define BottleCounter_COUNTER_LSB_PTR     ( (reg16 *)\
                BottleCounter_CounterUDB_sC16_counterdp_u0__A0_REG )
        #else /* 16-bit address space */
            #define BottleCounter_STATICCOUNT_LSB     (*(reg16 *) \
                BottleCounter_CounterUDB_sC16_counterdp_u0__16BIT_F0_REG )
            #define BottleCounter_STATICCOUNT_LSB_PTR ( (reg16 *) \
                BottleCounter_CounterUDB_sC16_counterdp_u0__16BIT_F0_REG )
            #define BottleCounter_PERIOD_LSB          (*(reg16 *) \
                BottleCounter_CounterUDB_sC16_counterdp_u0__16BIT_D0_REG )
            #define BottleCounter_PERIOD_LSB_PTR      ( (reg16 *) \
                BottleCounter_CounterUDB_sC16_counterdp_u0__16BIT_D0_REG )
            #define BottleCounter_COMPARE_LSB         (*(reg16 *) \
                BottleCounter_CounterUDB_sC16_counterdp_u0__16BIT_D1_REG )
            #define BottleCounter_COMPARE_LSB_PTR     ( (reg16 *) \
                BottleCounter_CounterUDB_sC16_counterdp_u0__16BIT_D1_REG )
            #define BottleCounter_COUNTER_LSB         (*(reg16 *) \
                BottleCounter_CounterUDB_sC16_counterdp_u0__16BIT_A0_REG )  
            #define BottleCounter_COUNTER_LSB_PTR     ( (reg16 *)\
                BottleCounter_CounterUDB_sC16_counterdp_u0__16BIT_A0_REG )
        #endif /* CY_PSOC3 */   
    #elif(BottleCounter_Resolution <= 24u) /* 24-bit Counter */
        
        #define BottleCounter_STATICCOUNT_LSB     (*(reg32 *) \
            BottleCounter_CounterUDB_sC16_counterdp_u0__F0_REG )
        #define BottleCounter_STATICCOUNT_LSB_PTR ( (reg32 *) \
            BottleCounter_CounterUDB_sC16_counterdp_u0__F0_REG )
        #define BottleCounter_PERIOD_LSB          (*(reg32 *) \
            BottleCounter_CounterUDB_sC16_counterdp_u0__D0_REG )
        #define BottleCounter_PERIOD_LSB_PTR      ( (reg32 *) \
            BottleCounter_CounterUDB_sC16_counterdp_u0__D0_REG )
        #define BottleCounter_COMPARE_LSB         (*(reg32 *) \
            BottleCounter_CounterUDB_sC16_counterdp_u0__D1_REG )
        #define BottleCounter_COMPARE_LSB_PTR     ( (reg32 *) \
            BottleCounter_CounterUDB_sC16_counterdp_u0__D1_REG )
        #define BottleCounter_COUNTER_LSB         (*(reg32 *) \
            BottleCounter_CounterUDB_sC16_counterdp_u0__A0_REG )  
        #define BottleCounter_COUNTER_LSB_PTR     ( (reg32 *)\
            BottleCounter_CounterUDB_sC16_counterdp_u0__A0_REG )
    
    #else /* 32-bit Counter */
        #if(CY_PSOC3 || CY_PSOC5) /* 8-bit address space */
            #define BottleCounter_STATICCOUNT_LSB     (*(reg32 *) \
                BottleCounter_CounterUDB_sC16_counterdp_u0__F0_REG )
            #define BottleCounter_STATICCOUNT_LSB_PTR ( (reg32 *) \
                BottleCounter_CounterUDB_sC16_counterdp_u0__F0_REG )
            #define BottleCounter_PERIOD_LSB          (*(reg32 *) \
                BottleCounter_CounterUDB_sC16_counterdp_u0__D0_REG )
            #define BottleCounter_PERIOD_LSB_PTR      ( (reg32 *) \
                BottleCounter_CounterUDB_sC16_counterdp_u0__D0_REG )
            #define BottleCounter_COMPARE_LSB         (*(reg32 *) \
                BottleCounter_CounterUDB_sC16_counterdp_u0__D1_REG )
            #define BottleCounter_COMPARE_LSB_PTR     ( (reg32 *) \
                BottleCounter_CounterUDB_sC16_counterdp_u0__D1_REG )
            #define BottleCounter_COUNTER_LSB         (*(reg32 *) \
                BottleCounter_CounterUDB_sC16_counterdp_u0__A0_REG )  
            #define BottleCounter_COUNTER_LSB_PTR     ( (reg32 *)\
                BottleCounter_CounterUDB_sC16_counterdp_u0__A0_REG )
        #else /* 32-bit address space */
            #define BottleCounter_STATICCOUNT_LSB     (*(reg32 *) \
                BottleCounter_CounterUDB_sC16_counterdp_u0__32BIT_F0_REG )
            #define BottleCounter_STATICCOUNT_LSB_PTR ( (reg32 *) \
                BottleCounter_CounterUDB_sC16_counterdp_u0__32BIT_F0_REG )
            #define BottleCounter_PERIOD_LSB          (*(reg32 *) \
                BottleCounter_CounterUDB_sC16_counterdp_u0__32BIT_D0_REG )
            #define BottleCounter_PERIOD_LSB_PTR      ( (reg32 *) \
                BottleCounter_CounterUDB_sC16_counterdp_u0__32BIT_D0_REG )
            #define BottleCounter_COMPARE_LSB         (*(reg32 *) \
                BottleCounter_CounterUDB_sC16_counterdp_u0__32BIT_D1_REG )
            #define BottleCounter_COMPARE_LSB_PTR     ( (reg32 *) \
                BottleCounter_CounterUDB_sC16_counterdp_u0__32BIT_D1_REG )
            #define BottleCounter_COUNTER_LSB         (*(reg32 *) \
                BottleCounter_CounterUDB_sC16_counterdp_u0__32BIT_A0_REG )  
            #define BottleCounter_COUNTER_LSB_PTR     ( (reg32 *)\
                BottleCounter_CounterUDB_sC16_counterdp_u0__32BIT_A0_REG )
        #endif /* CY_PSOC3 || CY_PSOC5 */   
    #endif

	#define BottleCounter_COUNTER_LSB_PTR_8BIT     ( (reg8 *)\
                BottleCounter_CounterUDB_sC16_counterdp_u0__A0_REG )
				
    #define BottleCounter_AUX_CONTROLDP0 \
        (*(reg8 *) BottleCounter_CounterUDB_sC16_counterdp_u0__DP_AUX_CTL_REG)
    
    #define BottleCounter_AUX_CONTROLDP0_PTR \
        ( (reg8 *) BottleCounter_CounterUDB_sC16_counterdp_u0__DP_AUX_CTL_REG)
    
    #if (BottleCounter_Resolution == 16 || BottleCounter_Resolution == 24 || BottleCounter_Resolution == 32)
       #define BottleCounter_AUX_CONTROLDP1 \
           (*(reg8 *) BottleCounter_CounterUDB_sC16_counterdp_u1__DP_AUX_CTL_REG)
       #define BottleCounter_AUX_CONTROLDP1_PTR \
           ( (reg8 *) BottleCounter_CounterUDB_sC16_counterdp_u1__DP_AUX_CTL_REG)
    #endif /* (BottleCounter_Resolution == 16 || BottleCounter_Resolution == 24 || BottleCounter_Resolution == 32) */
    
    #if (BottleCounter_Resolution == 24 || BottleCounter_Resolution == 32)
       #define BottleCounter_AUX_CONTROLDP2 \
           (*(reg8 *) BottleCounter_CounterUDB_sC16_counterdp_u2__DP_AUX_CTL_REG)
       #define BottleCounter_AUX_CONTROLDP2_PTR \
           ( (reg8 *) BottleCounter_CounterUDB_sC16_counterdp_u2__DP_AUX_CTL_REG)
    #endif /* (BottleCounter_Resolution == 24 || BottleCounter_Resolution == 32) */
    
    #if (BottleCounter_Resolution == 32)
       #define BottleCounter_AUX_CONTROLDP3 \
           (*(reg8 *) BottleCounter_CounterUDB_sC16_counterdp_u3__DP_AUX_CTL_REG)
       #define BottleCounter_AUX_CONTROLDP3_PTR \
           ( (reg8 *) BottleCounter_CounterUDB_sC16_counterdp_u3__DP_AUX_CTL_REG)
    #endif /* (BottleCounter_Resolution == 32) */

#endif  /* (BottleCounter_UsingFixedFunction) */

#if (BottleCounter_UsingFixedFunction)
    #define BottleCounter_STATUS         (*(reg8 *) BottleCounter_CounterHW__SR0 )
    /* In Fixed Function Block Status and Mask are the same register */
    #define BottleCounter_STATUS_MASK             (*(reg8 *) BottleCounter_CounterHW__SR0 )
    #define BottleCounter_STATUS_MASK_PTR         ( (reg8 *) BottleCounter_CounterHW__SR0 )
    #define BottleCounter_CONTROL                 (*(reg8 *) BottleCounter_CounterHW__CFG0)
    #define BottleCounter_CONTROL_PTR             ( (reg8 *) BottleCounter_CounterHW__CFG0)
    #define BottleCounter_CONTROL2                (*(reg8 *) BottleCounter_CounterHW__CFG1)
    #define BottleCounter_CONTROL2_PTR            ( (reg8 *) BottleCounter_CounterHW__CFG1)
    #if (CY_PSOC3 || CY_PSOC5LP)
        #define BottleCounter_CONTROL3       (*(reg8 *) BottleCounter_CounterHW__CFG2)
        #define BottleCounter_CONTROL3_PTR   ( (reg8 *) BottleCounter_CounterHW__CFG2)
    #endif /* (CY_PSOC3 || CY_PSOC5LP) */
    #define BottleCounter_GLOBAL_ENABLE           (*(reg8 *) BottleCounter_CounterHW__PM_ACT_CFG)
    #define BottleCounter_GLOBAL_ENABLE_PTR       ( (reg8 *) BottleCounter_CounterHW__PM_ACT_CFG)
    #define BottleCounter_GLOBAL_STBY_ENABLE      (*(reg8 *) BottleCounter_CounterHW__PM_STBY_CFG)
    #define BottleCounter_GLOBAL_STBY_ENABLE_PTR  ( (reg8 *) BottleCounter_CounterHW__PM_STBY_CFG)
    

    /********************************
    *    Constants
    ********************************/

    /* Fixed Function Block Chosen */
    #define BottleCounter_BLOCK_EN_MASK          BottleCounter_CounterHW__PM_ACT_MSK
    #define BottleCounter_BLOCK_STBY_EN_MASK     BottleCounter_CounterHW__PM_STBY_MSK 
    
    /* Control Register Bit Locations */    
    /* As defined in Register Map, part of TMRX_CFG0 register */
    #define BottleCounter_CTRL_ENABLE_SHIFT      0x00u
    #define BottleCounter_ONESHOT_SHIFT          0x02u
    /* Control Register Bit Masks */
    #define BottleCounter_CTRL_ENABLE            ((uint8)((uint8)0x01u << BottleCounter_CTRL_ENABLE_SHIFT))         
    #define BottleCounter_ONESHOT                ((uint8)((uint8)0x01u << BottleCounter_ONESHOT_SHIFT))

    /* Control2 Register Bit Masks */
    /* Set the mask for run mode */
    #if (CY_PSOC5A)
        /* Use CFG1 Mode bits to set run mode */
        #define BottleCounter_CTRL_MODE_SHIFT        0x01u    
        #define BottleCounter_CTRL_MODE_MASK         ((uint8)((uint8)0x07u << BottleCounter_CTRL_MODE_SHIFT))
    #endif /* (CY_PSOC5A) */
    #if (CY_PSOC3 || CY_PSOC5LP)
        /* Use CFG2 Mode bits to set run mode */
        #define BottleCounter_CTRL_MODE_SHIFT        0x00u    
        #define BottleCounter_CTRL_MODE_MASK         ((uint8)((uint8)0x03u << BottleCounter_CTRL_MODE_SHIFT))
    #endif /* (CY_PSOC3 || CY_PSOC5LP) */
    /* Set the mask for interrupt (raw/status register) */
    #define BottleCounter_CTRL2_IRQ_SEL_SHIFT     0x00u
    #define BottleCounter_CTRL2_IRQ_SEL          ((uint8)((uint8)0x01u << BottleCounter_CTRL2_IRQ_SEL_SHIFT))     
    
    /* Status Register Bit Locations */
    #define BottleCounter_STATUS_ZERO_SHIFT      0x07u  /* As defined in Register Map, part of TMRX_SR0 register */ 

    /* Status Register Interrupt Enable Bit Locations */
    #define BottleCounter_STATUS_ZERO_INT_EN_MASK_SHIFT      (BottleCounter_STATUS_ZERO_SHIFT - 0x04u)

    /* Status Register Bit Masks */                           
    #define BottleCounter_STATUS_ZERO            ((uint8)((uint8)0x01u << BottleCounter_STATUS_ZERO_SHIFT))

    /* Status Register Interrupt Bit Masks*/
    #define BottleCounter_STATUS_ZERO_INT_EN_MASK       ((uint8)((uint8)BottleCounter_STATUS_ZERO >> 0x04u))
    
    /*RT1 Synch Constants: Applicable for PSoC3 and PSoC5LP */
    #define BottleCounter_RT1_SHIFT            0x04u
    #define BottleCounter_RT1_MASK             ((uint8)((uint8)0x03u << BottleCounter_RT1_SHIFT))  /* Sync TC and CMP bit masks */
    #define BottleCounter_SYNC                 ((uint8)((uint8)0x03u << BottleCounter_RT1_SHIFT))
    #define BottleCounter_SYNCDSI_SHIFT        0x00u
    #define BottleCounter_SYNCDSI_MASK         ((uint8)((uint8)0x0Fu << BottleCounter_SYNCDSI_SHIFT)) /* Sync all DSI inputs */
    #define BottleCounter_SYNCDSI_EN           ((uint8)((uint8)0x0Fu << BottleCounter_SYNCDSI_SHIFT)) /* Sync all DSI inputs */
    
#else /* !BottleCounter_UsingFixedFunction */
    #define BottleCounter_STATUS               (* (reg8 *) BottleCounter_CounterUDB_sSTSReg_stsreg__STATUS_REG )
    #define BottleCounter_STATUS_PTR           (  (reg8 *) BottleCounter_CounterUDB_sSTSReg_stsreg__STATUS_REG )
    #define BottleCounter_STATUS_MASK          (* (reg8 *) BottleCounter_CounterUDB_sSTSReg_stsreg__MASK_REG )
    #define BottleCounter_STATUS_MASK_PTR      (  (reg8 *) BottleCounter_CounterUDB_sSTSReg_stsreg__MASK_REG )
    #define BottleCounter_STATUS_AUX_CTRL      (*(reg8 *) BottleCounter_CounterUDB_sSTSReg_stsreg__STATUS_AUX_CTL_REG)
    #define BottleCounter_STATUS_AUX_CTRL_PTR  ( (reg8 *) BottleCounter_CounterUDB_sSTSReg_stsreg__STATUS_AUX_CTL_REG)
    #define BottleCounter_CONTROL              (* (reg8 *) BottleCounter_CounterUDB_sCTRLReg_ctrlreg__CONTROL_REG )
    #define BottleCounter_CONTROL_PTR          (  (reg8 *) BottleCounter_CounterUDB_sCTRLReg_ctrlreg__CONTROL_REG )


    /********************************
    *    Constants
    ********************************/
    /* Control Register Bit Locations */
    #define BottleCounter_CTRL_CAPMODE0_SHIFT    0x03u       /* As defined by Verilog Implementation */
    #define BottleCounter_CTRL_RESET_SHIFT       0x06u       /* As defined by Verilog Implementation */
    #define BottleCounter_CTRL_ENABLE_SHIFT      0x07u       /* As defined by Verilog Implementation */
    /* Control Register Bit Masks */
    #define BottleCounter_CTRL_CMPMODE_MASK      0x07u 
    #define BottleCounter_CTRL_CAPMODE_MASK      0x03u  
    #define BottleCounter_CTRL_RESET             ((uint8)((uint8)0x01u << BottleCounter_CTRL_RESET_SHIFT))  
    #define BottleCounter_CTRL_ENABLE            ((uint8)((uint8)0x01u << BottleCounter_CTRL_ENABLE_SHIFT)) 

    /* Status Register Bit Locations */
    #define BottleCounter_STATUS_CMP_SHIFT       0x00u       /* As defined by Verilog Implementation */
    #define BottleCounter_STATUS_ZERO_SHIFT      0x01u       /* As defined by Verilog Implementation */
    #define BottleCounter_STATUS_OVERFLOW_SHIFT  0x02u       /* As defined by Verilog Implementation */
    #define BottleCounter_STATUS_UNDERFLOW_SHIFT 0x03u       /* As defined by Verilog Implementation */
    #define BottleCounter_STATUS_CAPTURE_SHIFT   0x04u       /* As defined by Verilog Implementation */
    #define BottleCounter_STATUS_FIFOFULL_SHIFT  0x05u       /* As defined by Verilog Implementation */
    #define BottleCounter_STATUS_FIFONEMP_SHIFT  0x06u       /* As defined by Verilog Implementation */
    /* Status Register Interrupt Enable Bit Locations - UDB Status Interrupt Mask match Status Bit Locations*/
    #define BottleCounter_STATUS_CMP_INT_EN_MASK_SHIFT       BottleCounter_STATUS_CMP_SHIFT       
    #define BottleCounter_STATUS_ZERO_INT_EN_MASK_SHIFT      BottleCounter_STATUS_ZERO_SHIFT      
    #define BottleCounter_STATUS_OVERFLOW_INT_EN_MASK_SHIFT  BottleCounter_STATUS_OVERFLOW_SHIFT  
    #define BottleCounter_STATUS_UNDERFLOW_INT_EN_MASK_SHIFT BottleCounter_STATUS_UNDERFLOW_SHIFT 
    #define BottleCounter_STATUS_CAPTURE_INT_EN_MASK_SHIFT   BottleCounter_STATUS_CAPTURE_SHIFT   
    #define BottleCounter_STATUS_FIFOFULL_INT_EN_MASK_SHIFT  BottleCounter_STATUS_FIFOFULL_SHIFT  
    #define BottleCounter_STATUS_FIFONEMP_INT_EN_MASK_SHIFT  BottleCounter_STATUS_FIFONEMP_SHIFT  
    /* Status Register Bit Masks */                
    #define BottleCounter_STATUS_CMP             ((uint8)((uint8)0x01u << BottleCounter_STATUS_CMP_SHIFT))  
    #define BottleCounter_STATUS_ZERO            ((uint8)((uint8)0x01u << BottleCounter_STATUS_ZERO_SHIFT)) 
    #define BottleCounter_STATUS_OVERFLOW        ((uint8)((uint8)0x01u << BottleCounter_STATUS_OVERFLOW_SHIFT)) 
    #define BottleCounter_STATUS_UNDERFLOW       ((uint8)((uint8)0x01u << BottleCounter_STATUS_UNDERFLOW_SHIFT)) 
    #define BottleCounter_STATUS_CAPTURE         ((uint8)((uint8)0x01u << BottleCounter_STATUS_CAPTURE_SHIFT)) 
    #define BottleCounter_STATUS_FIFOFULL        ((uint8)((uint8)0x01u << BottleCounter_STATUS_FIFOFULL_SHIFT))
    #define BottleCounter_STATUS_FIFONEMP        ((uint8)((uint8)0x01u << BottleCounter_STATUS_FIFONEMP_SHIFT))
    /* Status Register Interrupt Bit Masks  - UDB Status Interrupt Mask match Status Bit Locations */
    #define BottleCounter_STATUS_CMP_INT_EN_MASK            BottleCounter_STATUS_CMP                    
    #define BottleCounter_STATUS_ZERO_INT_EN_MASK           BottleCounter_STATUS_ZERO            
    #define BottleCounter_STATUS_OVERFLOW_INT_EN_MASK       BottleCounter_STATUS_OVERFLOW        
    #define BottleCounter_STATUS_UNDERFLOW_INT_EN_MASK      BottleCounter_STATUS_UNDERFLOW       
    #define BottleCounter_STATUS_CAPTURE_INT_EN_MASK        BottleCounter_STATUS_CAPTURE         
    #define BottleCounter_STATUS_FIFOFULL_INT_EN_MASK       BottleCounter_STATUS_FIFOFULL        
    #define BottleCounter_STATUS_FIFONEMP_INT_EN_MASK       BottleCounter_STATUS_FIFONEMP         
    

    /* StatusI Interrupt Enable bit Location in the Auxilliary Control Register */
    #define BottleCounter_STATUS_ACTL_INT_EN     0x10u /* As defined for the ACTL Register */
    
    /* Datapath Auxillary Control Register definitions */
    #define BottleCounter_AUX_CTRL_FIFO0_CLR         0x01u   /* As defined by Register map */
    #define BottleCounter_AUX_CTRL_FIFO1_CLR         0x02u   /* As defined by Register map */
    #define BottleCounter_AUX_CTRL_FIFO0_LVL         0x04u   /* As defined by Register map */
    #define BottleCounter_AUX_CTRL_FIFO1_LVL         0x08u   /* As defined by Register map */
    #define BottleCounter_STATUS_ACTL_INT_EN_MASK    0x10u   /* As defined for the ACTL Register */
    
#endif /* BottleCounter_UsingFixedFunction */

#endif  /* CY_COUNTER_BottleCounter_H */


/* [] END OF FILE */

