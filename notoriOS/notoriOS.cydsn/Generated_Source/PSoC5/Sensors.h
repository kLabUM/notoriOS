/*******************************************************************************
* File Name: Sensors.h
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


#if !defined(CY_UART_Sensors_H)
#define CY_UART_Sensors_H

#include "cyfitter.h"
#include "cytypes.h"
#include "CyLib.h" /* For CyEnterCriticalSection() and CyExitCriticalSection() functions */


/***************************************
* Conditional Compilation Parameters
***************************************/

#define Sensors_RX_ENABLED                     (1u)
#define Sensors_TX_ENABLED                     (0u)
#define Sensors_HD_ENABLED                     (0u)
#define Sensors_RX_INTERRUPT_ENABLED           (0u)
#define Sensors_TX_INTERRUPT_ENABLED           (0u)
#define Sensors_INTERNAL_CLOCK_USED            (0u)
#define Sensors_RXHW_ADDRESS_ENABLED           (0u)
#define Sensors_OVER_SAMPLE_COUNT              (8u)
#define Sensors_PARITY_TYPE                    (0u)
#define Sensors_PARITY_TYPE_SW                 (0u)
#define Sensors_BREAK_DETECT                   (0u)
#define Sensors_BREAK_BITS_TX                  (13u)
#define Sensors_BREAK_BITS_RX                  (13u)
#define Sensors_TXCLKGEN_DP                    (1u)
#define Sensors_USE23POLLING                   (1u)
#define Sensors_FLOW_CONTROL                   (0u)
#define Sensors_CLK_FREQ                       (0u)
#define Sensors_TX_BUFFER_SIZE                 (4u)
#define Sensors_RX_BUFFER_SIZE                 (4u)

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component UART_v2_50 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */

#if defined(Sensors_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG)
    #define Sensors_CONTROL_REG_REMOVED            (0u)
#else
    #define Sensors_CONTROL_REG_REMOVED            (1u)
#endif /* End Sensors_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */


/***************************************
*      Data Structure Definition
***************************************/

/* Sleep Mode API Support */
typedef struct Sensors_backupStruct_
{
    uint8 enableState;

    #if(Sensors_CONTROL_REG_REMOVED == 0u)
        uint8 cr;
    #endif /* End Sensors_CONTROL_REG_REMOVED */

} Sensors_BACKUP_STRUCT;


/***************************************
*       Function Prototypes
***************************************/

void Sensors_Start(void) ;
void Sensors_Stop(void) ;
uint8 Sensors_ReadControlRegister(void) ;
void Sensors_WriteControlRegister(uint8 control) ;

void Sensors_Init(void) ;
void Sensors_Enable(void) ;
void Sensors_SaveConfig(void) ;
void Sensors_RestoreConfig(void) ;
void Sensors_Sleep(void) ;
void Sensors_Wakeup(void) ;

/* Only if RX is enabled */
#if( (Sensors_RX_ENABLED) || (Sensors_HD_ENABLED) )

    #if (Sensors_RX_INTERRUPT_ENABLED)
        #define Sensors_EnableRxInt()  CyIntEnable (Sensors_RX_VECT_NUM)
        #define Sensors_DisableRxInt() CyIntDisable(Sensors_RX_VECT_NUM)
        CY_ISR_PROTO(Sensors_RXISR);
    #endif /* Sensors_RX_INTERRUPT_ENABLED */

    void Sensors_SetRxAddressMode(uint8 addressMode)
                                                           ;
    void Sensors_SetRxAddress1(uint8 address) ;
    void Sensors_SetRxAddress2(uint8 address) ;

    void  Sensors_SetRxInterruptMode(uint8 intSrc) ;
    uint8 Sensors_ReadRxData(void) ;
    uint8 Sensors_ReadRxStatus(void) ;
    uint8 Sensors_GetChar(void) ;
    uint16 Sensors_GetByte(void) ;
    uint8 Sensors_GetRxBufferSize(void)
                                                            ;
    void Sensors_ClearRxBuffer(void) ;

    /* Obsolete functions, defines for backward compatible */
    #define Sensors_GetRxInterruptSource   Sensors_ReadRxStatus

#endif /* End (Sensors_RX_ENABLED) || (Sensors_HD_ENABLED) */

