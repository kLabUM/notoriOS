/*******************************************************************************
* File Name: SDI12_UART.h
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


#if !defined(CY_UART_SDI12_UART_H)
#define CY_UART_SDI12_UART_H

#include "cyfitter.h"
#include "cytypes.h"
#include "CyLib.h" /* For CyEnterCriticalSection() and CyExitCriticalSection() functions */


/***************************************
* Conditional Compilation Parameters
***************************************/

#define SDI12_UART_RX_ENABLED                     (1u)
#define SDI12_UART_TX_ENABLED                     (1u)
#define SDI12_UART_HD_ENABLED                     (0u)
#define SDI12_UART_RX_INTERRUPT_ENABLED           (0u)
#define SDI12_UART_TX_INTERRUPT_ENABLED           (0u)
#define SDI12_UART_INTERNAL_CLOCK_USED            (1u)
#define SDI12_UART_RXHW_ADDRESS_ENABLED           (0u)
#define SDI12_UART_OVER_SAMPLE_COUNT              (8u)
#define SDI12_UART_PARITY_TYPE                    (1u)
#define SDI12_UART_PARITY_TYPE_SW                 (0u)
#define SDI12_UART_BREAK_DETECT                   (0u)
#define SDI12_UART_BREAK_BITS_TX                  (13u)
#define SDI12_UART_BREAK_BITS_RX                  (13u)
#define SDI12_UART_TXCLKGEN_DP                    (1u)
#define SDI12_UART_USE23POLLING                   (1u)
#define SDI12_UART_FLOW_CONTROL                   (0u)
#define SDI12_UART_CLK_FREQ                       (0u)
#define SDI12_UART_TX_BUFFER_SIZE                 (4u)
#define SDI12_UART_RX_BUFFER_SIZE                 (4u)

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component UART_v2_50 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */

#if defined(SDI12_UART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG)
    #define SDI12_UART_CONTROL_REG_REMOVED            (0u)
#else
    #define SDI12_UART_CONTROL_REG_REMOVED            (1u)
#endif /* End SDI12_UART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */


/***************************************
*      Data Structure Definition
***************************************/

/* Sleep Mode API Support */
typedef struct SDI12_UART_backupStruct_
{
    uint8 enableState;

    #if(SDI12_UART_CONTROL_REG_REMOVED == 0u)
        uint8 cr;
    #endif /* End SDI12_UART_CONTROL_REG_REMOVED */

} SDI12_UART_BACKUP_STRUCT;


/***************************************
*       Function Prototypes
***************************************/

void SDI12_UART_Start(void) ;
void SDI12_UART_Stop(void) ;
uint8 SDI12_UART_ReadControlRegister(void) ;
void SDI12_UART_WriteControlRegister(uint8 control) ;

void SDI12_UART_Init(void) ;
void SDI12_UART_Enable(void) ;
void SDI12_UART_SaveConfig(void) ;
void SDI12_UART_RestoreConfig(void) ;
void SDI12_UART_Sleep(void) ;
void SDI12_UART_Wakeup(void) ;

/* Only if RX is enabled */
#if( (SDI12_UART_RX_ENABLED) || (SDI12_UART_HD_ENABLED) )

    #if (SDI12_UART_RX_INTERRUPT_ENABLED)
        #define SDI12_UART_EnableRxInt()  CyIntEnable (SDI12_UART_RX_VECT_NUM)
        #define SDI12_UART_DisableRxInt() CyIntDisable(SDI12_UART_RX_VECT_NUM)
        CY_ISR_PROTO(SDI12_UART_RXISR);
    #endif /* SDI12_UART_RX_INTERRUPT_ENABLED */

    void SDI12_UART_SetRxAddressMode(uint8 addressMode)
                                                           ;
    void SDI12_UART_SetRxAddress1(uint8 address) ;
    void SDI12_UART_SetRxAddress2(uint8 address) ;

    void  SDI12_UART_SetRxInterruptMode(uint8 intSrc) ;
    uint8 SDI12_UART_ReadRxData(void) ;
    uint8 SDI12_UART_ReadRxStatus(void) ;
    uint8 SDI12_UART_GetChar(void) ;
    uint16 SDI12_UART_GetByte(void) ;
    uint8 SDI12_UART_GetRxBufferSize(void)
                                                            ;
    void SDI12_UART_ClearRxBuffer(void) ;

    /* Obsolete functions, defines for backward compatible */
    #define SDI12_UART_GetRxInterruptSource   SDI12_UART_ReadRxStatus

#endif /* End (SDI12_UART_RX_ENABLED) || (SDI12_UART_HD_ENABLED) */

