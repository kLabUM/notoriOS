/*******************************************************************************
* File Name: Modem.h
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


#if !defined(CY_UART_Modem_H)
#define CY_UART_Modem_H

#include "cyfitter.h"
#include "cytypes.h"
#include "CyLib.h" /* For CyEnterCriticalSection() and CyExitCriticalSection() functions */


/***************************************
* Conditional Compilation Parameters
***************************************/

#define Modem_RX_ENABLED                     (1u)
#define Modem_TX_ENABLED                     (1u)
#define Modem_HD_ENABLED                     (0u)
#define Modem_RX_INTERRUPT_ENABLED           (0u)
#define Modem_TX_INTERRUPT_ENABLED           (0u)
#define Modem_INTERNAL_CLOCK_USED            (1u)
#define Modem_RXHW_ADDRESS_ENABLED           (0u)
#define Modem_OVER_SAMPLE_COUNT              (8u)
#define Modem_PARITY_TYPE                    (0u)
#define Modem_PARITY_TYPE_SW                 (0u)
#define Modem_BREAK_DETECT                   (0u)
#define Modem_BREAK_BITS_TX                  (13u)
#define Modem_BREAK_BITS_RX                  (13u)
#define Modem_TXCLKGEN_DP                    (1u)
#define Modem_USE23POLLING                   (1u)
#define Modem_FLOW_CONTROL                   (0u)
#define Modem_CLK_FREQ                       (0u)
#define Modem_TX_BUFFER_SIZE                 (4u)
#define Modem_RX_BUFFER_SIZE                 (4u)

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component UART_v2_50 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */

#if defined(Modem_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG)
    #define Modem_CONTROL_REG_REMOVED            (0u)
#else
    #define Modem_CONTROL_REG_REMOVED            (1u)
#endif /* End Modem_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */


/***************************************
*      Data Structure Definition
***************************************/

/* Sleep Mode API Support */
typedef struct Modem_backupStruct_
{
    uint8 enableState;

    #if(Modem_CONTROL_REG_REMOVED == 0u)
        uint8 cr;
    #endif /* End Modem_CONTROL_REG_REMOVED */

} Modem_BACKUP_STRUCT;


/***************************************
*       Function Prototypes
***************************************/

void Modem_Start(void) ;
void Modem_Stop(void) ;
uint8 Modem_ReadControlRegister(void) ;
void Modem_WriteControlRegister(uint8 control) ;

void Modem_Init(void) ;
void Modem_Enable(void) ;
void Modem_SaveConfig(void) ;
void Modem_RestoreConfig(void) ;
void Modem_Sleep(void) ;
void Modem_Wakeup(void) ;

/* Only if RX is enabled */
#if( (Modem_RX_ENABLED) || (Modem_HD_ENABLED) )

    #if (Modem_RX_INTERRUPT_ENABLED)
        #define Modem_EnableRxInt()  CyIntEnable (Modem_RX_VECT_NUM)
        #define Modem_DisableRxInt() CyIntDisable(Modem_RX_VECT_NUM)
        CY_ISR_PROTO(Modem_RXISR);
    #endif /* Modem_RX_INTERRUPT_ENABLED */

    void Modem_SetRxAddressMode(uint8 addressMode)
                                                           ;
    void Modem_SetRxAddress1(uint8 address) ;
    void Modem_SetRxAddress2(uint8 address) ;

    void  Modem_SetRxInterruptMode(uint8 intSrc) ;
    uint8 Modem_ReadRxData(void) ;
    uint8 Modem_ReadRxStatus(void) ;
    uint8 Modem_GetChar(void) ;
    uint16 Modem_GetByte(void) ;
    uint8 Modem_GetRxBufferSize(void)
                                                            ;
    void Modem_ClearRxBuffer(void) ;

    /* Obsolete functions, defines for backward compatible */
    #define Modem_GetRxInterruptSource   Modem_ReadRxStatus

#endif /* End (Modem_RX_ENABLED) || (Modem_HD_ENABLED) */