/* Only if TX is enabled */
#if(Sensors_TX_ENABLED || Sensors_HD_ENABLED)

    #if(Sensors_TX_INTERRUPT_ENABLED)
        #define Sensors_EnableTxInt()  CyIntEnable (Sensors_TX_VECT_NUM)
        #define Sensors_DisableTxInt() CyIntDisable(Sensors_TX_VECT_NUM)
        #define Sensors_SetPendingTxInt() CyIntSetPending(Sensors_TX_VECT_NUM)
        #define Sensors_ClearPendingTxInt() CyIntClearPending(Sensors_TX_VECT_NUM)
        CY_ISR_PROTO(Sensors_TXISR);
    #endif /* Sensors_TX_INTERRUPT_ENABLED */

    void Sensors_SetTxInterruptMode(uint8 intSrc) ;
    void Sensors_WriteTxData(uint8 txDataByte) ;
    uint8 Sensors_ReadTxStatus(void) ;
    void Sensors_PutChar(uint8 txDataByte) ;
    void Sensors_PutString(const char8 string[]) ;
    void Sensors_PutArray(const uint8 string[], uint8 byteCount)
                                                            ;
    void Sensors_PutCRLF(uint8 txDataByte) ;
    void Sensors_ClearTxBuffer(void) ;
    void Sensors_SetTxAddressMode(uint8 addressMode) ;
    void Sensors_SendBreak(uint8 retMode) ;
    uint8 Sensors_GetTxBufferSize(void)
                                                            ;
    /* Obsolete functions, defines for backward compatible */
    #define Sensors_PutStringConst         Sensors_PutString
    #define Sensors_PutArrayConst          Sensors_PutArray
    #define Sensors_GetTxInterruptSource   Sensors_ReadTxStatus

#endif /* End Sensors_TX_ENABLED || Sensors_HD_ENABLED */

#if(Sensors_HD_ENABLED)
    void Sensors_LoadRxConfig(void) ;
    void Sensors_LoadTxConfig(void) ;
#endif /* End Sensors_HD_ENABLED */


