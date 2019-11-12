/*******************************************************************************
* File Name: Debug.h
* Version 2.50
*
* Description:
*  Contains the function prototypes and constants available to the UART
*  user module.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_UART_Debug_H)
#define CY_UART_Debug_H

#include "cyfitter.h"
#include "cytypes.h"
#include "CyLib.h" /* For CyEnterCriticalSection() and CyExitCriticalSection() functions */


/***************************************
* Conditional Compilation Parameters
***************************************/

#define Debug_RX_ENABLED                     (1u)
#define Debug_TX_ENABLED                     (1u)
#define Debug_HD_ENABLED                     (0u)
#define Debug_RX_INTERRUPT_ENABLED           (0u)
#define Debug_TX_INTERRUPT_ENABLED           (0u)
#define Debug_INTERNAL_CLOCK_USED            (1u)
#define Debug_RXHW_ADDRESS_ENABLED           (0u)
#define Debug_OVER_SAMPLE_COUNT              (8u)
#define Debug_PARITY_TYPE                    (0u)
#define Debug_PARITY_TYPE_SW                 (0u)
#define Debug_BREAK_DETECT                   (0u)
#define Debug_BREAK_BITS_TX                  (13u)
#define Debug_BREAK_BITS_RX                  (13u)
#define Debug_TXCLKGEN_DP                    (1u)
#define Debug_USE23POLLING                   (1u)
#define Debug_FLOW_CONTROL                   (0u)
#define Debug_CLK_FREQ                       (0u)
#define Debug_TX_BUFFER_SIZE                 (4u)
#define Debug_RX_BUFFER_SIZE                 (4u)

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component UART_v2_50 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */

#if defined(Debug_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG)
    #define Debug_CONTROL_REG_REMOVED            (0u)
#else
    #define Debug_CONTROL_REG_REMOVED            (1u)
#endif /* End Debug_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */


/***************************************
*      Data Structure Definition
***************************************/

/* Sleep Mode API Support */
typedef struct Debug_backupStruct_
{
    uint8 enableState;

    #if(Debug_CONTROL_REG_REMOVED == 0u)
        uint8 cr;
    #endif /* End Debug_CONTROL_REG_REMOVED */

} Debug_BACKUP_STRUCT;


/***************************************
*       Function Prototypes
***************************************/

void Debug_Start(void) ;
void Debug_Stop(void) ;
uint8 Debug_ReadControlRegister(void) ;
void Debug_WriteControlRegister(uint8 control) ;

void Debug_Init(void) ;
void Debug_Enable(void) ;
void Debug_SaveConfig(void) ;
void Debug_RestoreConfig(void) ;
void Debug_Sleep(void) ;
void Debug_Wakeup(void) ;

/* Only if RX is enabled */
#if( (Debug_RX_ENABLED) || (Debug_HD_ENABLED) )

    #if (Debug_RX_INTERRUPT_ENABLED)
        #define Debug_EnableRxInt()  CyIntEnable (Debug_RX_VECT_NUM)
        #define Debug_DisableRxInt() CyIntDisable(Debug_RX_VECT_NUM)
        CY_ISR_PROTO(Debug_RXISR);
    #endif /* Debug_RX_INTERRUPT_ENABLED */

    void Debug_SetRxAddressMode(uint8 addressMode)
                                                           ;
    void Debug_SetRxAddress1(uint8 address) ;
    void Debug_SetRxAddress2(uint8 address) ;

    void  Debug_SetRxInterruptMode(uint8 intSrc) ;
    uint8 Debug_ReadRxData(void) ;
    uint8 Debug_ReadRxStatus(void) ;
    uint8 Debug_GetChar(void) ;
    uint16 Debug_GetByte(void) ;
    uint8 Debug_GetRxBufferSize(void)
                                                            ;
    void Debug_ClearRxBuffer(void) ;

    /* Obsolete functions, defines for backward compatible */
    #define Debug_GetRxInterruptSource   Debug_ReadRxStatus

#endif /* End (Debug_RX_ENABLED) || (Debug_HD_ENABLED) */