/* Only if TX is enabled */
#if(Modem_TX_ENABLED || Modem_HD_ENABLED)

    #if(Modem_TX_INTERRUPT_ENABLED)
        #define Modem_EnableTxInt()  CyIntEnable (Modem_TX_VECT_NUM)
        #define Modem_DisableTxInt() CyIntDisable(Modem_TX_VECT_NUM)
        #define Modem_SetPendingTxInt() CyIntSetPending(Modem_TX_VECT_NUM)
        #define Modem_ClearPendingTxInt() CyIntClearPending(Modem_TX_VECT_NUM)
        CY_ISR_PROTO(Modem_TXISR);
    #endif /* Modem_TX_INTERRUPT_ENABLED */

    void Modem_SetTxInterruptMode(uint8 intSrc) ;
    void Modem_WriteTxData(uint8 txDataByte) ;
    uint8 Modem_ReadTxStatus(void) ;
    void Modem_PutChar(uint8 txDataByte) ;
    void Modem_PutString(const char8 string[]) ;
    void Modem_PutArray(const uint8 string[], uint8 byteCount)
                                                            ;
    void Modem_PutCRLF(uint8 txDataByte) ;
    void Modem_ClearTxBuffer(void) ;
    void Modem_SetTxAddressMode(uint8 addressMode) ;
    void Modem_SendBreak(uint8 retMode) ;
    uint8 Modem_GetTxBufferSize(void)
                                                            ;
    /* Obsolete functions, defines for backward compatible */
    #define Modem_PutStringConst         Modem_PutString
    #define Modem_PutArrayConst          Modem_PutArray
    #define Modem_GetTxInterruptSource   Modem_ReadTxStatus

#endif /* End Modem_TX_ENABLED || Modem_HD_ENABLED */

#if(Modem_HD_ENABLED)
    void Modem_LoadRxConfig(void) ;
    void Modem_LoadTxConfig(void) ;
#endif /* End Modem_HD_ENABLED */


