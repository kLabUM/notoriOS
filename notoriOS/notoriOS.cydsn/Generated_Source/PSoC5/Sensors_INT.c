/*******************************************************************************
* File Name: SensorsINT.c
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

#include "Sensors.h"
#include "cyapicallbacks.h"


/***************************************
* Custom Declarations
***************************************/
/* `#START CUSTOM_DECLARATIONS` Place your declaration here */

/* `#END` */

#if (Sensors_RX_INTERRUPT_ENABLED && (Sensors_RX_ENABLED || Sensors_HD_ENABLED))
    /*******************************************************************************
    * Function Name: Sensors_RXISR
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
    *  Sensors_rxBuffer - RAM buffer pointer for save received data.
    *  Sensors_rxBufferWrite - cyclic index for write to rxBuffer,
    *     increments after each byte saved to buffer.
    *  Sensors_rxBufferRead - cyclic index for read from rxBuffer,
    *     checked to detect overflow condition.
    *  Sensors_rxBufferOverflow - software overflow flag. Set to one
    *     when Sensors_rxBufferWrite index overtakes
    *     Sensors_rxBufferRead index.
    *  Sensors_rxBufferLoopDetect - additional variable to detect overflow.
    *     Set to one when Sensors_rxBufferWrite is equal to
    *    Sensors_rxBufferRead
    *  Sensors_rxAddressMode - this variable contains the Address mode,
    *     selected in customizer or set by UART_SetRxAddressMode() API.
    *  Sensors_rxAddressDetected - set to 1 when correct address received,
    *     and analysed to store following addressed data bytes to the buffer.
    *     When not correct address received, set to 0 to skip following data bytes.
    *
    *******************************************************************************/
    CY_ISR(Sensors_RXISR)
    {
        uint8 readData;
        uint8 readStatus;
        uint8 increment_pointer = 0u;

    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef Sensors_RXISR_ENTRY_CALLBACK
        Sensors_RXISR_EntryCallback();
    #endif /* Sensors_RXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START Sensors_RXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        do
        {
            /* Read receiver status register */
            readStatus = Sensors_RXSTATUS_REG;
            /* Copy the same status to readData variable for backward compatibility support 
            *  of the user code in Sensors_RXISR_ERROR` section. 
            */
            readData = readStatus;

            if((readStatus & (Sensors_RX_STS_BREAK | 
                            Sensors_RX_STS_PAR_ERROR |
                            Sensors_RX_STS_STOP_ERROR | 
                            Sensors_RX_STS_OVERRUN)) != 0u)
            {
                /* ERROR handling. */
                Sensors_errorStatus |= readStatus & ( Sensors_RX_STS_BREAK | 
                                                            Sensors_RX_STS_PAR_ERROR | 
                                                            Sensors_RX_STS_STOP_ERROR | 
                                                            Sensors_RX_STS_OVERRUN);
                /* `#START Sensors_RXISR_ERROR` */

                /* `#END` */
                
            #ifdef Sensors_RXISR_ERROR_CALLBACK
                Sensors_RXISR_ERROR_Callback();
            #endif /* Sensors_RXISR_ERROR_CALLBACK */
            }
            
            if((readStatus & Sensors_RX_STS_FIFO_NOTEMPTY) != 0u)
            {
                /* Read data from the RX data register */
                readData = Sensors_RXDATA_REG;
            #if (Sensors_RXHW_ADDRESS_ENABLED)
                if(Sensors_rxAddressMode == (uint8)Sensors__B_UART__AM_SW_DETECT_TO_BUFFER)
                {
                    if((readStatus & Sensors_RX_STS_MRKSPC) != 0u)
                    {
                        if ((readStatus & Sensors_RX_STS_ADDR_MATCH) != 0u)
                        {
                            Sensors_rxAddressDetected = 1u;
                        }
                        else
                        {
                            Sensors_rxAddressDetected = 0u;
                        }
                    }
                    if(Sensors_rxAddressDetected != 0u)
                    {   /* Store only addressed data */
                        Sensors_rxBuffer[Sensors_rxBufferWrite] = readData;
                        increment_pointer = 1u;
                    }
                }
                else /* Without software addressing */
                {
                    Sensors_rxBuffer[Sensors_rxBufferWrite] = readData;
                    increment_pointer = 1u;
                }
            #else  /* Without addressing */
                Sensors_rxBuffer[Sensors_rxBufferWrite] = readData;
                increment_pointer = 1u;
            #endif /* (Sensors_RXHW_ADDRESS_ENABLED) */

                /* Do not increment buffer pointer when skip not addressed data */
                if(increment_pointer != 0u)
                {
                    if(Sensors_rxBufferLoopDetect != 0u)
                    {   /* Set Software Buffer status Overflow */
                        Sensors_rxBufferOverflow = 1u;
                    }
                    /* Set next pointer. */
                    Sensors_rxBufferWrite++;

                    /* Check pointer for a loop condition */
                    if(Sensors_rxBufferWrite >= Sensors_RX_BUFFER_SIZE)
                    {
                        Sensors_rxBufferWrite = 0u;
                    }

                    /* Detect pre-overload condition and set flag */
                    if(Sensors_rxBufferWrite == Sensors_rxBufferRead)
                    {
                        Sensors_rxBufferLoopDetect = 1u;
                        /* When Hardware Flow Control selected */
                        #if (Sensors_FLOW_CONTROL != 0u)
                            /* Disable RX interrupt mask, it is enabled when user read data from the buffer using APIs */
                            Sensors_RXSTATUS_MASK_REG  &= (uint8)~Sensors_RX_STS_FIFO_NOTEMPTY;
                            CyIntClearPending(Sensors_RX_VECT_NUM);
                            break; /* Break the reading of the FIFO loop, leave the data there for generating RTS signal */
                        #endif /* (Sensors_FLOW_CONTROL != 0u) */
                    }
                }
            }
        }while((readStatus & Sensors_RX_STS_FIFO_NOTEMPTY) != 0u);

        /* User code required at end of ISR (Optional) */
        /* `#START Sensors_RXISR_END` */

        /* `#END` */

    #ifdef Sensors_RXISR_EXIT_CALLBACK
        Sensors_RXISR_ExitCallback();
    #endif /* Sensors_RXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
    }
    