/* Communication bootloader APIs */
#if defined(CYDEV_BOOTLOADER_IO_COMP) && ((CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Sensors) || \
                                          (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Custom_Interface))
    /* Physical layer functions */
    void    Sensors_CyBtldrCommStart(void) CYSMALL ;
    void    Sensors_CyBtldrCommStop(void) CYSMALL ;
    void    Sensors_CyBtldrCommReset(void) CYSMALL ;
    cystatus Sensors_CyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;
    cystatus Sensors_CyBtldrCommRead(uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;

    #if (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Sensors)
        #define CyBtldrCommStart    Sensors_CyBtldrCommStart
        #define CyBtldrCommStop     Sensors_CyBtldrCommStop
        #define CyBtldrCommReset    Sensors_CyBtldrCommReset
        #define CyBtldrCommWrite    Sensors_CyBtldrCommWrite
        #define CyBtldrCommRead     Sensors_CyBtldrCommRead
    #endif  /* (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Sensors) */

    /* Byte to Byte time out for detecting end of block data from host */
    #define Sensors_BYTE2BYTE_TIME_OUT (25u)
    #define Sensors_PACKET_EOP         (0x17u) /* End of packet defined by bootloader */
    #define Sensors_WAIT_EOP_DELAY     (5u)    /* Additional 5ms to wait for End of packet */
    #define Sensors_BL_CHK_DELAY_MS    (1u)    /* Time Out quantity equal 1mS */

#endif /* CYDEV_BOOTLOADER_IO_COMP */


/***************************************
*          API Constants
***************************************/
/* Parameters for SetTxAddressMode API*/
#define Sensors_SET_SPACE      (0x00u)
#define Sensors_SET_MARK       (0x01u)

/* Status Register definitions */
#if( (Sensors_TX_ENABLED) || (Sensors_HD_ENABLED) )
    #if(Sensors_TX_INTERRUPT_ENABLED)
        #define Sensors_TX_VECT_NUM            (uint8)Sensors_TXInternalInterrupt__INTC_NUMBER
        #define Sensors_TX_PRIOR_NUM           (uint8)Sensors_TXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* Sensors_TX_INTERRUPT_ENABLED */

    #define Sensors_TX_STS_COMPLETE_SHIFT          (0x00u)
    #define Sensors_TX_STS_FIFO_EMPTY_SHIFT        (0x01u)
    #define Sensors_TX_STS_FIFO_NOT_FULL_SHIFT     (0x03u)
    #if(Sensors_TX_ENABLED)
        #define Sensors_TX_STS_FIFO_FULL_SHIFT     (0x02u)
    #else /* (Sensors_HD_ENABLED) */
        #define Sensors_TX_STS_FIFO_FULL_SHIFT     (0x05u)  /* Needs MD=0 */
    #endif /* (Sensors_TX_ENABLED) */

    #define Sensors_TX_STS_COMPLETE            (uint8)(0x01u << Sensors_TX_STS_COMPLETE_SHIFT)
    #define Sensors_TX_STS_FIFO_EMPTY          (uint8)(0x01u << Sensors_TX_STS_FIFO_EMPTY_SHIFT)
    #define Sensors_TX_STS_FIFO_FULL           (uint8)(0x01u << Sensors_TX_STS_FIFO_FULL_SHIFT)
    #define Sensors_TX_STS_FIFO_NOT_FULL       (uint8)(0x01u << Sensors_TX_STS_FIFO_NOT_FULL_SHIFT)
#endif /* End (Sensors_TX_ENABLED) || (Sensors_HD_ENABLED)*/

#if( (Sensors_RX_ENABLED) || (Sensors_HD_ENABLED) )
    #if(Sensors_RX_INTERRUPT_ENABLED)
        #define Sensors_RX_VECT_NUM            (uint8)Sensors_RXInternalInterrupt__INTC_NUMBER
        #define Sensors_RX_PRIOR_NUM           (uint8)Sensors_RXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* Sensors_RX_INTERRUPT_ENABLED */
    #define Sensors_RX_STS_MRKSPC_SHIFT            (0x00u)
    #define Sensors_RX_STS_BREAK_SHIFT             (0x01u)
    #define Sensors_RX_STS_PAR_ERROR_SHIFT         (0x02u)
    #define Sensors_RX_STS_STOP_ERROR_SHIFT        (0x03u)
    #define Sensors_RX_STS_OVERRUN_SHIFT           (0x04u)
    #define Sensors_RX_STS_FIFO_NOTEMPTY_SHIFT     (0x05u)
    #define Sensors_RX_STS_ADDR_MATCH_SHIFT        (0x06u)
    #define Sensors_RX_STS_SOFT_BUFF_OVER_SHIFT    (0x07u)

    #define Sensors_RX_STS_MRKSPC           (uint8)(0x01u << Sensors_RX_STS_MRKSPC_SHIFT)
    #define Sensors_RX_STS_BREAK            (uint8)(0x01u << Sensors_RX_STS_BREAK_SHIFT)
    #define Sensors_RX_STS_PAR_ERROR        (uint8)(0x01u << Sensors_RX_STS_PAR_ERROR_SHIFT)
    #define Sensors_RX_STS_STOP_ERROR       (uint8)(0x01u << Sensors_RX_STS_STOP_ERROR_SHIFT)
    #define Sensors_RX_STS_OVERRUN          (uint8)(0x01u << Sensors_RX_STS_OVERRUN_SHIFT)
    #define Sensors_RX_STS_FIFO_NOTEMPTY    (uint8)(0x01u << Sensors_RX_STS_FIFO_NOTEMPTY_SHIFT)
    #define Sensors_RX_STS_ADDR_MATCH       (uint8)(0x01u << Sensors_RX_STS_ADDR_MATCH_SHIFT)
    #define Sensors_RX_STS_SOFT_BUFF_OVER   (uint8)(0x01u << Sensors_RX_STS_SOFT_BUFF_OVER_SHIFT)
    #define Sensors_RX_HW_MASK                     (0x7Fu)
#endif /* End (Sensors_RX_ENABLED) || (Sensors_HD_ENABLED) */

/* Control Register definitions */
#define Sensors_CTRL_HD_SEND_SHIFT                 (0x00u) /* 1 enable TX part in Half Duplex mode */
#define Sensors_CTRL_HD_SEND_BREAK_SHIFT           (0x01u) /* 1 send BREAK signal in Half Duplez mode */
#define Sensors_CTRL_MARK_SHIFT                    (0x02u) /* 1 sets mark, 0 sets space */
#define Sensors_CTRL_PARITY_TYPE0_SHIFT            (0x03u) /* Defines the type of parity implemented */
#define Sensors_CTRL_PARITY_TYPE1_SHIFT            (0x04u) /* Defines the type of parity implemented */
#define Sensors_CTRL_RXADDR_MODE0_SHIFT            (0x05u)
#define Sensors_CTRL_RXADDR_MODE1_SHIFT            (0x06u)
#define Sensors_CTRL_RXADDR_MODE2_SHIFT            (0x07u)

#define Sensors_CTRL_HD_SEND               (uint8)(0x01u << Sensors_CTRL_HD_SEND_SHIFT)
#define Sensors_CTRL_HD_SEND_BREAK         (uint8)(0x01u << Sensors_CTRL_HD_SEND_BREAK_SHIFT)
#define Sensors_CTRL_MARK                  (uint8)(0x01u << Sensors_CTRL_MARK_SHIFT)
#define Sensors_CTRL_PARITY_TYPE_MASK      (uint8)(0x03u << Sensors_CTRL_PARITY_TYPE0_SHIFT)
#define Sensors_CTRL_RXADDR_MODE_MASK      (uint8)(0x07u << Sensors_CTRL_RXADDR_MODE0_SHIFT)

/* StatusI Register Interrupt Enable Control Bits. As defined by the Register map for the AUX Control Register */
#define Sensors_INT_ENABLE                         (0x10u)

/* Bit Counter (7-bit) Control Register Bit Definitions. As defined by the Register map for the AUX Control Register */
#define Sensors_CNTR_ENABLE                        (0x20u)

/*   Constants for SendBreak() "retMode" parameter  */
#define Sensors_SEND_BREAK                         (0x00u)
#define Sensors_WAIT_FOR_COMPLETE_REINIT           (0x01u)
#define Sensors_REINIT                             (0x02u)
#define Sensors_SEND_WAIT_REINIT                   (0x03u)

#define Sensors_OVER_SAMPLE_8                      (8u)
#define Sensors_OVER_SAMPLE_16                     (16u)

#define Sensors_BIT_CENTER                         (Sensors_OVER_SAMPLE_COUNT - 2u)

#define Sensors_FIFO_LENGTH                        (4u)
#define Sensors_NUMBER_OF_START_BIT                (1u)
#define Sensors_MAX_BYTE_VALUE                     (0xFFu)

/* 8X always for count7 implementation */
#define Sensors_TXBITCTR_BREAKBITS8X   ((Sensors_BREAK_BITS_TX * Sensors_OVER_SAMPLE_8) - 1u)
/* 8X or 16X for DP implementation */
#define Sensors_TXBITCTR_BREAKBITS ((Sensors_BREAK_BITS_TX * Sensors_OVER_SAMPLE_COUNT) - 1u)

#define Sensors_HALF_BIT_COUNT   \
                            (((Sensors_OVER_SAMPLE_COUNT / 2u) + (Sensors_USE23POLLING * 1u)) - 2u)
#if (Sensors_OVER_SAMPLE_COUNT == Sensors_OVER_SAMPLE_8)
    #define Sensors_HD_TXBITCTR_INIT   (((Sensors_BREAK_BITS_TX + \
                            Sensors_NUMBER_OF_START_BIT) * Sensors_OVER_SAMPLE_COUNT) - 1u)

    /* This parameter is increased on the 2 in 2 out of 3 mode to sample voting in the middle */
    #define Sensors_RXBITCTR_INIT  ((((Sensors_BREAK_BITS_RX + Sensors_NUMBER_OF_START_BIT) \
                            * Sensors_OVER_SAMPLE_COUNT) + Sensors_HALF_BIT_COUNT) - 1u)