/* Communication bootloader APIs */
#if defined(CYDEV_BOOTLOADER_IO_COMP) && ((CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Modem) || \
                                          (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Custom_Interface))
    /* Physical layer functions */
    void    Modem_CyBtldrCommStart(void) CYSMALL ;
    void    Modem_CyBtldrCommStop(void) CYSMALL ;
    void    Modem_CyBtldrCommReset(void) CYSMALL ;
    cystatus Modem_CyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;
    cystatus Modem_CyBtldrCommRead(uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;

    #if (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Modem)
        #define CyBtldrCommStart    Modem_CyBtldrCommStart
        #define CyBtldrCommStop     Modem_CyBtldrCommStop
        #define CyBtldrCommReset    Modem_CyBtldrCommReset
        #define CyBtldrCommWrite    Modem_CyBtldrCommWrite
        #define CyBtldrCommRead     Modem_CyBtldrCommRead
    #endif  /* (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Modem) */

    /* Byte to Byte time out for detecting end of block data from host */
    #define Modem_BYTE2BYTE_TIME_OUT (25u)
    #define Modem_PACKET_EOP         (0x17u) /* End of packet defined by bootloader */
    #define Modem_WAIT_EOP_DELAY     (5u)    /* Additional 5ms to wait for End of packet */
    #define Modem_BL_CHK_DELAY_MS    (1u)    /* Time Out quantity equal 1mS */

#endif /* CYDEV_BOOTLOADER_IO_COMP */


/***************************************
*          API Constants
***************************************/
/* Parameters for SetTxAddressMode API*/
#define Modem_SET_SPACE      (0x00u)
#define Modem_SET_MARK       (0x01u)

/* Status Register definitions */
#if( (Modem_TX_ENABLED) || (Modem_HD_ENABLED) )
    #if(Modem_TX_INTERRUPT_ENABLED)
        #define Modem_TX_VECT_NUM            (uint8)Modem_TXInternalInterrupt__INTC_NUMBER
        #define Modem_TX_PRIOR_NUM           (uint8)Modem_TXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* Modem_TX_INTERRUPT_ENABLED */

    #define Modem_TX_STS_COMPLETE_SHIFT          (0x00u)
    #define Modem_TX_STS_FIFO_EMPTY_SHIFT        (0x01u)
    #define Modem_TX_STS_FIFO_NOT_FULL_SHIFT     (0x03u)
    #if(Modem_TX_ENABLED)
        #define Modem_TX_STS_FIFO_FULL_SHIFT     (0x02u)
    #else /* (Modem_HD_ENABLED) */
        #define Modem_TX_STS_FIFO_FULL_SHIFT     (0x05u)  /* Needs MD=0 */
    #endif /* (Modem_TX_ENABLED) */

    #define Modem_TX_STS_COMPLETE            (uint8)(0x01u << Modem_TX_STS_COMPLETE_SHIFT)
    #define Modem_TX_STS_FIFO_EMPTY          (uint8)(0x01u << Modem_TX_STS_FIFO_EMPTY_SHIFT)
    #define Modem_TX_STS_FIFO_FULL           (uint8)(0x01u << Modem_TX_STS_FIFO_FULL_SHIFT)
    #define Modem_TX_STS_FIFO_NOT_FULL       (uint8)(0x01u << Modem_TX_STS_FIFO_NOT_FULL_SHIFT)
#endif /* End (Modem_TX_ENABLED) || (Modem_HD_ENABLED)*/

#if( (Modem_RX_ENABLED) || (Modem_HD_ENABLED) )
    #if(Modem_RX_INTERRUPT_ENABLED)
        #define Modem_RX_VECT_NUM            (uint8)Modem_RXInternalInterrupt__INTC_NUMBER
        #define Modem_RX_PRIOR_NUM           (uint8)Modem_RXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* Modem_RX_INTERRUPT_ENABLED */
    #define Modem_RX_STS_MRKSPC_SHIFT            (0x00u)
    #define Modem_RX_STS_BREAK_SHIFT             (0x01u)
    #define Modem_RX_STS_PAR_ERROR_SHIFT         (0x02u)
    #define Modem_RX_STS_STOP_ERROR_SHIFT        (0x03u)
    #define Modem_RX_STS_OVERRUN_SHIFT           (0x04u)
    #define Modem_RX_STS_FIFO_NOTEMPTY_SHIFT     (0x05u)
    #define Modem_RX_STS_ADDR_MATCH_SHIFT        (0x06u)
    #define Modem_RX_STS_SOFT_BUFF_OVER_SHIFT    (0x07u)

    #define Modem_RX_STS_MRKSPC           (uint8)(0x01u << Modem_RX_STS_MRKSPC_SHIFT)
    #define Modem_RX_STS_BREAK            (uint8)(0x01u << Modem_RX_STS_BREAK_SHIFT)
    #define Modem_RX_STS_PAR_ERROR        (uint8)(0x01u << Modem_RX_STS_PAR_ERROR_SHIFT)
    #define Modem_RX_STS_STOP_ERROR       (uint8)(0x01u << Modem_RX_STS_STOP_ERROR_SHIFT)
    #define Modem_RX_STS_OVERRUN          (uint8)(0x01u << Modem_RX_STS_OVERRUN_SHIFT)
    #define Modem_RX_STS_FIFO_NOTEMPTY    (uint8)(0x01u << Modem_RX_STS_FIFO_NOTEMPTY_SHIFT)
    #define Modem_RX_STS_ADDR_MATCH       (uint8)(0x01u << Modem_RX_STS_ADDR_MATCH_SHIFT)
    #define Modem_RX_STS_SOFT_BUFF_OVER   (uint8)(0x01u << Modem_RX_STS_SOFT_BUFF_OVER_SHIFT)
    #define Modem_RX_HW_MASK                     (0x7Fu)
#endif /* End (Modem_RX_ENABLED) || (Modem_HD_ENABLED) */

/* Control Register definitions */
#define Modem_CTRL_HD_SEND_SHIFT                 (0x00u) /* 1 enable TX part in Half Duplex mode */
#define Modem_CTRL_HD_SEND_BREAK_SHIFT           (0x01u) /* 1 send BREAK signal in Half Duplez mode */
#define Modem_CTRL_MARK_SHIFT                    (0x02u) /* 1 sets mark, 0 sets space */
#define Modem_CTRL_PARITY_TYPE0_SHIFT            (0x03u) /* Defines the type of parity implemented */
#define Modem_CTRL_PARITY_TYPE1_SHIFT            (0x04u) /* Defines the type of parity implemented */
#define Modem_CTRL_RXADDR_MODE0_SHIFT            (0x05u)
#define Modem_CTRL_RXADDR_MODE1_SHIFT            (0x06u)
#define Modem_CTRL_RXADDR_MODE2_SHIFT            (0x07u)

#define Modem_CTRL_HD_SEND               (uint8)(0x01u << Modem_CTRL_HD_SEND_SHIFT)
#define Modem_CTRL_HD_SEND_BREAK         (uint8)(0x01u << Modem_CTRL_HD_SEND_BREAK_SHIFT)
#define Modem_CTRL_MARK                  (uint8)(0x01u << Modem_CTRL_MARK_SHIFT)
#define Modem_CTRL_PARITY_TYPE_MASK      (uint8)(0x03u << Modem_CTRL_PARITY_TYPE0_SHIFT)
#define Modem_CTRL_RXADDR_MODE_MASK      (uint8)(0x07u << Modem_CTRL_RXADDR_MODE0_SHIFT)

/* StatusI Register Interrupt Enable Control Bits. As defined by the Register map for the AUX Control Register */
#define Modem_INT_ENABLE                         (0x10u)

/* Bit Counter (7-bit) Control Register Bit Definitions. As defined by the Register map for the AUX Control Register */
#define Modem_CNTR_ENABLE                        (0x20u)

/*   Constants for SendBreak() "retMode" parameter  */
#define Modem_SEND_BREAK                         (0x00u)
#define Modem_WAIT_FOR_COMPLETE_REINIT           (0x01u)
#define Modem_REINIT                             (0x02u)
#define Modem_SEND_WAIT_REINIT                   (0x03u)

#define Modem_OVER_SAMPLE_8                      (8u)
#define Modem_OVER_SAMPLE_16                     (16u)

#define Modem_BIT_CENTER                         (Modem_OVER_SAMPLE_COUNT - 2u)

#define Modem_FIFO_LENGTH                        (4u)
#define Modem_NUMBER_OF_START_BIT                (1u)
#define Modem_MAX_BYTE_VALUE                     (0xFFu)

/* 8X always for count7 implementation */
#define Modem_TXBITCTR_BREAKBITS8X   ((Modem_BREAK_BITS_TX * Modem_OVER_SAMPLE_8) - 1u)
/* 8X or 16X for DP implementation */
#define Modem_TXBITCTR_BREAKBITS ((Modem_BREAK_BITS_TX * Modem_OVER_SAMPLE_COUNT) - 1u)

#define Modem_HALF_BIT_COUNT   \
                            (((Modem_OVER_SAMPLE_COUNT / 2u) + (Modem_USE23POLLING * 1u)) - 2u)
#if (Modem_OVER_SAMPLE_COUNT == Modem_OVER_SAMPLE_8)
    #define Modem_HD_TXBITCTR_INIT   (((Modem_BREAK_BITS_TX + \
                            Modem_NUMBER_OF_START_BIT) * Modem_OVER_SAMPLE_COUNT) - 1u)

    /* This parameter is increased on the 2 in 2 out of 3 mode to sample voting in the middle */
    #define Modem_RXBITCTR_INIT  ((((Modem_BREAK_BITS_RX + Modem_NUMBER_OF_START_BIT) \
                            * Modem_OVER_SAMPLE_COUNT) + Modem_HALF_BIT_COUNT) - 1u)