/* Only if TX is enabled */
#if(SDI12_UART_TX_ENABLED || SDI12_UART_HD_ENABLED)

    #if(SDI12_UART_TX_INTERRUPT_ENABLED)
        #define SDI12_UART_EnableTxInt()  CyIntEnable (SDI12_UART_TX_VECT_NUM)
        #define SDI12_UART_DisableTxInt() CyIntDisable(SDI12_UART_TX_VECT_NUM)
        #define SDI12_UART_SetPendingTxInt() CyIntSetPending(SDI12_UART_TX_VECT_NUM)
        #define SDI12_UART_ClearPendingTxInt() CyIntClearPending(SDI12_UART_TX_VECT_NUM)
        CY_ISR_PROTO(SDI12_UART_TXISR);
    #endif /* SDI12_UART_TX_INTERRUPT_ENABLED */

    void SDI12_UART_SetTxInterruptMode(uint8 intSrc) ;
    void SDI12_UART_WriteTxData(uint8 txDataByte) ;
    uint8 SDI12_UART_ReadTxStatus(void) ;
    void SDI12_UART_PutChar(uint8 txDataByte) ;
    void SDI12_UART_PutString(const char8 string[]) ;
    void SDI12_UART_PutArray(const uint8 string[], uint8 byteCount)
                                                            ;
    void SDI12_UART_PutCRLF(uint8 txDataByte) ;
    void SDI12_UART_ClearTxBuffer(void) ;
    void SDI12_UART_SetTxAddressMode(uint8 addressMode) ;
    void SDI12_UART_SendBreak(uint8 retMode) ;
    uint8 SDI12_UART_GetTxBufferSize(void)
                                                            ;
    /* Obsolete functions, defines for backward compatible */
    #define SDI12_UART_PutStringConst         SDI12_UART_PutString
    #define SDI12_UART_PutArrayConst          SDI12_UART_PutArray
    #define SDI12_UART_GetTxInterruptSource   SDI12_UART_ReadTxStatus

#endif /* End SDI12_UART_TX_ENABLED || SDI12_UART_HD_ENABLED */

#if(SDI12_UART_HD_ENABLED)
    void SDI12_UART_LoadRxConfig(void) ;
    void SDI12_UART_LoadTxConfig(void) ;
#endif /* End SDI12_UART_HD_ENABLED */