/* Only if TX is enabled */
#if(Debug_TX_ENABLED || Debug_HD_ENABLED)

    #if(Debug_TX_INTERRUPT_ENABLED)
        #define Debug_EnableTxInt()  CyIntEnable (Debug_TX_VECT_NUM)
        #define Debug_DisableTxInt() CyIntDisable(Debug_TX_VECT_NUM)
        #define Debug_SetPendingTxInt() CyIntSetPending(Debug_TX_VECT_NUM)
        #define Debug_ClearPendingTxInt() CyIntClearPending(Debug_TX_VECT_NUM)
        CY_ISR_PROTO(Debug_TXISR);
    #endif /* Debug_TX_INTERRUPT_ENABLED */

    void Debug_SetTxInterruptMode(uint8 intSrc) ;
    void Debug_WriteTxData(uint8 txDataByte) ;
    uint8 Debug_ReadTxStatus(void) ;
    void Debug_PutChar(uint8 txDataByte) ;
    void Debug_PutString(const char8 string[]) ;
    void Debug_PutArray(const uint8 string[], uint8 byteCount)
                                                            ;
    void Debug_PutCRLF(uint8 txDataByte) ;
    void Debug_ClearTxBuffer(void) ;
    void Debug_SetTxAddressMode(uint8 addressMode) ;
    void Debug_SendBreak(uint8 retMode) ;
    uint8 Debug_GetTxBufferSize(void)
                                                            ;
    /* Obsolete functions, defines for backward compatible */
    #define Debug_PutStringConst         Debug_PutString
    #define Debug_PutArrayConst          Debug_PutArray
    #define Debug_GetTxInterruptSource   Debug_ReadTxStatus

#endif /* End Debug_TX_ENABLED || Debug_HD_ENABLED */

#if(Debug_HD_ENABLED)
    void Debug_LoadRxConfig(void) ;
    void Debug_LoadTxConfig(void) ;
#endif /* End Debug_HD_ENABLED */