#else /* Modem_OVER_SAMPLE_COUNT == Modem_OVER_SAMPLE_16 */
    #define Modem_HD_TXBITCTR_INIT   ((8u * Modem_OVER_SAMPLE_COUNT) - 1u)
    /* 7bit counter need one more bit for OverSampleCount = 16 */
    #define Modem_RXBITCTR_INIT      (((7u * Modem_OVER_SAMPLE_COUNT) - 1u) + \
                                                      Modem_HALF_BIT_COUNT)
#endif /* End Modem_OVER_SAMPLE_COUNT */

#define Modem_HD_RXBITCTR_INIT                   Modem_RXBITCTR_INIT


/***************************************
* Global variables external identifier
***************************************/

extern uint8 Modem_initVar;
#if (Modem_TX_INTERRUPT_ENABLED && Modem_TX_ENABLED)
    extern volatile uint8 Modem_txBuffer[Modem_TX_BUFFER_SIZE];
    extern volatile uint8 Modem_txBufferRead;
    extern uint8 Modem_txBufferWrite;
#endif /* (Modem_TX_INTERRUPT_ENABLED && Modem_TX_ENABLED) */
#if (Modem_RX_INTERRUPT_ENABLED && (Modem_RX_ENABLED || Modem_HD_ENABLED))
    extern uint8 Modem_errorStatus;
    extern volatile uint8 Modem_rxBuffer[Modem_RX_BUFFER_SIZE];
    extern volatile uint8 Modem_rxBufferRead;
    extern volatile uint8 Modem_rxBufferWrite;
    extern volatile uint8 Modem_rxBufferLoopDetect;
    extern volatile uint8 Modem_rxBufferOverflow;
    #if (Modem_RXHW_ADDRESS_ENABLED)
        extern volatile uint8 Modem_rxAddressMode;
        extern volatile uint8 Modem_rxAddressDetected;
    #endif /* (Modem_RXHW_ADDRESS_ENABLED) */