/* Communication bootloader APIs */
#if defined(CYDEV_BOOTLOADER_IO_COMP) && ((CYDEV_BOOTLOADER_IO_COMP == CyBtldr_SDI12_UART) || \
                                          (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Custom_Interface))
    /* Physical layer functions */
    void    SDI12_UART_CyBtldrCommStart(void) CYSMALL ;
    void    SDI12_UART_CyBtldrCommStop(void) CYSMALL ;
    void    SDI12_UART_CyBtldrCommReset(void) CYSMALL ;
    cystatus SDI12_UART_CyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;
    cystatus SDI12_UART_CyBtldrCommRead(uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;

    #if (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_SDI12_UART)
        #define CyBtldrCommStart    SDI12_UART_CyBtldrCommStart
        #define CyBtldrCommStop     SDI12_UART_CyBtldrCommStop
        #define CyBtldrCommReset    SDI12_UART_CyBtldrCommReset
        #define CyBtldrCommWrite    SDI12_UART_CyBtldrCommWrite
        #define CyBtldrCommRead     SDI12_UART_CyBtldrCommRead
    #endif  /* (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_SDI12_UART) */

    /* Byte to Byte time out for detecting end of block data from host */
    #define SDI12_UART_BYTE2BYTE_TIME_OUT (25u)
    #define SDI12_UART_PACKET_EOP         (0x17u) /* End of packet defined by bootloader */
    #define SDI12_UART_WAIT_EOP_DELAY     (5u)    /* Additional 5ms to wait for End of packet */
    #define SDI12_UART_BL_CHK_DELAY_MS    (1u)    /* Time Out quantity equal 1mS */

#endif /* CYDEV_BOOTLOADER_IO_COMP */


/***************************************
*          API Constants
***************************************/
/* Parameters for SetTxAddressMode API*/
#define SDI12_UART_SET_SPACE      (0x00u)
#define SDI12_UART_SET_MARK       (0x01u)

/* Status Register definitions */
#if( (SDI12_UART_TX_ENABLED) || (SDI12_UART_HD_ENABLED) )
    #if(SDI12_UART_TX_INTERRUPT_ENABLED)
        #define SDI12_UART_TX_VECT_NUM            (uint8)SDI12_UART_TXInternalInterrupt__INTC_NUMBER
        #define SDI12_UART_TX_PRIOR_NUM           (uint8)SDI12_UART_TXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* SDI12_UART_TX_INTERRUPT_ENABLED */

    #define SDI12_UART_TX_STS_COMPLETE_SHIFT          (0x00u)
    #define SDI12_UART_TX_STS_FIFO_EMPTY_SHIFT        (0x01u)
    #define SDI12_UART_TX_STS_FIFO_NOT_FULL_SHIFT     (0x03u)
    #if(SDI12_UART_TX_ENABLED)
        #define SDI12_UART_TX_STS_FIFO_FULL_SHIFT     (0x02u)
    #else /* (SDI12_UART_HD_ENABLED) */
        #define SDI12_UART_TX_STS_FIFO_FULL_SHIFT     (0x05u)  /* Needs MD=0 */
    #endif /* (SDI12_UART_TX_ENABLED) */

    #define SDI12_UART_TX_STS_COMPLETE            (uint8)(0x01u << SDI12_UART_TX_STS_COMPLETE_SHIFT)
    #define SDI12_UART_TX_STS_FIFO_EMPTY          (uint8)(0x01u << SDI12_UART_TX_STS_FIFO_EMPTY_SHIFT)
    #define SDI12_UART_TX_STS_FIFO_FULL           (uint8)(0x01u << SDI12_UART_TX_STS_FIFO_FULL_SHIFT)
    #define SDI12_UART_TX_STS_FIFO_NOT_FULL       (uint8)(0x01u << SDI12_UART_TX_STS_FIFO_NOT_FULL_SHIFT)
#endif /* End (SDI12_UART_TX_ENABLED) || (SDI12_UART_HD_ENABLED)*/

#if( (SDI12_UART_RX_ENABLED) || (SDI12_UART_HD_ENABLED) )
    #if(SDI12_UART_RX_INTERRUPT_ENABLED)
        #define SDI12_UART_RX_VECT_NUM            (uint8)SDI12_UART_RXInternalInterrupt__INTC_NUMBER
        #define SDI12_UART_RX_PRIOR_NUM           (uint8)SDI12_UART_RXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* SDI12_UART_RX_INTERRUPT_ENABLED */
    #define SDI12_UART_RX_STS_MRKSPC_SHIFT            (0x00u)
    #define SDI12_UART_RX_STS_BREAK_SHIFT             (0x01u)
    #define SDI12_UART_RX_STS_PAR_ERROR_SHIFT         (0x02u)
    #define SDI12_UART_RX_STS_STOP_ERROR_SHIFT        (0x03u)
    #define SDI12_UART_RX_STS_OVERRUN_SHIFT           (0x04u)
    #define SDI12_UART_RX_STS_FIFO_NOTEMPTY_SHIFT     (0x05u)
    #define SDI12_UART_RX_STS_ADDR_MATCH_SHIFT        (0x06u)
    #define SDI12_UART_RX_STS_SOFT_BUFF_OVER_SHIFT    (0x07u)

    #define SDI12_UART_RX_STS_MRKSPC           (uint8)(0x01u << SDI12_UART_RX_STS_MRKSPC_SHIFT)
    #define SDI12_UART_RX_STS_BREAK            (uint8)(0x01u << SDI12_UART_RX_STS_BREAK_SHIFT)
    #define SDI12_UART_RX_STS_PAR_ERROR        (uint8)(0x01u << SDI12_UART_RX_STS_PAR_ERROR_SHIFT)
    #define SDI12_UART_RX_STS_STOP_ERROR       (uint8)(0x01u << SDI12_UART_RX_STS_STOP_ERROR_SHIFT)
    #define SDI12_UART_RX_STS_OVERRUN          (uint8)(0x01u << SDI12_UART_RX_STS_OVERRUN_SHIFT)
    #define SDI12_UART_RX_STS_FIFO_NOTEMPTY    (uint8)(0x01u << SDI12_UART_RX_STS_FIFO_NOTEMPTY_SHIFT)
    #define SDI12_UART_RX_STS_ADDR_MATCH       (uint8)(0x01u << SDI12_UART_RX_STS_ADDR_MATCH_SHIFT)
    #define SDI12_UART_RX_STS_SOFT_BUFF_OVER   (uint8)(0x01u << SDI12_UART_RX_STS_SOFT_BUFF_OVER_SHIFT)
    #define SDI12_UART_RX_HW_MASK                     (0x7Fu)
#endif /* End (SDI12_UART_RX_ENABLED) || (SDI12_UART_HD_ENABLED) */

/* Control Register definitions */
#define SDI12_UART_CTRL_HD_SEND_SHIFT                 (0x00u) /* 1 enable TX part in Half Duplex mode */
#define SDI12_UART_CTRL_HD_SEND_BREAK_SHIFT           (0x01u) /* 1 send BREAK signal in Half Duplez mode */
#define SDI12_UART_CTRL_MARK_SHIFT                    (0x02u) /* 1 sets mark, 0 sets space */
#define SDI12_UART_CTRL_PARITY_TYPE0_SHIFT            (0x03u) /* Defines the type of parity implemented */
#define SDI12_UART_CTRL_PARITY_TYPE1_SHIFT            (0x04u) /* Defines the type of parity implemented */
#define SDI12_UART_CTRL_RXADDR_MODE0_SHIFT            (0x05u)
#define SDI12_UART_CTRL_RXADDR_MODE1_SHIFT            (0x06u)
#define SDI12_UART_CTRL_RXADDR_MODE2_SHIFT            (0x07u)

#define SDI12_UART_CTRL_HD_SEND               (uint8)(0x01u << SDI12_UART_CTRL_HD_SEND_SHIFT)
#define SDI12_UART_CTRL_HD_SEND_BREAK         (uint8)(0x01u << SDI12_UART_CTRL_HD_SEND_BREAK_SHIFT)
#define SDI12_UART_CTRL_MARK                  (uint8)(0x01u << SDI12_UART_CTRL_MARK_SHIFT)
#define SDI12_UART_CTRL_PARITY_TYPE_MASK      (uint8)(0x03u << SDI12_UART_CTRL_PARITY_TYPE0_SHIFT)
#define SDI12_UART_CTRL_RXADDR_MODE_MASK      (uint8)(0x07u << SDI12_UART_CTRL_RXADDR_MODE0_SHIFT)

/* StatusI Register Interrupt Enable Control Bits. As defined by the Register map for the AUX Control Register */
#define SDI12_UART_INT_ENABLE                         (0x10u)

/* Bit Counter (7-bit) Control Register Bit Definitions. As defined by the Register map for the AUX Control Register */
#define SDI12_UART_CNTR_ENABLE                        (0x20u)

/*   Constants for SendBreak() "retMode" parameter  */
#define SDI12_UART_SEND_BREAK                         (0x00u)
#define SDI12_UART_WAIT_FOR_COMPLETE_REINIT           (0x01u)
#define SDI12_UART_REINIT                             (0x02u)
#define SDI12_UART_SEND_WAIT_REINIT                   (0x03u)

#define SDI12_UART_OVER_SAMPLE_8                      (8u)
#define SDI12_UART_OVER_SAMPLE_16                     (16u)

#define SDI12_UART_BIT_CENTER                         (SDI12_UART_OVER_SAMPLE_COUNT - 2u)

#define SDI12_UART_FIFO_LENGTH                        (4u)
#define SDI12_UART_NUMBER_OF_START_BIT                (1u)
#define SDI12_UART_MAX_BYTE_VALUE                     (0xFFu)

/* 8X always for count7 implementation */
#define SDI12_UART_TXBITCTR_BREAKBITS8X   ((SDI12_UART_BREAK_BITS_TX * SDI12_UART_OVER_SAMPLE_8) - 1u)
/* 8X or 16X for DP implementation */
#define SDI12_UART_TXBITCTR_BREAKBITS ((SDI12_UART_BREAK_BITS_TX * SDI12_UART_OVER_SAMPLE_COUNT) - 1u)

#define SDI12_UART_HALF_BIT_COUNT   \
                            (((SDI12_UART_OVER_SAMPLE_COUNT / 2u) + (SDI12_UART_USE23POLLING * 1u)) - 2u)
#if (SDI12_UART_OVER_SAMPLE_COUNT == SDI12_UART_OVER_SAMPLE_8)
    #define SDI12_UART_HD_TXBITCTR_INIT   (((SDI12_UART_BREAK_BITS_TX + \
                            SDI12_UART_NUMBER_OF_START_BIT) * SDI12_UART_OVER_SAMPLE_COUNT) - 1u)

    /* This parameter is increased on the 2 in 2 out of 3 mode to sample voting in the middle */
    #define SDI12_UART_RXBITCTR_INIT  ((((SDI12_UART_BREAK_BITS_RX + SDI12_UART_NUMBER_OF_START_BIT) \
                            * SDI12_UART_OVER_SAMPLE_COUNT) + SDI12_UART_HALF_BIT_COUNT) - 1u)