#endif /* (Sensors_RX_INTERRUPT_ENABLED && (Sensors_RX_ENABLED || Sensors_HD_ENABLED)) */


#if (Sensors_TX_INTERRUPT_ENABLED && Sensors_TX_ENABLED)
    /*******************************************************************************
    * Function Name: Sensors_TXISR
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
    *  Sensors_txBuffer - RAM buffer pointer for transmit data from.
    *  Sensors_txBufferRead - cyclic index for read and transmit data
    *     from txBuffer, increments after each transmitted byte.
    *  Sensors_rxBufferWrite - cyclic index for write to txBuffer,
    *     checked to detect available for transmission bytes.
    *
    *******************************************************************************/
    CY_ISR(Sensors_TXISR)
    {
    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef Sensors_TXISR_ENTRY_CALLBACK
        Sensors_TXISR_EntryCallback();
    #endif /* Sensors_TXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START Sensors_TXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        while((Sensors_txBufferRead != Sensors_txBufferWrite) &&
             ((Sensors_TXSTATUS_REG & Sensors_TX_STS_FIFO_FULL) == 0u))
        {
            /* Check pointer wrap around */
            if(Sensors_txBufferRead >= Sensors_TX_BUFFER_SIZE)
            {
                Sensors_txBufferRead = 0u;
            }

            Sensors_TXDATA_REG = Sensors_txBuffer[Sensors_txBufferRead];

            /* Set next pointer */
            Sensors_txBufferRead++;
        }

        /* User code required at end of ISR (Optional) */
        /* `#START Sensors_TXISR_END` */

        /* `#END` */

    #ifdef Sensors_TXISR_EXIT_CALLBACK
        Sensors_TXISR_ExitCallback();
    #endif /* Sensors_TXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
   }
#endif /* (Sensors_TX_INTERRUPT_ENABLED && Sensors_TX_ENABLED) */


/* [] END OF FILE */