/* Communication bootloader APIs */
#if defined(CYDEV_BOOTLOADER_IO_COMP) && ((CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Debug) || \
                                          (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Custom_Interface))
    /* Physical layer functions */
    void    Debug_CyBtldrCommStart(void) CYSMALL ;
    void    Debug_CyBtldrCommStop(void) CYSMALL ;
    void    Debug_CyBtldrCommReset(void) CYSMALL ;
    cystatus Debug_CyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;
    cystatus Debug_CyBtldrCommRead(uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;

    #if (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Debug)
        #define CyBtldrCommStart    Debug_CyBtldrCommStart
        #define CyBtldrCommStop     Debug_CyBtldrCommStop
        #define CyBtldrCommReset    Debug_CyBtldrCommReset
        #define CyBtldrCommWrite    Debug_CyBtldrCommWrite
        #define CyBtldrCommRead     Debug_CyBtldrCommRead
    #endif  /* (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Debug) */

    /* Byte to Byte time out for detecting end of block data from host */
    #define Debug_BYTE2BYTE_TIME_OUT (25u)
    #define Debug_PACKET_EOP         (0x17u) /* End of packet defined by bootloader */
    #define Debug_WAIT_EOP_DELAY     (5u)    /* Additional 5ms to wait for End of packet */
    #define Debug_BL_CHK_DELAY_MS    (1u)    /* Time Out quantity equal 1mS */

#endif /* CYDEV_BOOTLOADER_IO_COMP */


/***************************************
*          API Constants
***************************************/
/* Parameters for SetTxAddressMode API*/
#define Debug_SET_SPACE      (0x00u)
#define Debug_SET_MARK       (0x01u)

/* Status Register definitions */
#if( (Debug_TX_ENABLED) || (Debug_HD_ENABLED) )
    #if(Debug_TX_INTERRUPT_ENABLED)
        #define Debug_TX_VECT_NUM            (uint8)Debug_TXInternalInterrupt__INTC_NUMBER
        #define Debug_TX_PRIOR_NUM           (uint8)Debug_TXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* Debug_TX_INTERRUPT_ENABLED */

    #define Debug_TX_STS_COMPLETE_SHIFT          (0x00u)
    #define Debug_TX_STS_FIFO_EMPTY_SHIFT        (0x01u)
    #define Debug_TX_STS_FIFO_NOT_FULL_SHIFT     (0x03u)
    #if(Debug_TX_ENABLED)
        #define Debug_TX_STS_FIFO_FULL_SHIFT     (0x02u)
    #else /* (Debug_HD_ENABLED) */
        #define Debug_TX_STS_FIFO_FULL_SHIFT     (0x05u)  /* Needs MD=0 */
    #endif /* (Debug_TX_ENABLED) */

    #define Debug_TX_STS_COMPLETE            (uint8)(0x01u << Debug_TX_STS_COMPLETE_SHIFT)
    #define Debug_TX_STS_FIFO_EMPTY          (uint8)(0x01u << Debug_TX_STS_FIFO_EMPTY_SHIFT)
    #define Debug_TX_STS_FIFO_FULL           (uint8)(0x01u << Debug_TX_STS_FIFO_FULL_SHIFT)
    #define Debug_TX_STS_FIFO_NOT_FULL       (uint8)(0x01u << Debug_TX_STS_FIFO_NOT_FULL_SHIFT)
#endif /* End (Debug_TX_ENABLED) || (Debug_HD_ENABLED)*/

#if( (Debug_RX_ENABLED) || (Debug_HD_ENABLED) )
    #if(Debug_RX_INTERRUPT_ENABLED)
        #define Debug_RX_VECT_NUM            (uint8)Debug_RXInternalInterrupt__INTC_NUMBER
        #define Debug_RX_PRIOR_NUM           (uint8)Debug_RXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* Debug_RX_INTERRUPT_ENABLED */
    #define Debug_RX_STS_MRKSPC_SHIFT            (0x00u)
    #define Debug_RX_STS_BREAK_SHIFT             (0x01u)
    #define Debug_RX_STS_PAR_ERROR_SHIFT         (0x02u)
    #define Debug_RX_STS_STOP_ERROR_SHIFT        (0x03u)
    #define Debug_RX_STS_OVERRUN_SHIFT           (0x04u)
    #define Debug_RX_STS_FIFO_NOTEMPTY_SHIFT     (0x05u)
    #define Debug_RX_STS_ADDR_MATCH_SHIFT        (0x06u)
    #define Debug_RX_STS_SOFT_BUFF_OVER_SHIFT    (0x07u)

    #define Debug_RX_STS_MRKSPC           (uint8)(0x01u << Debug_RX_STS_MRKSPC_SHIFT)
    #define Debug_RX_STS_BREAK            (uint8)(0x01u << Debug_RX_STS_BREAK_SHIFT)
    #define Debug_RX_STS_PAR_ERROR        (uint8)(0x01u << Debug_RX_STS_PAR_ERROR_SHIFT)
    #define Debug_RX_STS_STOP_ERROR       (uint8)(0x01u << Debug_RX_STS_STOP_ERROR_SHIFT)
    #define Debug_RX_STS_OVERRUN          (uint8)(0x01u << Debug_RX_STS_OVERRUN_SHIFT)
    #define Debug_RX_STS_FIFO_NOTEMPTY    (uint8)(0x01u << Debug_RX_STS_FIFO_NOTEMPTY_SHIFT)
    #define Debug_RX_STS_ADDR_MATCH       (uint8)(0x01u << Debug_RX_STS_ADDR_MATCH_SHIFT)
    #define Debug_RX_STS_SOFT_BUFF_OVER   (uint8)(0x01u << Debug_RX_STS_SOFT_BUFF_OVER_SHIFT)
    #define Debug_RX_HW_MASK                     (0x7Fu)
#endif /* End (Debug_RX_ENABLED) || (Debug_HD_ENABLED) */

/* Control Register definitions */
#define Debug_CTRL_HD_SEND_SHIFT                 (0x00u) /* 1 enable TX part in Half Duplex mode */
#define Debug_CTRL_HD_SEND_BREAK_SHIFT           (0x01u) /* 1 send BREAK signal in Half Duplez mode */
#define Debug_CTRL_MARK_SHIFT                    (0x02u) /* 1 sets mark, 0 sets space */
#define Debug_CTRL_PARITY_TYPE0_SHIFT            (0x03u) /* Defines the type of parity implemented */
#define Debug_CTRL_PARITY_TYPE1_SHIFT            (0x04u) /* Defines the type of parity implemented */
#define Debug_CTRL_RXADDR_MODE0_SHIFT            (0x05u)
#define Debug_CTRL_RXADDR_MODE1_SHIFT            (0x06u)
#define Debug_CTRL_RXADDR_MODE2_SHIFT            (0x07u)

#define Debug_CTRL_HD_SEND               (uint8)(0x01u << Debug_CTRL_HD_SEND_SHIFT)
#define Debug_CTRL_HD_SEND_BREAK         (uint8)(0x01u << Debug_CTRL_HD_SEND_BREAK_SHIFT)
#define Debug_CTRL_MARK                  (uint8)(0x01u << Debug_CTRL_MARK_SHIFT)
#define Debug_CTRL_PARITY_TYPE_MASK      (uint8)(0x03u << Debug_CTRL_PARITY_TYPE0_SHIFT)
#define Debug_CTRL_RXADDR_MODE_MASK      (uint8)(0x07u << Debug_CTRL_RXADDR_MODE0_SHIFT)

/* StatusI Register Interrupt Enable Control Bits. As defined by the Register map for the AUX Control Register */
#define Debug_INT_ENABLE                         (0x10u)

/* Bit Counter (7-bit) Control Register Bit Definitions. As defined by the Register map for the AUX Control Register */
#define Debug_CNTR_ENABLE                        (0x20u)

/*   Constants for SendBreak() "retMode" parameter  */
#define Debug_SEND_BREAK                         (0x00u)
#define Debug_WAIT_FOR_COMPLETE_REINIT           (0x01u)
#define Debug_REINIT                             (0x02u)
#define Debug_SEND_WAIT_REINIT                   (0x03u)

#define Debug_OVER_SAMPLE_8                      (8u)
#define Debug_OVER_SAMPLE_16                     (16u)

#define Debug_BIT_CENTER                         (Debug_OVER_SAMPLE_COUNT - 2u)

#define Debug_FIFO_LENGTH                        (4u)
#define Debug_NUMBER_OF_START_BIT                (1u)
#define Debug_MAX_BYTE_VALUE                     (0xFFu)

/* 8X always for count7 implementation */
#define Debug_TXBITCTR_BREAKBITS8X   ((Debug_BREAK_BITS_TX * Debug_OVER_SAMPLE_8) - 1u)
/* 8X or 16X for DP implementation */
#define Debug_TXBITCTR_BREAKBITS ((Debug_BREAK_BITS_TX * Debug_OVER_SAMPLE_COUNT) - 1u)

#define Debug_HALF_BIT_COUNT   \
                            (((Debug_OVER_SAMPLE_COUNT / 2u) + (Debug_USE23POLLING * 1u)) - 2u)
#if (Debug_OVER_SAMPLE_COUNT == Debug_OVER_SAMPLE_8)
    #define Debug_HD_TXBITCTR_INIT   (((Debug_BREAK_BITS_TX + \
                            Debug_NUMBER_OF_START_BIT) * Debug_OVER_SAMPLE_COUNT) - 1u)

    /* This parameter is increased on the 2 in 2 out of 3 mode to sample voting in the middle */
    #define Debug_RXBITCTR_INIT  ((((Debug_BREAK_BITS_RX + Debug_NUMBER_OF_START_BIT) \
                            * Debug_OVER_SAMPLE_COUNT) + Debug_HALF_BIT_COUNT) - 1u)