#else /* SDI12_UART_OVER_SAMPLE_COUNT == SDI12_UART_OVER_SAMPLE_16 */
    #define SDI12_UART_HD_TXBITCTR_INIT   ((8u * SDI12_UART_OVER_SAMPLE_COUNT) - 1u)
    /* 7bit counter need one more bit for OverSampleCount = 16 */
    #define SDI12_UART_RXBITCTR_INIT      (((7u * SDI12_UART_OVER_SAMPLE_COUNT) - 1u) + \
                                                      SDI12_UART_HALF_BIT_COUNT)
#endif /* End SDI12_UART_OVER_SAMPLE_COUNT */

#define SDI12_UART_HD_RXBITCTR_INIT                   SDI12_UART_RXBITCTR_INIT


/***************************************
* Global variables external identifier
***************************************/

extern uint8 SDI12_UART_initVar;
#if (SDI12_UART_TX_INTERRUPT_ENABLED && SDI12_UART_TX_ENABLED)
    extern volatile uint8 SDI12_UART_txBuffer[SDI12_UART_TX_BUFFER_SIZE];
    extern volatile uint8 SDI12_UART_txBufferRead;
    extern uint8 SDI12_UART_txBufferWrite;
#endif /* (SDI12_UART_TX_INTERRUPT_ENABLED && SDI12_UART_TX_ENABLED) */
#if (SDI12_UART_RX_INTERRUPT_ENABLED && (SDI12_UART_RX_ENABLED || SDI12_UART_HD_ENABLED))
    extern uint8 SDI12_UART_errorStatus;
    extern volatile uint8 SDI12_UART_rxBuffer[SDI12_UART_RX_BUFFER_SIZE];
    extern volatile uint8 SDI12_UART_rxBufferRead;
    extern volatile uint8 SDI12_UART_rxBufferWrite;
    extern volatile uint8 SDI12_UART_rxBufferLoopDetect;
    extern volatile uint8 SDI12_UART_rxBufferOverflow;
    #if (SDI12_UART_RXHW_ADDRESS_ENABLED)
        extern volatile uint8 SDI12_UART_rxAddressMode;
        extern volatile uint8 SDI12_UART_rxAddressDetected;
    #endif /* (SDI12_UART_RXHW_ADDRESS_ENABLED) */