#else /* Sensors_OVER_SAMPLE_COUNT == Sensors_OVER_SAMPLE_16 */
    #define Sensors_HD_TXBITCTR_INIT   ((8u * Sensors_OVER_SAMPLE_COUNT) - 1u)
    /* 7bit counter need one more bit for OverSampleCount = 16 */
    #define Sensors_RXBITCTR_INIT      (((7u * Sensors_OVER_SAMPLE_COUNT) - 1u) + \
                                                      Sensors_HALF_BIT_COUNT)
#endif /* End Sensors_OVER_SAMPLE_COUNT */

#define Sensors_HD_RXBITCTR_INIT                   Sensors_RXBITCTR_INIT


/***************************************
* Global variables external identifier
***************************************/

extern uint8 Sensors_initVar;
#if (Sensors_TX_INTERRUPT_ENABLED && Sensors_TX_ENABLED)
    extern volatile uint8 Sensors_txBuffer[Sensors_TX_BUFFER_SIZE];
    extern volatile uint8 Sensors_txBufferRead;
    extern uint8 Sensors_txBufferWrite;
#endif /* (Sensors_TX_INTERRUPT_ENABLED && Sensors_TX_ENABLED) */
#if (Sensors_RX_INTERRUPT_ENABLED && (Sensors_RX_ENABLED || Sensors_HD_ENABLED))
    extern uint8 Sensors_errorStatus;
    extern volatile uint8 Sensors_rxBuffer[Sensors_RX_BUFFER_SIZE];
    extern volatile uint8 Sensors_rxBufferRead;
    extern volatile uint8 Sensors_rxBufferWrite;
    extern volatile uint8 Sensors_rxBufferLoopDetect;
    extern volatile uint8 Sensors_rxBufferOverflow;
    #if (Sensors_RXHW_ADDRESS_ENABLED)
        extern volatile uint8 Sensors_rxAddressMode;
        extern volatile uint8 Sensors_rxAddressDetected;
    #endif /* (Sensors_RXHW_ADDRESS_ENABLED) */