#endif /* (Modem_RX_INTERRUPT_ENABLED && (Modem_RX_ENABLED || Modem_HD_ENABLED)) */


/***************************************
* Enumerated Types and Parameters
***************************************/

#define Modem__B_UART__AM_SW_BYTE_BYTE 1
#define Modem__B_UART__AM_SW_DETECT_TO_BUFFER 2
#define Modem__B_UART__AM_HW_BYTE_BY_BYTE 3
#define Modem__B_UART__AM_HW_DETECT_TO_BUFFER 4
#define Modem__B_UART__AM_NONE 0

#define Modem__B_UART__NONE_REVB 0
#define Modem__B_UART__EVEN_REVB 1
#define Modem__B_UART__ODD_REVB 2
#define Modem__B_UART__MARK_SPACE_REVB 3



/***************************************
*    Initial Parameter Constants
***************************************/

/* UART shifts max 8 bits, Mark/Space functionality working if 9 selected */
#define Modem_NUMBER_OF_DATA_BITS    ((8u > 8u) ? 8u : 8u)
#define Modem_NUMBER_OF_STOP_BITS    (1u)

#if (Modem_RXHW_ADDRESS_ENABLED)
    #define Modem_RX_ADDRESS_MODE    (0u)
    #define Modem_RX_HW_ADDRESS1     (0u)
    #define Modem_RX_HW_ADDRESS2     (0u)
#endif /* (Modem_RXHW_ADDRESS_ENABLED) */

#define Modem_INIT_RX_INTERRUPTS_MASK \
                                  (uint8)((1 << Modem_RX_STS_FIFO_NOTEMPTY_SHIFT) \
                                        | (0 << Modem_RX_STS_MRKSPC_SHIFT) \
                                        | (0 << Modem_RX_STS_ADDR_MATCH_SHIFT) \
                                        | (0 << Modem_RX_STS_PAR_ERROR_SHIFT) \
                                        | (0 << Modem_RX_STS_STOP_ERROR_SHIFT) \
                                        | (0 << Modem_RX_STS_BREAK_SHIFT) \
                                        | (0 << Modem_RX_STS_OVERRUN_SHIFT))

#define Modem_INIT_TX_INTERRUPTS_MASK \
                                  (uint8)((0 << Modem_TX_STS_COMPLETE_SHIFT) \
                                        | (0 << Modem_TX_STS_FIFO_EMPTY_SHIFT) \
                                        | (0 << Modem_TX_STS_FIFO_FULL_SHIFT) \
                                        | (0 << Modem_TX_STS_FIFO_NOT_FULL_SHIFT))


/***************************************
*              Registers
***************************************/