#endif /* (SDI12_UART_RX_INTERRUPT_ENABLED && (SDI12_UART_RX_ENABLED || SDI12_UART_HD_ENABLED)) */


/***************************************
* Enumerated Types and Parameters
***************************************/

#define SDI12_UART__B_UART__AM_SW_BYTE_BYTE 1
#define SDI12_UART__B_UART__AM_SW_DETECT_TO_BUFFER 2
#define SDI12_UART__B_UART__AM_HW_BYTE_BY_BYTE 3
#define SDI12_UART__B_UART__AM_HW_DETECT_TO_BUFFER 4
#define SDI12_UART__B_UART__AM_NONE 0

#define SDI12_UART__B_UART__NONE_REVB 0
#define SDI12_UART__B_UART__EVEN_REVB 1
#define SDI12_UART__B_UART__ODD_REVB 2
#define SDI12_UART__B_UART__MARK_SPACE_REVB 3



/***************************************
*    Initial Parameter Constants
***************************************/

/* UART shifts max 8 bits, Mark/Space functionality working if 9 selected */
#define SDI12_UART_NUMBER_OF_DATA_BITS    ((7u > 8u) ? 8u : 7u)
#define SDI12_UART_NUMBER_OF_STOP_BITS    (1u)

#if (SDI12_UART_RXHW_ADDRESS_ENABLED)
    #define SDI12_UART_RX_ADDRESS_MODE    (0u)
    #define SDI12_UART_RX_HW_ADDRESS1     (0u)
    #define SDI12_UART_RX_HW_ADDRESS2     (0u)
#endif /* (SDI12_UART_RXHW_ADDRESS_ENABLED) */

#define SDI12_UART_INIT_RX_INTERRUPTS_MASK \
                                  (uint8)((1 << SDI12_UART_RX_STS_FIFO_NOTEMPTY_SHIFT) \
                                        | (0 << SDI12_UART_RX_STS_MRKSPC_SHIFT) \
                                        | (0 << SDI12_UART_RX_STS_ADDR_MATCH_SHIFT) \
                                        | (0 << SDI12_UART_RX_STS_PAR_ERROR_SHIFT) \
                                        | (0 << SDI12_UART_RX_STS_STOP_ERROR_SHIFT) \
                                        | (0 << SDI12_UART_RX_STS_BREAK_SHIFT) \
                                        | (0 << SDI12_UART_RX_STS_OVERRUN_SHIFT))

#define SDI12_UART_INIT_TX_INTERRUPTS_MASK \
                                  (uint8)((0 << SDI12_UART_TX_STS_COMPLETE_SHIFT) \
                                        | (0 << SDI12_UART_TX_STS_FIFO_EMPTY_SHIFT) \
                                        | (0 << SDI12_UART_TX_STS_FIFO_FULL_SHIFT) \
                                        | (0 << SDI12_UART_TX_STS_FIFO_NOT_FULL_SHIFT))


/***************************************
*              Registers
***************************************/