#else /* Debug_OVER_SAMPLE_COUNT == Debug_OVER_SAMPLE_16 */
    #define Debug_HD_TXBITCTR_INIT   ((8u * Debug_OVER_SAMPLE_COUNT) - 1u)
    /* 7bit counter need one more bit for OverSampleCount = 16 */
    #define Debug_RXBITCTR_INIT      (((7u * Debug_OVER_SAMPLE_COUNT) - 1u) + \
                                                      Debug_HALF_BIT_COUNT)
#endif /* End Debug_OVER_SAMPLE_COUNT */

#define Debug_HD_RXBITCTR_INIT                   Debug_RXBITCTR_INIT


/***************************************
* Global variables external identifier
***************************************/

extern uint8 Debug_initVar;
#if (Debug_TX_INTERRUPT_ENABLED && Debug_TX_ENABLED)
    extern volatile uint8 Debug_txBuffer[Debug_TX_BUFFER_SIZE];
    extern volatile uint8 Debug_txBufferRead;
    extern uint8 Debug_txBufferWrite;
#endif /* (Debug_TX_INTERRUPT_ENABLED && Debug_TX_ENABLED) */
#if (Debug_RX_INTERRUPT_ENABLED && (Debug_RX_ENABLED || Debug_HD_ENABLED))
    extern uint8 Debug_errorStatus;
    extern volatile uint8 Debug_rxBuffer[Debug_RX_BUFFER_SIZE];
    extern volatile uint8 Debug_rxBufferRead;
    extern volatile uint8 Debug_rxBufferWrite;
    extern volatile uint8 Debug_rxBufferLoopDetect;
    extern volatile uint8 Debug_rxBufferOverflow;
    #if (Debug_RXHW_ADDRESS_ENABLED)
        extern volatile uint8 Debug_rxAddressMode;
        extern volatile uint8 Debug_rxAddressDetected;
    #endif /* (Debug_RXHW_ADDRESS_ENABLED) */