#ifdef Modem_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define Modem_CONTROL_REG \
                            (* (reg8 *) Modem_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
    #define Modem_CONTROL_PTR \
                            (  (reg8 *) Modem_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
#endif /* End Modem_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(Modem_TX_ENABLED)
    #define Modem_TXDATA_REG          (* (reg8 *) Modem_BUART_sTX_TxShifter_u0__F0_REG)
    #define Modem_TXDATA_PTR          (  (reg8 *) Modem_BUART_sTX_TxShifter_u0__F0_REG)
    #define Modem_TXDATA_AUX_CTL_REG  (* (reg8 *) Modem_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define Modem_TXDATA_AUX_CTL_PTR  (  (reg8 *) Modem_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define Modem_TXSTATUS_REG        (* (reg8 *) Modem_BUART_sTX_TxSts__STATUS_REG)
    #define Modem_TXSTATUS_PTR        (  (reg8 *) Modem_BUART_sTX_TxSts__STATUS_REG)
    #define Modem_TXSTATUS_MASK_REG   (* (reg8 *) Modem_BUART_sTX_TxSts__MASK_REG)
    #define Modem_TXSTATUS_MASK_PTR   (  (reg8 *) Modem_BUART_sTX_TxSts__MASK_REG)
    #define Modem_TXSTATUS_ACTL_REG   (* (reg8 *) Modem_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)
    #define Modem_TXSTATUS_ACTL_PTR   (  (reg8 *) Modem_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)

    /* DP clock */
    #if(Modem_TXCLKGEN_DP)
        #define Modem_TXBITCLKGEN_CTR_REG        \
                                        (* (reg8 *) Modem_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define Modem_TXBITCLKGEN_CTR_PTR        \
                                        (  (reg8 *) Modem_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define Modem_TXBITCLKTX_COMPLETE_REG    \
                                        (* (reg8 *) Modem_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
        #define Modem_TXBITCLKTX_COMPLETE_PTR    \
                                        (  (reg8 *) Modem_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
    #else     /* Count7 clock*/
        #define Modem_TXBITCTR_PERIOD_REG    \
                                        (* (reg8 *) Modem_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define Modem_TXBITCTR_PERIOD_PTR    \
                                        (  (reg8 *) Modem_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define Modem_TXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) Modem_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define Modem_TXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) Modem_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define Modem_TXBITCTR_COUNTER_REG   \
                                        (* (reg8 *) Modem_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
        #define Modem_TXBITCTR_COUNTER_PTR   \
                                        (  (reg8 *) Modem_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
    #endif /* Modem_TXCLKGEN_DP */

#endif /* End Modem_TX_ENABLED */

#if(Modem_HD_ENABLED)

    #define Modem_TXDATA_REG             (* (reg8 *) Modem_BUART_sRX_RxShifter_u0__F1_REG )
    #define Modem_TXDATA_PTR             (  (reg8 *) Modem_BUART_sRX_RxShifter_u0__F1_REG )
    #define Modem_TXDATA_AUX_CTL_REG     (* (reg8 *) Modem_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)
    #define Modem_TXDATA_AUX_CTL_PTR     (  (reg8 *) Modem_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define Modem_TXSTATUS_REG           (* (reg8 *) Modem_BUART_sRX_RxSts__STATUS_REG )
    #define Modem_TXSTATUS_PTR           (  (reg8 *) Modem_BUART_sRX_RxSts__STATUS_REG )
    #define Modem_TXSTATUS_MASK_REG      (* (reg8 *) Modem_BUART_sRX_RxSts__MASK_REG )
    #define Modem_TXSTATUS_MASK_PTR      (  (reg8 *) Modem_BUART_sRX_RxSts__MASK_REG )
    #define Modem_TXSTATUS_ACTL_REG      (* (reg8 *) Modem_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define Modem_TXSTATUS_ACTL_PTR      (  (reg8 *) Modem_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End Modem_HD_ENABLED */

#if( (Modem_RX_ENABLED) || (Modem_HD_ENABLED) )
    #define Modem_RXDATA_REG             (* (reg8 *) Modem_BUART_sRX_RxShifter_u0__F0_REG )
    #define Modem_RXDATA_PTR             (  (reg8 *) Modem_BUART_sRX_RxShifter_u0__F0_REG )
    #define Modem_RXADDRESS1_REG         (* (reg8 *) Modem_BUART_sRX_RxShifter_u0__D0_REG )
    #define Modem_RXADDRESS1_PTR         (  (reg8 *) Modem_BUART_sRX_RxShifter_u0__D0_REG )
    #define Modem_RXADDRESS2_REG         (* (reg8 *) Modem_BUART_sRX_RxShifter_u0__D1_REG )
    #define Modem_RXADDRESS2_PTR         (  (reg8 *) Modem_BUART_sRX_RxShifter_u0__D1_REG )
    #define Modem_RXDATA_AUX_CTL_REG     (* (reg8 *) Modem_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define Modem_RXBITCTR_PERIOD_REG    (* (reg8 *) Modem_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define Modem_RXBITCTR_PERIOD_PTR    (  (reg8 *) Modem_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define Modem_RXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) Modem_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define Modem_RXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) Modem_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define Modem_RXBITCTR_COUNTER_REG   (* (reg8 *) Modem_BUART_sRX_RxBitCounter__COUNT_REG )
    #define Modem_RXBITCTR_COUNTER_PTR   (  (reg8 *) Modem_BUART_sRX_RxBitCounter__COUNT_REG )

    #define Modem_RXSTATUS_REG           (* (reg8 *) Modem_BUART_sRX_RxSts__STATUS_REG )
    #define Modem_RXSTATUS_PTR           (  (reg8 *) Modem_BUART_sRX_RxSts__STATUS_REG )
    #define Modem_RXSTATUS_MASK_REG      (* (reg8 *) Modem_BUART_sRX_RxSts__MASK_REG )
    #define Modem_RXSTATUS_MASK_PTR      (  (reg8 *) Modem_BUART_sRX_RxSts__MASK_REG )
    #define Modem_RXSTATUS_ACTL_REG      (* (reg8 *) Modem_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define Modem_RXSTATUS_ACTL_PTR      (  (reg8 *) Modem_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End  (Modem_RX_ENABLED) || (Modem_HD_ENABLED) */

#if(Modem_INTERNAL_CLOCK_USED)
    /* Register to enable or disable the digital clocks */
    #define Modem_INTCLOCK_CLKEN_REG     (* (reg8 *) Modem_IntClock__PM_ACT_CFG)
    #define Modem_INTCLOCK_CLKEN_PTR     (  (reg8 *) Modem_IntClock__PM_ACT_CFG)

    /* Clock mask for this clock. */
    #define Modem_INTCLOCK_CLKEN_MASK    Modem_IntClock__PM_ACT_MSK
#endif /* End Modem_INTERNAL_CLOCK_USED */


/***************************************
*       Register Constants
***************************************/

#if(Modem_TX_ENABLED)
    #define Modem_TX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End Modem_TX_ENABLED */

#if(Modem_HD_ENABLED)
    #define Modem_TX_FIFO_CLR            (0x02u) /* FIFO1 CLR */
#endif /* End Modem_HD_ENABLED */

#if( (Modem_RX_ENABLED) || (Modem_HD_ENABLED) )
    #define Modem_RX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End  (Modem_RX_ENABLED) || (Modem_HD_ENABLED) */


/***************************************
* The following code is DEPRECATED and
* should not be used in new projects.
***************************************/

/* UART v2_40 obsolete definitions */
#define Modem_WAIT_1_MS      Modem_BL_CHK_DELAY_MS   

#define Modem_TXBUFFERSIZE   Modem_TX_BUFFER_SIZE
#define Modem_RXBUFFERSIZE   Modem_RX_BUFFER_SIZE

#if (Modem_RXHW_ADDRESS_ENABLED)
    #define Modem_RXADDRESSMODE  Modem_RX_ADDRESS_MODE
    #define Modem_RXHWADDRESS1   Modem_RX_HW_ADDRESS1
    #define Modem_RXHWADDRESS2   Modem_RX_HW_ADDRESS2
    /* Backward compatible define */
    #define Modem_RXAddressMode  Modem_RXADDRESSMODE
#endif /* (Modem_RXHW_ADDRESS_ENABLED) */

/* UART v2_30 obsolete definitions */
#define Modem_initvar                    Modem_initVar

#define Modem_RX_Enabled                 Modem_RX_ENABLED
#define Modem_TX_Enabled                 Modem_TX_ENABLED
#define Modem_HD_Enabled                 Modem_HD_ENABLED
#define Modem_RX_IntInterruptEnabled     Modem_RX_INTERRUPT_ENABLED
#define Modem_TX_IntInterruptEnabled     Modem_TX_INTERRUPT_ENABLED
#define Modem_InternalClockUsed          Modem_INTERNAL_CLOCK_USED
#define Modem_RXHW_Address_Enabled       Modem_RXHW_ADDRESS_ENABLED
#define Modem_OverSampleCount            Modem_OVER_SAMPLE_COUNT
#define Modem_ParityType                 Modem_PARITY_TYPE

#if( Modem_TX_ENABLED && (Modem_TXBUFFERSIZE > Modem_FIFO_LENGTH))
    #define Modem_TXBUFFER               Modem_txBuffer
    #define Modem_TXBUFFERREAD           Modem_txBufferRead
    #define Modem_TXBUFFERWRITE          Modem_txBufferWrite
#endif /* End Modem_TX_ENABLED */
#if( ( Modem_RX_ENABLED || Modem_HD_ENABLED ) && \
     (Modem_RXBUFFERSIZE > Modem_FIFO_LENGTH) )
    #define Modem_RXBUFFER               Modem_rxBuffer
    #define Modem_RXBUFFERREAD           Modem_rxBufferRead
    #define Modem_RXBUFFERWRITE          Modem_rxBufferWrite
    #define Modem_RXBUFFERLOOPDETECT     Modem_rxBufferLoopDetect
    #define Modem_RXBUFFER_OVERFLOW      Modem_rxBufferOverflow
#endif /* End Modem_RX_ENABLED */

#ifdef Modem_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define Modem_CONTROL                Modem_CONTROL_REG
#endif /* End Modem_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(Modem_TX_ENABLED)
    #define Modem_TXDATA                 Modem_TXDATA_REG
    #define Modem_TXSTATUS               Modem_TXSTATUS_REG
    #define Modem_TXSTATUS_MASK          Modem_TXSTATUS_MASK_REG
    #define Modem_TXSTATUS_ACTL          Modem_TXSTATUS_ACTL_REG
    /* DP clock */
    #if(Modem_TXCLKGEN_DP)
        #define Modem_TXBITCLKGEN_CTR        Modem_TXBITCLKGEN_CTR_REG
        #define Modem_TXBITCLKTX_COMPLETE    Modem_TXBITCLKTX_COMPLETE_REG
    #else     /* Count7 clock*/
        #define Modem_TXBITCTR_PERIOD        Modem_TXBITCTR_PERIOD_REG
        #define Modem_TXBITCTR_CONTROL       Modem_TXBITCTR_CONTROL_REG
        #define Modem_TXBITCTR_COUNTER       Modem_TXBITCTR_COUNTER_REG
    #endif /* Modem_TXCLKGEN_DP */
#endif /* End Modem_TX_ENABLED */

#if(Modem_HD_ENABLED)
    #define Modem_TXDATA                 Modem_TXDATA_REG
    #define Modem_TXSTATUS               Modem_TXSTATUS_REG
    #define Modem_TXSTATUS_MASK          Modem_TXSTATUS_MASK_REG
    #define Modem_TXSTATUS_ACTL          Modem_TXSTATUS_ACTL_REG
#endif /* End Modem_HD_ENABLED */

#if( (Modem_RX_ENABLED) || (Modem_HD_ENABLED) )
    #define Modem_RXDATA                 Modem_RXDATA_REG
    #define Modem_RXADDRESS1             Modem_RXADDRESS1_REG
    #define Modem_RXADDRESS2             Modem_RXADDRESS2_REG
    #define Modem_RXBITCTR_PERIOD        Modem_RXBITCTR_PERIOD_REG
    #define Modem_RXBITCTR_CONTROL       Modem_RXBITCTR_CONTROL_REG
    #define Modem_RXBITCTR_COUNTER       Modem_RXBITCTR_COUNTER_REG
    #define Modem_RXSTATUS               Modem_RXSTATUS_REG
    #define Modem_RXSTATUS_MASK          Modem_RXSTATUS_MASK_REG
    #define Modem_RXSTATUS_ACTL          Modem_RXSTATUS_ACTL_REG
#endif /* End  (Modem_RX_ENABLED) || (Modem_HD_ENABLED) */

#if(Modem_INTERNAL_CLOCK_USED)
    #define Modem_INTCLOCK_CLKEN         Modem_INTCLOCK_CLKEN_REG
#endif /* End Modem_INTERNAL_CLOCK_USED */

#define Modem_WAIT_FOR_COMLETE_REINIT    Modem_WAIT_FOR_COMPLETE_REINIT

#endif  /* CY_UART_Modem_H */


/* [] END OF FILE */
