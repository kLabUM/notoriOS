/*******************************************************************************
* File Name: ModemINT.c
* Version 2.50
*
* Description:
*  This file provides all Interrupt Service functionality of the UART component
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "Modem.h"
#include "cyapicallbacks.h"


/***************************************
* Custom Declarations
***************************************/
/* `#START CUSTOM_DECLARATIONS` Place your declaration here */

/* `#END` */

#if (Modem_RX_INTERRUPT_ENABLED && (Modem_RX_ENABLED || Modem_HD_ENABLED))
    /*******************************************************************************
    * Function Name: Modem_RXISR
    ********************************************************************************
    *
    * Summary:
    *  Interrupt Service Routine for RX portion of the UART
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  Modem_rxBuffer - RAM buffer pointer for save received data.
    *  Modem_rxBufferWrite - cyclic index for write to rxBuffer,
    *     increments after each byte saved to buffer.
    *  Modem_rxBufferRead - cyclic index for read from rxBuffer,
    *     checked to detect overflow condition.
    *  Modem_rxBufferOverflow - software overflow flag. Set to one
    *     when Modem_rxBufferWrite index overtakes
    *     Modem_rxBufferRead index.
    *  Modem_rxBufferLoopDetect - additional variable to detect overflow.
    *     Set to one when Modem_rxBufferWrite is equal to
    *    Modem_rxBufferRead
    *  Modem_rxAddressMode - this variable contains the Address mode,
    *     selected in customizer or set by UART_SetRxAddressMode() API.
    *  Modem_rxAddressDetected - set to 1 when correct address received,
    *     and analysed to store following addressed data bytes to the buffer.
    *     When not correct address received, set to 0 to skip following data bytes.
    *
    *******************************************************************************/
    CY_ISR(Modem_RXISR)
    {
        uint8 readData;
        uint8 readStatus;
        uint8 increment_pointer = 0u;

    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef Modem_RXISR_ENTRY_CALLBACK
        Modem_RXISR_EntryCallback();
    #endif /* Modem_RXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START Modem_RXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        do
        {
            /* Read receiver status register */
            readStatus = Modem_RXSTATUS_REG;
            /* Copy the same status to readData variable for backward compatibility support 
            *  of the user code in Modem_RXISR_ERROR` section. 
            */
            readData = readStatus;

            if((readStatus & (Modem_RX_STS_BREAK | 
                            Modem_RX_STS_PAR_ERROR |
                            Modem_RX_STS_STOP_ERROR | 
                            Modem_RX_STS_OVERRUN)) != 0u)
            {
                /* ERROR handling. */
                Modem_errorStatus |= readStatus & ( Modem_RX_STS_BREAK | 
                                                            Modem_RX_STS_PAR_ERROR | 
                                                            Modem_RX_STS_STOP_ERROR | 
                                                            Modem_RX_STS_OVERRUN);
                /* `#START Modem_RXISR_ERROR` */

                /* `#END` */
                
            #ifdef Modem_RXISR_ERROR_CALLBACK
                Modem_RXISR_ERROR_Callback();
            #endif /* Modem_RXISR_ERROR_CALLBACK */
            }
            
            if((readStatus & Modem_RX_STS_FIFO_NOTEMPTY) != 0u)
            {
                /* Read data from the RX data register */
                readData = Modem_RXDATA_REG;
            #if (Modem_RXHW_ADDRESS_ENABLED)
                if(Modem_rxAddressMode == (uint8)Modem__B_UART__AM_SW_DETECT_TO_BUFFER)
                {
                    if((readStatus & Modem_RX_STS_MRKSPC) != 0u)
                    {
                        if ((readStatus & Modem_RX_STS_ADDR_MATCH) != 0u)
                        {
                            Modem_rxAddressDetected = 1u;
                        }
                        else
                        {
                            Modem_rxAddressDetected = 0u;
                        }
                    }
                    if(Modem_rxAddressDetected != 0u)
                    {   /* Store only addressed data */
                        Modem_rxBuffer[Modem_rxBufferWrite] = readData;
                        increment_pointer = 1u;
                    }
                }
                else /* Without software addressing */
                {
                    Modem_rxBuffer[Modem_rxBufferWrite] = readData;
                    increment_pointer = 1u;
                }
            #else  /* Without addressing */
                Modem_rxBuffer[Modem_rxBufferWrite] = readData;
                increment_pointer = 1u;
            #endif /* (Modem_RXHW_ADDRESS_ENABLED) */

                /* Do not increment buffer pointer when skip not addressed data */
                if(increment_pointer != 0u)
                {
                    if(Modem_rxBufferLoopDetect != 0u)
                    {   /* Set Software Buffer status Overflow */
                        Modem_rxBufferOverflow = 1u;
                    }
                    /* Set next pointer. */
                    Modem_rxBufferWrite++;

                    /* Check pointer for a loop condition */
                    if(Modem_rxBufferWrite >= Modem_RX_BUFFER_SIZE)
                    {
                        Modem_rxBufferWrite = 0u;
                    }

                    /* Detect pre-overload condition and set flag */
                    if(Modem_rxBufferWrite == Modem_rxBufferRead)
                    {
                        Modem_rxBufferLoopDetect = 1u;
                        /* When Hardware Flow Control selected */
                        #if (Modem_FLOW_CONTROL != 0u)
                            /* Disable RX interrupt mask, it is enabled when user read data from the buffer using APIs */
                            Modem_RXSTATUS_MASK_REG  &= (uint8)~Modem_RX_STS_FIFO_NOTEMPTY;
                            CyIntClearPending(Modem_RX_VECT_NUM);
                            break; /* Break the reading of the FIFO loop, leave the data there for generating RTS signal */
                        #endif /* (Modem_FLOW_CONTROL != 0u) */
                    }
                }
            }
        }while((readStatus & Modem_RX_STS_FIFO_NOTEMPTY) != 0u);

        /* User code required at end of ISR (Optional) */
        /* `#START Modem_RXISR_END` */

        /* `#END` */

    #ifdef Modem_RXISR_EXIT_CALLBACK
        Modem_RXISR_ExitCallback();
    #endif /* Modem_RXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
    }
    
