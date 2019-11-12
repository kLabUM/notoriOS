/*******************************************************************************
* File Name: DebugINT.c
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

#include "Debug.h"
#include "cyapicallbacks.h"


/***************************************
* Custom Declarations
***************************************/
/* `#START CUSTOM_DECLARATIONS` Place your declaration here */

/* `#END` */

#if (Debug_RX_INTERRUPT_ENABLED && (Debug_RX_ENABLED || Debug_HD_ENABLED))
    /*******************************************************************************
    * Function Name: Debug_RXISR
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
    *  Debug_rxBuffer - RAM buffer pointer for save received data.
    *  Debug_rxBufferWrite - cyclic index for write to rxBuffer,
    *     increments after each byte saved to buffer.
    *  Debug_rxBufferRead - cyclic index for read from rxBuffer,
    *     checked to detect overflow condition.
    *  Debug_rxBufferOverflow - software overflow flag. Set to one
    *     when Debug_rxBufferWrite index overtakes
    *     Debug_rxBufferRead index.
    *  Debug_rxBufferLoopDetect - additional variable to detect overflow.
    *     Set to one when Debug_rxBufferWrite is equal to
    *    Debug_rxBufferRead
    *  Debug_rxAddressMode - this variable contains the Address mode,
    *     selected in customizer or set by UART_SetRxAddressMode() API.
    *  Debug_rxAddressDetected - set to 1 when correct address received,
    *     and analysed to store following addressed data bytes to the buffer.
    *     When not correct address received, set to 0 to skip following data bytes.
    *
    *******************************************************************************/
    CY_ISR(Debug_RXISR)
    {
        uint8 readData;
        uint8 readStatus;
        uint8 increment_pointer = 0u;

    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef Debug_RXISR_ENTRY_CALLBACK
        Debug_RXISR_EntryCallback();
    #endif /* Debug_RXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START Debug_RXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        do
        {
            /* Read receiver status register */
            readStatus = Debug_RXSTATUS_REG;
            /* Copy the same status to readData variable for backward compatibility support 
            *  of the user code in Debug_RXISR_ERROR` section. 
            */
            readData = readStatus;

            if((readStatus & (Debug_RX_STS_BREAK | 
                            Debug_RX_STS_PAR_ERROR |
                            Debug_RX_STS_STOP_ERROR | 
                            Debug_RX_STS_OVERRUN)) != 0u)
            {
                /* ERROR handling. */
                Debug_errorStatus |= readStatus & ( Debug_RX_STS_BREAK | 
                                                            Debug_RX_STS_PAR_ERROR | 
                                                            Debug_RX_STS_STOP_ERROR | 
                                                            Debug_RX_STS_OVERRUN);
                /* `#START Debug_RXISR_ERROR` */

                /* `#END` */
                
            #ifdef Debug_RXISR_ERROR_CALLBACK
                Debug_RXISR_ERROR_Callback();
            #endif /* Debug_RXISR_ERROR_CALLBACK */
            }
            
            if((readStatus & Debug_RX_STS_FIFO_NOTEMPTY) != 0u)
            {
                /* Read data from the RX data register */
                readData = Debug_RXDATA_REG;
            #if (Debug_RXHW_ADDRESS_ENABLED)
                if(Debug_rxAddressMode == (uint8)Debug__B_UART__AM_SW_DETECT_TO_BUFFER)
                {
                    if((readStatus & Debug_RX_STS_MRKSPC) != 0u)
                    {
                        if ((readStatus & Debug_RX_STS_ADDR_MATCH) != 0u)
                        {
                            Debug_rxAddressDetected = 1u;
                        }
                        else
                        {
                            Debug_rxAddressDetected = 0u;
                        }
                    }
                    if(Debug_rxAddressDetected != 0u)
                    {   /* Store only addressed data */
                        Debug_rxBuffer[Debug_rxBufferWrite] = readData;
                        increment_pointer = 1u;
                    }
                }
                else /* Without software addressing */
                {
                    Debug_rxBuffer[Debug_rxBufferWrite] = readData;
                    increment_pointer = 1u;
                }
            #else  /* Without addressing */
                Debug_rxBuffer[Debug_rxBufferWrite] = readData;
                increment_pointer = 1u;
            #endif /* (Debug_RXHW_ADDRESS_ENABLED) */

                /* Do not increment buffer pointer when skip not addressed data */
                if(increment_pointer != 0u)
                {
                    if(Debug_rxBufferLoopDetect != 0u)
                    {   /* Set Software Buffer status Overflow */
                        Debug_rxBufferOverflow = 1u;
                    }
                    /* Set next pointer. */
                    Debug_rxBufferWrite++;

                    /* Check pointer for a loop condition */
                    if(Debug_rxBufferWrite >= Debug_RX_BUFFER_SIZE)
                    {
                        Debug_rxBufferWrite = 0u;
                    }

                    /* Detect pre-overload condition and set flag */
                    if(Debug_rxBufferWrite == Debug_rxBufferRead)
                    {
                        Debug_rxBufferLoopDetect = 1u;
                        /* When Hardware Flow Control selected */
                        #if (Debug_FLOW_CONTROL != 0u)
                            /* Disable RX interrupt mask, it is enabled when user read data from the buffer using APIs */
                            Debug_RXSTATUS_MASK_REG  &= (uint8)~Debug_RX_STS_FIFO_NOTEMPTY;
                            CyIntClearPending(Debug_RX_VECT_NUM);
                            break; /* Break the reading of the FIFO loop, leave the data there for generating RTS signal */
                        #endif /* (Debug_FLOW_CONTROL != 0u) */
                    }
                }
            }
        }while((readStatus & Debug_RX_STS_FIFO_NOTEMPTY) != 0u);

        /* User code required at end of ISR (Optional) */
        /* `#START Debug_RXISR_END` */

        /* `#END` */

    #ifdef Debug_RXISR_EXIT_CALLBACK
        Debug_RXISR_ExitCallback();
    #endif /* Debug_RXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
    }
    