#ifdef SDI12_UART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define SDI12_UART_CONTROL_REG \
                            (* (reg8 *) SDI12_UART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
    #define SDI12_UART_CONTROL_PTR \
                            (  (reg8 *) SDI12_UART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
#endif /* End SDI12_UART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(SDI12_UART_TX_ENABLED)
    #define SDI12_UART_TXDATA_REG          (* (reg8 *) SDI12_UART_BUART_sTX_TxShifter_u0__F0_REG)
    #define SDI12_UART_TXDATA_PTR          (  (reg8 *) SDI12_UART_BUART_sTX_TxShifter_u0__F0_REG)
    #define SDI12_UART_TXDATA_AUX_CTL_REG  (* (reg8 *) SDI12_UART_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define SDI12_UART_TXDATA_AUX_CTL_PTR  (  (reg8 *) SDI12_UART_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define SDI12_UART_TXSTATUS_REG        (* (reg8 *) SDI12_UART_BUART_sTX_TxSts__STATUS_REG)
    #define SDI12_UART_TXSTATUS_PTR        (  (reg8 *) SDI12_UART_BUART_sTX_TxSts__STATUS_REG)
    #define SDI12_UART_TXSTATUS_MASK_REG   (* (reg8 *) SDI12_UART_BUART_sTX_TxSts__MASK_REG)
    #define SDI12_UART_TXSTATUS_MASK_PTR   (  (reg8 *) SDI12_UART_BUART_sTX_TxSts__MASK_REG)
    #define SDI12_UART_TXSTATUS_ACTL_REG   (* (reg8 *) SDI12_UART_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)
    #define SDI12_UART_TXSTATUS_ACTL_PTR   (  (reg8 *) SDI12_UART_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)

    /* DP clock */
    #if(SDI12_UART_TXCLKGEN_DP)
        #define SDI12_UART_TXBITCLKGEN_CTR_REG        \
                                        (* (reg8 *) SDI12_UART_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define SDI12_UART_TXBITCLKGEN_CTR_PTR        \
                                        (  (reg8 *) SDI12_UART_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define SDI12_UART_TXBITCLKTX_COMPLETE_REG    \
                                        (* (reg8 *) SDI12_UART_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
        #define SDI12_UART_TXBITCLKTX_COMPLETE_PTR    \
                                        (  (reg8 *) SDI12_UART_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
    #else     /* Count7 clock*/
        #define SDI12_UART_TXBITCTR_PERIOD_REG    \
                                        (* (reg8 *) SDI12_UART_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define SDI12_UART_TXBITCTR_PERIOD_PTR    \
                                        (  (reg8 *) SDI12_UART_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define SDI12_UART_TXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) SDI12_UART_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define SDI12_UART_TXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) SDI12_UART_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define SDI12_UART_TXBITCTR_COUNTER_REG   \
                                        (* (reg8 *) SDI12_UART_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
        #define SDI12_UART_TXBITCTR_COUNTER_PTR   \
                                        (  (reg8 *) SDI12_UART_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
    #endif /* SDI12_UART_TXCLKGEN_DP */

#endif /* End SDI12_UART_TX_ENABLED */

#if(SDI12_UART_HD_ENABLED)

    #define SDI12_UART_TXDATA_REG             (* (reg8 *) SDI12_UART_BUART_sRX_RxShifter_u0__F1_REG )
    #define SDI12_UART_TXDATA_PTR             (  (reg8 *) SDI12_UART_BUART_sRX_RxShifter_u0__F1_REG )
    #define SDI12_UART_TXDATA_AUX_CTL_REG     (* (reg8 *) SDI12_UART_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)
    #define SDI12_UART_TXDATA_AUX_CTL_PTR     (  (reg8 *) SDI12_UART_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define SDI12_UART_TXSTATUS_REG           (* (reg8 *) SDI12_UART_BUART_sRX_RxSts__STATUS_REG )
    #define SDI12_UART_TXSTATUS_PTR           (  (reg8 *) SDI12_UART_BUART_sRX_RxSts__STATUS_REG )
    #define SDI12_UART_TXSTATUS_MASK_REG      (* (reg8 *) SDI12_UART_BUART_sRX_RxSts__MASK_REG )
    #define SDI12_UART_TXSTATUS_MASK_PTR      (  (reg8 *) SDI12_UART_BUART_sRX_RxSts__MASK_REG )
    #define SDI12_UART_TXSTATUS_ACTL_REG      (* (reg8 *) SDI12_UART_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define SDI12_UART_TXSTATUS_ACTL_PTR      (  (reg8 *) SDI12_UART_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End SDI12_UART_HD_ENABLED */

#if( (SDI12_UART_RX_ENABLED) || (SDI12_UART_HD_ENABLED) )
    #define SDI12_UART_RXDATA_REG             (* (reg8 *) SDI12_UART_BUART_sRX_RxShifter_u0__F0_REG )
    #define SDI12_UART_RXDATA_PTR             (  (reg8 *) SDI12_UART_BUART_sRX_RxShifter_u0__F0_REG )
    #define SDI12_UART_RXADDRESS1_REG         (* (reg8 *) SDI12_UART_BUART_sRX_RxShifter_u0__D0_REG )
    #define SDI12_UART_RXADDRESS1_PTR         (  (reg8 *) SDI12_UART_BUART_sRX_RxShifter_u0__D0_REG )
    #define SDI12_UART_RXADDRESS2_REG         (* (reg8 *) SDI12_UART_BUART_sRX_RxShifter_u0__D1_REG )
    #define SDI12_UART_RXADDRESS2_PTR         (  (reg8 *) SDI12_UART_BUART_sRX_RxShifter_u0__D1_REG )
    #define SDI12_UART_RXDATA_AUX_CTL_REG     (* (reg8 *) SDI12_UART_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define SDI12_UART_RXBITCTR_PERIOD_REG    (* (reg8 *) SDI12_UART_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define SDI12_UART_RXBITCTR_PERIOD_PTR    (  (reg8 *) SDI12_UART_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define SDI12_UART_RXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) SDI12_UART_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define SDI12_UART_RXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) SDI12_UART_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define SDI12_UART_RXBITCTR_COUNTER_REG   (* (reg8 *) SDI12_UART_BUART_sRX_RxBitCounter__COUNT_REG )
    #define SDI12_UART_RXBITCTR_COUNTER_PTR   (  (reg8 *) SDI12_UART_BUART_sRX_RxBitCounter__COUNT_REG )

    #define SDI12_UART_RXSTATUS_REG           (* (reg8 *) SDI12_UART_BUART_sRX_RxSts__STATUS_REG )
    #define SDI12_UART_RXSTATUS_PTR           (  (reg8 *) SDI12_UART_BUART_sRX_RxSts__STATUS_REG )
    #define SDI12_UART_RXSTATUS_MASK_REG      (* (reg8 *) SDI12_UART_BUART_sRX_RxSts__MASK_REG )
    #define SDI12_UART_RXSTATUS_MASK_PTR      (  (reg8 *) SDI12_UART_BUART_sRX_RxSts__MASK_REG )
    #define SDI12_UART_RXSTATUS_ACTL_REG      (* (reg8 *) SDI12_UART_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define SDI12_UART_RXSTATUS_ACTL_PTR      (  (reg8 *) SDI12_UART_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End  (SDI12_UART_RX_ENABLED) || (SDI12_UART_HD_ENABLED) */

#if(SDI12_UART_INTERNAL_CLOCK_USED)
    /* Register to enable or disable the digital clocks */
    #define SDI12_UART_INTCLOCK_CLKEN_REG     (* (reg8 *) SDI12_UART_IntClock__PM_ACT_CFG)
    #define SDI12_UART_INTCLOCK_CLKEN_PTR     (  (reg8 *) SDI12_UART_IntClock__PM_ACT_CFG)

    /* Clock mask for this clock. */
    #define SDI12_UART_INTCLOCK_CLKEN_MASK    SDI12_UART_IntClock__PM_ACT_MSK
#endif /* End SDI12_UART_INTERNAL_CLOCK_USED */


/***************************************
*       Register Constants
***************************************/

#if(SDI12_UART_TX_ENABLED)
    #define SDI12_UART_TX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End SDI12_UART_TX_ENABLED */

#if(SDI12_UART_HD_ENABLED)
    #define SDI12_UART_TX_FIFO_CLR            (0x02u) /* FIFO1 CLR */
#endif /* End SDI12_UART_HD_ENABLED */

#if( (SDI12_UART_RX_ENABLED) || (SDI12_UART_HD_ENABLED) )
    #define SDI12_UART_RX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End  (SDI12_UART_RX_ENABLED) || (SDI12_UART_HD_ENABLED) */


/***************************************
* The following code is DEPRECATED and
* should not be used in new projects.
***************************************/

/* UART v2_40 obsolete definitions */
#define SDI12_UART_WAIT_1_MS      SDI12_UART_BL_CHK_DELAY_MS   

#define SDI12_UART_TXBUFFERSIZE   SDI12_UART_TX_BUFFER_SIZE
#define SDI12_UART_RXBUFFERSIZE   SDI12_UART_RX_BUFFER_SIZE

#if (SDI12_UART_RXHW_ADDRESS_ENABLED)
    #define SDI12_UART_RXADDRESSMODE  SDI12_UART_RX_ADDRESS_MODE
    #define SDI12_UART_RXHWADDRESS1   SDI12_UART_RX_HW_ADDRESS1
    #define SDI12_UART_RXHWADDRESS2   SDI12_UART_RX_HW_ADDRESS2
    /* Backward compatible define */
    #define SDI12_UART_RXAddressMode  SDI12_UART_RXADDRESSMODE
#endif /* (SDI12_UART_RXHW_ADDRESS_ENABLED) */

/* UART v2_30 obsolete definitions */
#define SDI12_UART_initvar                    SDI12_UART_initVar

#define SDI12_UART_RX_Enabled                 SDI12_UART_RX_ENABLED
#define SDI12_UART_TX_Enabled                 SDI12_UART_TX_ENABLED
#define SDI12_UART_HD_Enabled                 SDI12_UART_HD_ENABLED
#define SDI12_UART_RX_IntInterruptEnabled     SDI12_UART_RX_INTERRUPT_ENABLED
#define SDI12_UART_TX_IntInterruptEnabled     SDI12_UART_TX_INTERRUPT_ENABLED
#define SDI12_UART_InternalClockUsed          SDI12_UART_INTERNAL_CLOCK_USED
#define SDI12_UART_RXHW_Address_Enabled       SDI12_UART_RXHW_ADDRESS_ENABLED
#define SDI12_UART_OverSampleCount            SDI12_UART_OVER_SAMPLE_COUNT
#define SDI12_UART_ParityType                 SDI12_UART_PARITY_TYPE

#if( SDI12_UART_TX_ENABLED && (SDI12_UART_TXBUFFERSIZE > SDI12_UART_FIFO_LENGTH))
    #define SDI12_UART_TXBUFFER               SDI12_UART_txBuffer
    #define SDI12_UART_TXBUFFERREAD           SDI12_UART_txBufferRead
    #define SDI12_UART_TXBUFFERWRITE          SDI12_UART_txBufferWrite
#endif /* End SDI12_UART_TX_ENABLED */
#if( ( SDI12_UART_RX_ENABLED || SDI12_UART_HD_ENABLED ) && \
     (SDI12_UART_RXBUFFERSIZE > SDI12_UART_FIFO_LENGTH) )
    #define SDI12_UART_RXBUFFER               SDI12_UART_rxBuffer
    #define SDI12_UART_RXBUFFERREAD           SDI12_UART_rxBufferRead
    #define SDI12_UART_RXBUFFERWRITE          SDI12_UART_rxBufferWrite
    #define SDI12_UART_RXBUFFERLOOPDETECT     SDI12_UART_rxBufferLoopDetect
    #define SDI12_UART_RXBUFFER_OVERFLOW      SDI12_UART_rxBufferOverflow
#endif /* End SDI12_UART_RX_ENABLED */

#ifdef SDI12_UART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define SDI12_UART_CONTROL                SDI12_UART_CONTROL_REG
#endif /* End SDI12_UART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(SDI12_UART_TX_ENABLED)
    #define SDI12_UART_TXDATA                 SDI12_UART_TXDATA_REG
    #define SDI12_UART_TXSTATUS               SDI12_UART_TXSTATUS_REG
    #define SDI12_UART_TXSTATUS_MASK          SDI12_UART_TXSTATUS_MASK_REG
    #define SDI12_UART_TXSTATUS_ACTL          SDI12_UART_TXSTATUS_ACTL_REG
    /* DP clock */
    #if(SDI12_UART_TXCLKGEN_DP)
        #define SDI12_UART_TXBITCLKGEN_CTR        SDI12_UART_TXBITCLKGEN_CTR_REG
        #define SDI12_UART_TXBITCLKTX_COMPLETE    SDI12_UART_TXBITCLKTX_COMPLETE_REG
    #else     /* Count7 clock*/
        #define SDI12_UART_TXBITCTR_PERIOD        SDI12_UART_TXBITCTR_PERIOD_REG
        #define SDI12_UART_TXBITCTR_CONTROL       SDI12_UART_TXBITCTR_CONTROL_REG
        #define SDI12_UART_TXBITCTR_COUNTER       SDI12_UART_TXBITCTR_COUNTER_REG
    #endif /* SDI12_UART_TXCLKGEN_DP */
#endif /* End SDI12_UART_TX_ENABLED */

#if(SDI12_UART_HD_ENABLED)
    #define SDI12_UART_TXDATA                 SDI12_UART_TXDATA_REG
    #define SDI12_UART_TXSTATUS               SDI12_UART_TXSTATUS_REG
    #define SDI12_UART_TXSTATUS_MASK          SDI12_UART_TXSTATUS_MASK_REG
    #define SDI12_UART_TXSTATUS_ACTL          SDI12_UART_TXSTATUS_ACTL_REG
#endif /* End SDI12_UART_HD_ENABLED */

#if( (SDI12_UART_RX_ENABLED) || (SDI12_UART_HD_ENABLED) )
    #define SDI12_UART_RXDATA                 SDI12_UART_RXDATA_REG
    #define SDI12_UART_RXADDRESS1             SDI12_UART_RXADDRESS1_REG
    #define SDI12_UART_RXADDRESS2             SDI12_UART_RXADDRESS2_REG
    #define SDI12_UART_RXBITCTR_PERIOD        SDI12_UART_RXBITCTR_PERIOD_REG
    #define SDI12_UART_RXBITCTR_CONTROL       SDI12_UART_RXBITCTR_CONTROL_REG
    #define SDI12_UART_RXBITCTR_COUNTER       SDI12_UART_RXBITCTR_COUNTER_REG
    #define SDI12_UART_RXSTATUS               SDI12_UART_RXSTATUS_REG
    #define SDI12_UART_RXSTATUS_MASK          SDI12_UART_RXSTATUS_MASK_REG
    #define SDI12_UART_RXSTATUS_ACTL          SDI12_UART_RXSTATUS_ACTL_REG
#endif /* End  (SDI12_UART_RX_ENABLED) || (SDI12_UART_HD_ENABLED) */

#if(SDI12_UART_INTERNAL_CLOCK_USED)
    #define SDI12_UART_INTCLOCK_CLKEN         SDI12_UART_INTCLOCK_CLKEN_REG
#endif /* End SDI12_UART_INTERNAL_CLOCK_USED */

#define SDI12_UART_WAIT_FOR_COMLETE_REINIT    SDI12_UART_WAIT_FOR_COMPLETE_REINIT

#endif  /* CY_UART_SDI12_UART_H */


/* [] END OF FILE */