#endif /* (Sensors_RX_INTERRUPT_ENABLED && (Sensors_RX_ENABLED || Sensors_HD_ENABLED)) */


/***************************************
* Enumerated Types and Parameters
***************************************/

#define Sensors__B_UART__AM_SW_BYTE_BYTE 1
#define Sensors__B_UART__AM_SW_DETECT_TO_BUFFER 2
#define Sensors__B_UART__AM_HW_BYTE_BY_BYTE 3
#define Sensors__B_UART__AM_HW_DETECT_TO_BUFFER 4
#define Sensors__B_UART__AM_NONE 0

#define Sensors__B_UART__NONE_REVB 0
#define Sensors__B_UART__EVEN_REVB 1
#define Sensors__B_UART__ODD_REVB 2
#define Sensors__B_UART__MARK_SPACE_REVB 3



/***************************************
*    Initial Parameter Constants
***************************************/

/* UART shifts max 8 bits, Mark/Space functionality working if 9 selected */
#define Sensors_NUMBER_OF_DATA_BITS    ((8u > 8u) ? 8u : 8u)
#define Sensors_NUMBER_OF_STOP_BITS    (1u)

#if (Sensors_RXHW_ADDRESS_ENABLED)
    #define Sensors_RX_ADDRESS_MODE    (0u)
    #define Sensors_RX_HW_ADDRESS1     (0u)
    #define Sensors_RX_HW_ADDRESS2     (0u)
#endif /* (Sensors_RXHW_ADDRESS_ENABLED) */

#define Sensors_INIT_RX_INTERRUPTS_MASK \
                                  (uint8)((1 << Sensors_RX_STS_FIFO_NOTEMPTY_SHIFT) \
                                        | (0 << Sensors_RX_STS_MRKSPC_SHIFT) \
                                        | (0 << Sensors_RX_STS_ADDR_MATCH_SHIFT) \
                                        | (0 << Sensors_RX_STS_PAR_ERROR_SHIFT) \
                                        | (0 << Sensors_RX_STS_STOP_ERROR_SHIFT) \
                                        | (0 << Sensors_RX_STS_BREAK_SHIFT) \
                                        | (0 << Sensors_RX_STS_OVERRUN_SHIFT))

#define Sensors_INIT_TX_INTERRUPTS_MASK \
                                  (uint8)((0 << Sensors_TX_STS_COMPLETE_SHIFT) \
                                        | (0 << Sensors_TX_STS_FIFO_EMPTY_SHIFT) \
                                        | (0 << Sensors_TX_STS_FIFO_FULL_SHIFT) \
                                        | (0 << Sensors_TX_STS_FIFO_NOT_FULL_SHIFT))


/***************************************
*              Registers
***************************************/