#endif /* (Debug_RX_INTERRUPT_ENABLED && (Debug_RX_ENABLED || Debug_HD_ENABLED)) */


/***************************************
* Enumerated Types and Parameters
***************************************/

#define Debug__B_UART__AM_SW_BYTE_BYTE 1
#define Debug__B_UART__AM_SW_DETECT_TO_BUFFER 2
#define Debug__B_UART__AM_HW_BYTE_BY_BYTE 3
#define Debug__B_UART__AM_HW_DETECT_TO_BUFFER 4
#define Debug__B_UART__AM_NONE 0

#define Debug__B_UART__NONE_REVB 0
#define Debug__B_UART__EVEN_REVB 1
#define Debug__B_UART__ODD_REVB 2
#define Debug__B_UART__MARK_SPACE_REVB 3



/***************************************
*    Initial Parameter Constants
***************************************/

/* UART shifts max 8 bits, Mark/Space functionality working if 9 selected */
#define Debug_NUMBER_OF_DATA_BITS    ((8u > 8u) ? 8u : 8u)
#define Debug_NUMBER_OF_STOP_BITS    (1u)

#if (Debug_RXHW_ADDRESS_ENABLED)
    #define Debug_RX_ADDRESS_MODE    (0u)
    #define Debug_RX_HW_ADDRESS1     (0u)
    #define Debug_RX_HW_ADDRESS2     (0u)
#endif /* (Debug_RXHW_ADDRESS_ENABLED) */

#define Debug_INIT_RX_INTERRUPTS_MASK \
                                  (uint8)((1 << Debug_RX_STS_FIFO_NOTEMPTY_SHIFT) \
                                        | (0 << Debug_RX_STS_MRKSPC_SHIFT) \
                                        | (0 << Debug_RX_STS_ADDR_MATCH_SHIFT) \
                                        | (0 << Debug_RX_STS_PAR_ERROR_SHIFT) \
                                        | (0 << Debug_RX_STS_STOP_ERROR_SHIFT) \
                                        | (0 << Debug_RX_STS_BREAK_SHIFT) \
                                        | (0 << Debug_RX_STS_OVERRUN_SHIFT))

#define Debug_INIT_TX_INTERRUPTS_MASK \
                                  (uint8)((0 << Debug_TX_STS_COMPLETE_SHIFT) \
                                        | (0 << Debug_TX_STS_FIFO_EMPTY_SHIFT) \
                                        | (0 << Debug_TX_STS_FIFO_FULL_SHIFT) \
                                        | (0 << Debug_TX_STS_FIFO_NOT_FULL_SHIFT))


/***************************************
*              Registers
***************************************/