#endif /* (Modem_RX_INTERRUPT_ENABLED && (Modem_RX_ENABLED || Modem_HD_ENABLED)) */


#if (Modem_TX_INTERRUPT_ENABLED && Modem_TX_ENABLED)
    /*******************************************************************************
    * Function Name: Modem_TXISR
    ********************************************************************************
    *
    * Summary:
    * Interrupt Service Routine for the TX portion of the UART
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  Modem_txBuffer - RAM buffer pointer for transmit data from.
    *  Modem_txBufferRead - cyclic index for read and transmit data
    *     from txBuffer, increments after each transmitted byte.
    *  Modem_rxBufferWrite - cyclic index for write to txBuffer,
    *     checked to detect available for transmission bytes.
    *
    *******************************************************************************/
    CY_ISR(Modem_TXISR)
    {
    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef Modem_TXISR_ENTRY_CALLBACK
        Modem_TXISR_EntryCallback();
    #endif /* Modem_TXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START Modem_TXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        while((Modem_txBufferRead != Modem_txBufferWrite) &&
             ((Modem_TXSTATUS_REG & Modem_TX_STS_FIFO_FULL) == 0u))
        {
            /* Check pointer wrap around */
            if(Modem_txBufferRead >= Modem_TX_BUFFER_SIZE)
            {
                Modem_txBufferRead = 0u;
            }

            Modem_TXDATA_REG = Modem_txBuffer[Modem_txBufferRead];

            /* Set next pointer */
            Modem_txBufferRead++;
        }

        /* User code required at end of ISR (Optional) */
        /* `#START Modem_TXISR_END` */

        /* `#END` */

    #ifdef Modem_TXISR_EXIT_CALLBACK
        Modem_TXISR_ExitCallback();
    #endif /* Modem_TXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
   }
#endif /* (Modem_TX_INTERRUPT_ENABLED && Modem_TX_ENABLED) */


/* [] END OF FILE */