#endif /* (Debug_RX_INTERRUPT_ENABLED && (Debug_RX_ENABLED || Debug_HD_ENABLED)) */


#if (Debug_TX_INTERRUPT_ENABLED && Debug_TX_ENABLED)
    /*******************************************************************************
    * Function Name: Debug_TXISR
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
    *  Debug_txBuffer - RAM buffer pointer for transmit data from.
    *  Debug_txBufferRead - cyclic index for read and transmit data
    *     from txBuffer, increments after each transmitted byte.
    *  Debug_rxBufferWrite - cyclic index for write to txBuffer,
    *     checked to detect available for transmission bytes.
    *
    *******************************************************************************/
    CY_ISR(Debug_TXISR)
    {
    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef Debug_TXISR_ENTRY_CALLBACK
        Debug_TXISR_EntryCallback();
    #endif /* Debug_TXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START Debug_TXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        while((Debug_txBufferRead != Debug_txBufferWrite) &&
             ((Debug_TXSTATUS_REG & Debug_TX_STS_FIFO_FULL) == 0u))
        {
            /* Check pointer wrap around */
            if(Debug_txBufferRead >= Debug_TX_BUFFER_SIZE)
            {
                Debug_txBufferRead = 0u;
            }

            Debug_TXDATA_REG = Debug_txBuffer[Debug_txBufferRead];

            /* Set next pointer */
            Debug_txBufferRead++;
        }

        /* User code required at end of ISR (Optional) */
        /* `#START Debug_TXISR_END` */

        /* `#END` */

    #ifdef Debug_TXISR_EXIT_CALLBACK
        Debug_TXISR_ExitCallback();
    #endif /* Debug_TXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
   }
#endif /* (Debug_TX_INTERRUPT_ENABLED && Debug_TX_ENABLED) */


/* [] END OF FILE */