#ifdef Debug_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define Debug_CONTROL_REG \
                            (* (reg8 *) Debug_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
    #define Debug_CONTROL_PTR \
                            (  (reg8 *) Debug_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
#endif /* End Debug_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(Debug_TX_ENABLED)
    #define Debug_TXDATA_REG          (* (reg8 *) Debug_BUART_sTX_TxShifter_u0__F0_REG)
    #define Debug_TXDATA_PTR          (  (reg8 *) Debug_BUART_sTX_TxShifter_u0__F0_REG)
    #define Debug_TXDATA_AUX_CTL_REG  (* (reg8 *) Debug_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define Debug_TXDATA_AUX_CTL_PTR  (  (reg8 *) Debug_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define Debug_TXSTATUS_REG        (* (reg8 *) Debug_BUART_sTX_TxSts__STATUS_REG)
    #define Debug_TXSTATUS_PTR        (  (reg8 *) Debug_BUART_sTX_TxSts__STATUS_REG)
    #define Debug_TXSTATUS_MASK_REG   (* (reg8 *) Debug_BUART_sTX_TxSts__MASK_REG)
    #define Debug_TXSTATUS_MASK_PTR   (  (reg8 *) Debug_BUART_sTX_TxSts__MASK_REG)
    #define Debug_TXSTATUS_ACTL_REG   (* (reg8 *) Debug_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)
    #define Debug_TXSTATUS_ACTL_PTR   (  (reg8 *) Debug_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)

    /* DP clock */
    #if(Debug_TXCLKGEN_DP)
        #define Debug_TXBITCLKGEN_CTR_REG        \
                                        (* (reg8 *) Debug_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define Debug_TXBITCLKGEN_CTR_PTR        \
                                        (  (reg8 *) Debug_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define Debug_TXBITCLKTX_COMPLETE_REG    \
                                        (* (reg8 *) Debug_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
        #define Debug_TXBITCLKTX_COMPLETE_PTR    \
                                        (  (reg8 *) Debug_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
    #else     /* Count7 clock*/
        #define Debug_TXBITCTR_PERIOD_REG    \
                                        (* (reg8 *) Debug_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define Debug_TXBITCTR_PERIOD_PTR    \
                                        (  (reg8 *) Debug_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define Debug_TXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) Debug_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define Debug_TXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) Debug_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define Debug_TXBITCTR_COUNTER_REG   \
                                        (* (reg8 *) Debug_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
        #define Debug_TXBITCTR_COUNTER_PTR   \
                                        (  (reg8 *) Debug_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
    #endif /* Debug_TXCLKGEN_DP */

#endif /* End Debug_TX_ENABLED */

#if(Debug_HD_ENABLED)

    #define Debug_TXDATA_REG             (* (reg8 *) Debug_BUART_sRX_RxShifter_u0__F1_REG )
    #define Debug_TXDATA_PTR             (  (reg8 *) Debug_BUART_sRX_RxShifter_u0__F1_REG )
    #define Debug_TXDATA_AUX_CTL_REG     (* (reg8 *) Debug_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)
    #define Debug_TXDATA_AUX_CTL_PTR     (  (reg8 *) Debug_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define Debug_TXSTATUS_REG           (* (reg8 *) Debug_BUART_sRX_RxSts__STATUS_REG )
    #define Debug_TXSTATUS_PTR           (  (reg8 *) Debug_BUART_sRX_RxSts__STATUS_REG )
    #define Debug_TXSTATUS_MASK_REG      (* (reg8 *) Debug_BUART_sRX_RxSts__MASK_REG )
    #define Debug_TXSTATUS_MASK_PTR      (  (reg8 *) Debug_BUART_sRX_RxSts__MASK_REG )
    #define Debug_TXSTATUS_ACTL_REG      (* (reg8 *) Debug_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define Debug_TXSTATUS_ACTL_PTR      (  (reg8 *) Debug_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End Debug_HD_ENABLED */

#if( (Debug_RX_ENABLED) || (Debug_HD_ENABLED) )
    #define Debug_RXDATA_REG             (* (reg8 *) Debug_BUART_sRX_RxShifter_u0__F0_REG )
    #define Debug_RXDATA_PTR             (  (reg8 *) Debug_BUART_sRX_RxShifter_u0__F0_REG )
    #define Debug_RXADDRESS1_REG         (* (reg8 *) Debug_BUART_sRX_RxShifter_u0__D0_REG )
    #define Debug_RXADDRESS1_PTR         (  (reg8 *) Debug_BUART_sRX_RxShifter_u0__D0_REG )
    #define Debug_RXADDRESS2_REG         (* (reg8 *) Debug_BUART_sRX_RxShifter_u0__D1_REG )
    #define Debug_RXADDRESS2_PTR         (  (reg8 *) Debug_BUART_sRX_RxShifter_u0__D1_REG )
    #define Debug_RXDATA_AUX_CTL_REG     (* (reg8 *) Debug_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define Debug_RXBITCTR_PERIOD_REG    (* (reg8 *) Debug_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define Debug_RXBITCTR_PERIOD_PTR    (  (reg8 *) Debug_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define Debug_RXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) Debug_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define Debug_RXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) Debug_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define Debug_RXBITCTR_COUNTER_REG   (* (reg8 *) Debug_BUART_sRX_RxBitCounter__COUNT_REG )
    #define Debug_RXBITCTR_COUNTER_PTR   (  (reg8 *) Debug_BUART_sRX_RxBitCounter__COUNT_REG )

    #define Debug_RXSTATUS_REG           (* (reg8 *) Debug_BUART_sRX_RxSts__STATUS_REG )
    #define Debug_RXSTATUS_PTR           (  (reg8 *) Debug_BUART_sRX_RxSts__STATUS_REG )
    #define Debug_RXSTATUS_MASK_REG      (* (reg8 *) Debug_BUART_sRX_RxSts__MASK_REG )
    #define Debug_RXSTATUS_MASK_PTR      (  (reg8 *) Debug_BUART_sRX_RxSts__MASK_REG )
    #define Debug_RXSTATUS_ACTL_REG      (* (reg8 *) Debug_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define Debug_RXSTATUS_ACTL_PTR      (  (reg8 *) Debug_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End  (Debug_RX_ENABLED) || (Debug_HD_ENABLED) */

#if(Debug_INTERNAL_CLOCK_USED)
    /* Register to enable or disable the digital clocks */
    #define Debug_INTCLOCK_CLKEN_REG     (* (reg8 *) Debug_IntClock__PM_ACT_CFG)
    #define Debug_INTCLOCK_CLKEN_PTR     (  (reg8 *) Debug_IntClock__PM_ACT_CFG)

    /* Clock mask for this clock. */
    #define Debug_INTCLOCK_CLKEN_MASK    Debug_IntClock__PM_ACT_MSK
#endif /* End Debug_INTERNAL_CLOCK_USED */


/***************************************
*       Register Constants
***************************************/

#if(Debug_TX_ENABLED)
    #define Debug_TX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End Debug_TX_ENABLED */

#if(Debug_HD_ENABLED)
    #define Debug_TX_FIFO_CLR            (0x02u) /* FIFO1 CLR */
#endif /* End Debug_HD_ENABLED */

#if( (Debug_RX_ENABLED) || (Debug_HD_ENABLED) )
    #define Debug_RX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End  (Debug_RX_ENABLED) || (Debug_HD_ENABLED) */


/***************************************
* The following code is DEPRECATED and
* should not be used in new projects.
***************************************/

/* UART v2_40 obsolete definitions */
#define Debug_WAIT_1_MS      Debug_BL_CHK_DELAY_MS   

#define Debug_TXBUFFERSIZE   Debug_TX_BUFFER_SIZE
#define Debug_RXBUFFERSIZE   Debug_RX_BUFFER_SIZE

#if (Debug_RXHW_ADDRESS_ENABLED)
    #define Debug_RXADDRESSMODE  Debug_RX_ADDRESS_MODE
    #define Debug_RXHWADDRESS1   Debug_RX_HW_ADDRESS1
    #define Debug_RXHWADDRESS2   Debug_RX_HW_ADDRESS2
    /* Backward compatible define */
    #define Debug_RXAddressMode  Debug_RXADDRESSMODE
#endif /* (Debug_RXHW_ADDRESS_ENABLED) */

/* UART v2_30 obsolete definitions */
#define Debug_initvar                    Debug_initVar

#define Debug_RX_Enabled                 Debug_RX_ENABLED
#define Debug_TX_Enabled                 Debug_TX_ENABLED
#define Debug_HD_Enabled                 Debug_HD_ENABLED
#define Debug_RX_IntInterruptEnabled     Debug_RX_INTERRUPT_ENABLED
#define Debug_TX_IntInterruptEnabled     Debug_TX_INTERRUPT_ENABLED
#define Debug_InternalClockUsed          Debug_INTERNAL_CLOCK_USED
#define Debug_RXHW_Address_Enabled       Debug_RXHW_ADDRESS_ENABLED
#define Debug_OverSampleCount            Debug_OVER_SAMPLE_COUNT
#define Debug_ParityType                 Debug_PARITY_TYPE

#if( Debug_TX_ENABLED && (Debug_TXBUFFERSIZE > Debug_FIFO_LENGTH))
    #define Debug_TXBUFFER               Debug_txBuffer
    #define Debug_TXBUFFERREAD           Debug_txBufferRead
    #define Debug_TXBUFFERWRITE          Debug_txBufferWrite
#endif /* End Debug_TX_ENABLED */
#if( ( Debug_RX_ENABLED || Debug_HD_ENABLED ) && \
     (Debug_RXBUFFERSIZE > Debug_FIFO_LENGTH) )
    #define Debug_RXBUFFER               Debug_rxBuffer
    #define Debug_RXBUFFERREAD           Debug_rxBufferRead
    #define Debug_RXBUFFERWRITE          Debug_rxBufferWrite
    #define Debug_RXBUFFERLOOPDETECT     Debug_rxBufferLoopDetect
    #define Debug_RXBUFFER_OVERFLOW      Debug_rxBufferOverflow
#endif /* End Debug_RX_ENABLED */

#ifdef Debug_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define Debug_CONTROL                Debug_CONTROL_REG
#endif /* End Debug_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(Debug_TX_ENABLED)
    #define Debug_TXDATA                 Debug_TXDATA_REG
    #define Debug_TXSTATUS               Debug_TXSTATUS_REG
    #define Debug_TXSTATUS_MASK          Debug_TXSTATUS_MASK_REG
    #define Debug_TXSTATUS_ACTL          Debug_TXSTATUS_ACTL_REG
    /* DP clock */
    #if(Debug_TXCLKGEN_DP)
        #define Debug_TXBITCLKGEN_CTR        Debug_TXBITCLKGEN_CTR_REG
        #define Debug_TXBITCLKTX_COMPLETE    Debug_TXBITCLKTX_COMPLETE_REG
    #else     /* Count7 clock*/
        #define Debug_TXBITCTR_PERIOD        Debug_TXBITCTR_PERIOD_REG
        #define Debug_TXBITCTR_CONTROL       Debug_TXBITCTR_CONTROL_REG
        #define Debug_TXBITCTR_COUNTER       Debug_TXBITCTR_COUNTER_REG
    #endif /* Debug_TXCLKGEN_DP */
#endif /* End Debug_TX_ENABLED */

#if(Debug_HD_ENABLED)
    #define Debug_TXDATA                 Debug_TXDATA_REG
    #define Debug_TXSTATUS               Debug_TXSTATUS_REG
    #define Debug_TXSTATUS_MASK          Debug_TXSTATUS_MASK_REG
    #define Debug_TXSTATUS_ACTL          Debug_TXSTATUS_ACTL_REG
#endif /* End Debug_HD_ENABLED */

#if( (Debug_RX_ENABLED) || (Debug_HD_ENABLED) )
    #define Debug_RXDATA                 Debug_RXDATA_REG
    #define Debug_RXADDRESS1             Debug_RXADDRESS1_REG
    #define Debug_RXADDRESS2             Debug_RXADDRESS2_REG
    #define Debug_RXBITCTR_PERIOD        Debug_RXBITCTR_PERIOD_REG
    #define Debug_RXBITCTR_CONTROL       Debug_RXBITCTR_CONTROL_REG
    #define Debug_RXBITCTR_COUNTER       Debug_RXBITCTR_COUNTER_REG
    #define Debug_RXSTATUS               Debug_RXSTATUS_REG
    #define Debug_RXSTATUS_MASK          Debug_RXSTATUS_MASK_REG
    #define Debug_RXSTATUS_ACTL          Debug_RXSTATUS_ACTL_REG
#endif /* End  (Debug_RX_ENABLED) || (Debug_HD_ENABLED) */

#if(Debug_INTERNAL_CLOCK_USED)
    #define Debug_INTCLOCK_CLKEN         Debug_INTCLOCK_CLKEN_REG
#endif /* End Debug_INTERNAL_CLOCK_USED */

#define Debug_WAIT_FOR_COMLETE_REINIT    Debug_WAIT_FOR_COMPLETE_REINIT

#endif  /* CY_UART_Debug_H */


/* [] END OF FILE */