#ifdef Sensors_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define Sensors_CONTROL_REG \
                            (* (reg8 *) Sensors_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
    #define Sensors_CONTROL_PTR \
                            (  (reg8 *) Sensors_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
#endif /* End Sensors_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(Sensors_TX_ENABLED)
    #define Sensors_TXDATA_REG          (* (reg8 *) Sensors_BUART_sTX_TxShifter_u0__F0_REG)
    #define Sensors_TXDATA_PTR          (  (reg8 *) Sensors_BUART_sTX_TxShifter_u0__F0_REG)
    #define Sensors_TXDATA_AUX_CTL_REG  (* (reg8 *) Sensors_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define Sensors_TXDATA_AUX_CTL_PTR  (  (reg8 *) Sensors_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define Sensors_TXSTATUS_REG        (* (reg8 *) Sensors_BUART_sTX_TxSts__STATUS_REG)
    #define Sensors_TXSTATUS_PTR        (  (reg8 *) Sensors_BUART_sTX_TxSts__STATUS_REG)
    #define Sensors_TXSTATUS_MASK_REG   (* (reg8 *) Sensors_BUART_sTX_TxSts__MASK_REG)
    #define Sensors_TXSTATUS_MASK_PTR   (  (reg8 *) Sensors_BUART_sTX_TxSts__MASK_REG)
    #define Sensors_TXSTATUS_ACTL_REG   (* (reg8 *) Sensors_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)
    #define Sensors_TXSTATUS_ACTL_PTR   (  (reg8 *) Sensors_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)

    /* DP clock */
    #if(Sensors_TXCLKGEN_DP)
        #define Sensors_TXBITCLKGEN_CTR_REG        \
                                        (* (reg8 *) Sensors_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define Sensors_TXBITCLKGEN_CTR_PTR        \
                                        (  (reg8 *) Sensors_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define Sensors_TXBITCLKTX_COMPLETE_REG    \
                                        (* (reg8 *) Sensors_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
        #define Sensors_TXBITCLKTX_COMPLETE_PTR    \
                                        (  (reg8 *) Sensors_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
    #else     /* Count7 clock*/
        #define Sensors_TXBITCTR_PERIOD_REG    \
                                        (* (reg8 *) Sensors_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define Sensors_TXBITCTR_PERIOD_PTR    \
                                        (  (reg8 *) Sensors_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define Sensors_TXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) Sensors_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define Sensors_TXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) Sensors_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define Sensors_TXBITCTR_COUNTER_REG   \
                                        (* (reg8 *) Sensors_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
        #define Sensors_TXBITCTR_COUNTER_PTR   \
                                        (  (reg8 *) Sensors_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
    #endif /* Sensors_TXCLKGEN_DP */

#endif /* End Sensors_TX_ENABLED */

#if(Sensors_HD_ENABLED)

    #define Sensors_TXDATA_REG             (* (reg8 *) Sensors_BUART_sRX_RxShifter_u0__F1_REG )
    #define Sensors_TXDATA_PTR             (  (reg8 *) Sensors_BUART_sRX_RxShifter_u0__F1_REG )
    #define Sensors_TXDATA_AUX_CTL_REG     (* (reg8 *) Sensors_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)
    #define Sensors_TXDATA_AUX_CTL_PTR     (  (reg8 *) Sensors_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define Sensors_TXSTATUS_REG           (* (reg8 *) Sensors_BUART_sRX_RxSts__STATUS_REG )
    #define Sensors_TXSTATUS_PTR           (  (reg8 *) Sensors_BUART_sRX_RxSts__STATUS_REG )
    #define Sensors_TXSTATUS_MASK_REG      (* (reg8 *) Sensors_BUART_sRX_RxSts__MASK_REG )
    #define Sensors_TXSTATUS_MASK_PTR      (  (reg8 *) Sensors_BUART_sRX_RxSts__MASK_REG )
    #define Sensors_TXSTATUS_ACTL_REG      (* (reg8 *) Sensors_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define Sensors_TXSTATUS_ACTL_PTR      (  (reg8 *) Sensors_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End Sensors_HD_ENABLED */

#if( (Sensors_RX_ENABLED) || (Sensors_HD_ENABLED) )
    #define Sensors_RXDATA_REG             (* (reg8 *) Sensors_BUART_sRX_RxShifter_u0__F0_REG )
    #define Sensors_RXDATA_PTR             (  (reg8 *) Sensors_BUART_sRX_RxShifter_u0__F0_REG )
    #define Sensors_RXADDRESS1_REG         (* (reg8 *) Sensors_BUART_sRX_RxShifter_u0__D0_REG )
    #define Sensors_RXADDRESS1_PTR         (  (reg8 *) Sensors_BUART_sRX_RxShifter_u0__D0_REG )
    #define Sensors_RXADDRESS2_REG         (* (reg8 *) Sensors_BUART_sRX_RxShifter_u0__D1_REG )
    #define Sensors_RXADDRESS2_PTR         (  (reg8 *) Sensors_BUART_sRX_RxShifter_u0__D1_REG )
    #define Sensors_RXDATA_AUX_CTL_REG     (* (reg8 *) Sensors_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define Sensors_RXBITCTR_PERIOD_REG    (* (reg8 *) Sensors_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define Sensors_RXBITCTR_PERIOD_PTR    (  (reg8 *) Sensors_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define Sensors_RXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) Sensors_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define Sensors_RXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) Sensors_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define Sensors_RXBITCTR_COUNTER_REG   (* (reg8 *) Sensors_BUART_sRX_RxBitCounter__COUNT_REG )
    #define Sensors_RXBITCTR_COUNTER_PTR   (  (reg8 *) Sensors_BUART_sRX_RxBitCounter__COUNT_REG )

    #define Sensors_RXSTATUS_REG           (* (reg8 *) Sensors_BUART_sRX_RxSts__STATUS_REG )
    #define Sensors_RXSTATUS_PTR           (  (reg8 *) Sensors_BUART_sRX_RxSts__STATUS_REG )
    #define Sensors_RXSTATUS_MASK_REG      (* (reg8 *) Sensors_BUART_sRX_RxSts__MASK_REG )
    #define Sensors_RXSTATUS_MASK_PTR      (  (reg8 *) Sensors_BUART_sRX_RxSts__MASK_REG )
    #define Sensors_RXSTATUS_ACTL_REG      (* (reg8 *) Sensors_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define Sensors_RXSTATUS_ACTL_PTR      (  (reg8 *) Sensors_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End  (Sensors_RX_ENABLED) || (Sensors_HD_ENABLED) */

#if(Sensors_INTERNAL_CLOCK_USED)
    /* Register to enable or disable the digital clocks */
    #define Sensors_INTCLOCK_CLKEN_REG     (* (reg8 *) Sensors_IntClock__PM_ACT_CFG)
    #define Sensors_INTCLOCK_CLKEN_PTR     (  (reg8 *) Sensors_IntClock__PM_ACT_CFG)

    /* Clock mask for this clock. */
    #define Sensors_INTCLOCK_CLKEN_MASK    Sensors_IntClock__PM_ACT_MSK
#endif /* End Sensors_INTERNAL_CLOCK_USED */


/***************************************
*       Register Constants
***************************************/

#if(Sensors_TX_ENABLED)
    #define Sensors_TX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End Sensors_TX_ENABLED */

#if(Sensors_HD_ENABLED)
    #define Sensors_TX_FIFO_CLR            (0x02u) /* FIFO1 CLR */
#endif /* End Sensors_HD_ENABLED */

#if( (Sensors_RX_ENABLED) || (Sensors_HD_ENABLED) )
    #define Sensors_RX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End  (Sensors_RX_ENABLED) || (Sensors_HD_ENABLED) */


/***************************************
* The following code is DEPRECATED and
* should not be used in new projects.
***************************************/

/* UART v2_40 obsolete definitions */
#define Sensors_WAIT_1_MS      Sensors_BL_CHK_DELAY_MS   

#define Sensors_TXBUFFERSIZE   Sensors_TX_BUFFER_SIZE
#define Sensors_RXBUFFERSIZE   Sensors_RX_BUFFER_SIZE

#if (Sensors_RXHW_ADDRESS_ENABLED)
    #define Sensors_RXADDRESSMODE  Sensors_RX_ADDRESS_MODE
    #define Sensors_RXHWADDRESS1   Sensors_RX_HW_ADDRESS1
    #define Sensors_RXHWADDRESS2   Sensors_RX_HW_ADDRESS2
    /* Backward compatible define */
    #define Sensors_RXAddressMode  Sensors_RXADDRESSMODE
#endif /* (Sensors_RXHW_ADDRESS_ENABLED) */

/* UART v2_30 obsolete definitions */
#define Sensors_initvar                    Sensors_initVar

#define Sensors_RX_Enabled                 Sensors_RX_ENABLED
#define Sensors_TX_Enabled                 Sensors_TX_ENABLED
#define Sensors_HD_Enabled                 Sensors_HD_ENABLED
#define Sensors_RX_IntInterruptEnabled     Sensors_RX_INTERRUPT_ENABLED
#define Sensors_TX_IntInterruptEnabled     Sensors_TX_INTERRUPT_ENABLED
#define Sensors_InternalClockUsed          Sensors_INTERNAL_CLOCK_USED
#define Sensors_RXHW_Address_Enabled       Sensors_RXHW_ADDRESS_ENABLED
#define Sensors_OverSampleCount            Sensors_OVER_SAMPLE_COUNT
#define Sensors_ParityType                 Sensors_PARITY_TYPE

#if( Sensors_TX_ENABLED && (Sensors_TXBUFFERSIZE > Sensors_FIFO_LENGTH))
    #define Sensors_TXBUFFER               Sensors_txBuffer
    #define Sensors_TXBUFFERREAD           Sensors_txBufferRead
    #define Sensors_TXBUFFERWRITE          Sensors_txBufferWrite
#endif /* End Sensors_TX_ENABLED */
#if( ( Sensors_RX_ENABLED || Sensors_HD_ENABLED ) && \
     (Sensors_RXBUFFERSIZE > Sensors_FIFO_LENGTH) )
    #define Sensors_RXBUFFER               Sensors_rxBuffer
    #define Sensors_RXBUFFERREAD           Sensors_rxBufferRead
    #define Sensors_RXBUFFERWRITE          Sensors_rxBufferWrite
    #define Sensors_RXBUFFERLOOPDETECT     Sensors_rxBufferLoopDetect
    #define Sensors_RXBUFFER_OVERFLOW      Sensors_rxBufferOverflow
#endif /* End Sensors_RX_ENABLED */

#ifdef Sensors_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define Sensors_CONTROL                Sensors_CONTROL_REG
#endif /* End Sensors_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(Sensors_TX_ENABLED)
    #define Sensors_TXDATA                 Sensors_TXDATA_REG
    #define Sensors_TXSTATUS               Sensors_TXSTATUS_REG
    #define Sensors_TXSTATUS_MASK          Sensors_TXSTATUS_MASK_REG
    #define Sensors_TXSTATUS_ACTL          Sensors_TXSTATUS_ACTL_REG
    /* DP clock */
    #if(Sensors_TXCLKGEN_DP)
        #define Sensors_TXBITCLKGEN_CTR        Sensors_TXBITCLKGEN_CTR_REG
        #define Sensors_TXBITCLKTX_COMPLETE    Sensors_TXBITCLKTX_COMPLETE_REG
    #else     /* Count7 clock*/
        #define Sensors_TXBITCTR_PERIOD        Sensors_TXBITCTR_PERIOD_REG
        #define Sensors_TXBITCTR_CONTROL       Sensors_TXBITCTR_CONTROL_REG
        #define Sensors_TXBITCTR_COUNTER       Sensors_TXBITCTR_COUNTER_REG
    #endif /* Sensors_TXCLKGEN_DP */
#endif /* End Sensors_TX_ENABLED */

#if(Sensors_HD_ENABLED)
    #define Sensors_TXDATA                 Sensors_TXDATA_REG
    #define Sensors_TXSTATUS               Sensors_TXSTATUS_REG
    #define Sensors_TXSTATUS_MASK          Sensors_TXSTATUS_MASK_REG
    #define Sensors_TXSTATUS_ACTL          Sensors_TXSTATUS_ACTL_REG
#endif /* End Sensors_HD_ENABLED */

#if( (Sensors_RX_ENABLED) || (Sensors_HD_ENABLED) )
    #define Sensors_RXDATA                 Sensors_RXDATA_REG
    #define Sensors_RXADDRESS1             Sensors_RXADDRESS1_REG
    #define Sensors_RXADDRESS2             Sensors_RXADDRESS2_REG
    #define Sensors_RXBITCTR_PERIOD        Sensors_RXBITCTR_PERIOD_REG
    #define Sensors_RXBITCTR_CONTROL       Sensors_RXBITCTR_CONTROL_REG
    #define Sensors_RXBITCTR_COUNTER       Sensors_RXBITCTR_COUNTER_REG
    #define Sensors_RXSTATUS               Sensors_RXSTATUS_REG
    #define Sensors_RXSTATUS_MASK          Sensors_RXSTATUS_MASK_REG
    #define Sensors_RXSTATUS_ACTL          Sensors_RXSTATUS_ACTL_REG
#endif /* End  (Sensors_RX_ENABLED) || (Sensors_HD_ENABLED) */

#if(Sensors_INTERNAL_CLOCK_USED)
    #define Sensors_INTCLOCK_CLKEN         Sensors_INTCLOCK_CLKEN_REG
#endif /* End Sensors_INTERNAL_CLOCK_USED */

#define Sensors_WAIT_FOR_COMLETE_REINIT    Sensors_WAIT_FOR_COMPLETE_REINIT

#endif  /* CY_UART_Sensors_H */


/* [] END OF FILE */
