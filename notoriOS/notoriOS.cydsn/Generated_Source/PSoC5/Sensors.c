/*******************************************************************************
* File Name: Sensors.c
* Version 2.50
*
* Description:
*  This file provides all API functionality of the UART component
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "Sensors.h"
#if (Sensors_INTERNAL_CLOCK_USED)
    #include "Sensors_IntClock.h"
#endif /* End Sensors_INTERNAL_CLOCK_USED */


/***************************************
* Global data allocation
***************************************/

uint8 Sensors_initVar = 0u;

#if (Sensors_TX_INTERRUPT_ENABLED && Sensors_TX_ENABLED)
    volatile uint8 Sensors_txBuffer[Sensors_TX_BUFFER_SIZE];
    volatile uint8 Sensors_txBufferRead = 0u;
    uint8 Sensors_txBufferWrite = 0u;
#endif /* (Sensors_TX_INTERRUPT_ENABLED && Sensors_TX_ENABLED) */

#if (Sensors_RX_INTERRUPT_ENABLED && (Sensors_RX_ENABLED || Sensors_HD_ENABLED))
    uint8 Sensors_errorStatus = 0u;
    volatile uint8 Sensors_rxBuffer[Sensors_RX_BUFFER_SIZE];
    volatile uint8 Sensors_rxBufferRead  = 0u;
    volatile uint8 Sensors_rxBufferWrite = 0u;
    volatile uint8 Sensors_rxBufferLoopDetect = 0u;
    volatile uint8 Sensors_rxBufferOverflow   = 0u;
    #if (Sensors_RXHW_ADDRESS_ENABLED)
        volatile uint8 Sensors_rxAddressMode = Sensors_RX_ADDRESS_MODE;
        volatile uint8 Sensors_rxAddressDetected = 0u;
    #endif /* (Sensors_RXHW_ADDRESS_ENABLED) */
#endif /* (Sensors_RX_INTERRUPT_ENABLED && (Sensors_RX_ENABLED || Sensors_HD_ENABLED)) */


/*******************************************************************************
* Function Name: Sensors_Start
********************************************************************************
*
* Summary:
*  This is the preferred method to begin component operation.
*  Sensors_Start() sets the initVar variable, calls the
*  Sensors_Init() function, and then calls the
*  Sensors_Enable() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  The Sensors_intiVar variable is used to indicate initial
*  configuration of this component. The variable is initialized to zero (0u)
*  and set to one (1u) the first time Sensors_Start() is called. This
*  allows for component initialization without re-initialization in all
*  subsequent calls to the Sensors_Start() routine.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Sensors_Start(void) 
{
    /* If not initialized then initialize all required hardware and software */
    if(Sensors_initVar == 0u)
    {
        Sensors_Init();
        Sensors_initVar = 1u;
    }

    Sensors_Enable();
}


/*******************************************************************************
* Function Name: Sensors_Init
********************************************************************************
*
* Summary:
*  Initializes or restores the component according to the customizer Configure
*  dialog settings. It is not necessary to call Sensors_Init() because
*  the Sensors_Start() API calls this function and is the preferred
*  method to begin component operation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void Sensors_Init(void) 
{
    #if(Sensors_RX_ENABLED || Sensors_HD_ENABLED)

        #if (Sensors_RX_INTERRUPT_ENABLED)
            /* Set RX interrupt vector and priority */
            (void) CyIntSetVector(Sensors_RX_VECT_NUM, &Sensors_RXISR);
            CyIntSetPriority(Sensors_RX_VECT_NUM, Sensors_RX_PRIOR_NUM);
            Sensors_errorStatus = 0u;
        #endif /* (Sensors_RX_INTERRUPT_ENABLED) */

        #if (Sensors_RXHW_ADDRESS_ENABLED)
            Sensors_SetRxAddressMode(Sensors_RX_ADDRESS_MODE);
            Sensors_SetRxAddress1(Sensors_RX_HW_ADDRESS1);
            Sensors_SetRxAddress2(Sensors_RX_HW_ADDRESS2);
        #endif /* End Sensors_RXHW_ADDRESS_ENABLED */

        /* Init Count7 period */
        Sensors_RXBITCTR_PERIOD_REG = Sensors_RXBITCTR_INIT;
        /* Configure the Initial RX interrupt mask */
        Sensors_RXSTATUS_MASK_REG  = Sensors_INIT_RX_INTERRUPTS_MASK;
    #endif /* End Sensors_RX_ENABLED || Sensors_HD_ENABLED*/

    #if(Sensors_TX_ENABLED)
        #if (Sensors_TX_INTERRUPT_ENABLED)
            /* Set TX interrupt vector and priority */
            (void) CyIntSetVector(Sensors_TX_VECT_NUM, &Sensors_TXISR);
            CyIntSetPriority(Sensors_TX_VECT_NUM, Sensors_TX_PRIOR_NUM);
        #endif /* (Sensors_TX_INTERRUPT_ENABLED) */

        /* Write Counter Value for TX Bit Clk Generator*/
        #if (Sensors_TXCLKGEN_DP)
            Sensors_TXBITCLKGEN_CTR_REG = Sensors_BIT_CENTER;
            Sensors_TXBITCLKTX_COMPLETE_REG = ((Sensors_NUMBER_OF_DATA_BITS +
                        Sensors_NUMBER_OF_START_BIT) * Sensors_OVER_SAMPLE_COUNT) - 1u;
        #else
            Sensors_TXBITCTR_PERIOD_REG = ((Sensors_NUMBER_OF_DATA_BITS +
                        Sensors_NUMBER_OF_START_BIT) * Sensors_OVER_SAMPLE_8) - 1u;
        #endif /* End Sensors_TXCLKGEN_DP */

        /* Configure the Initial TX interrupt mask */
        #if (Sensors_TX_INTERRUPT_ENABLED)
            Sensors_TXSTATUS_MASK_REG = Sensors_TX_STS_FIFO_EMPTY;
        #else
            Sensors_TXSTATUS_MASK_REG = Sensors_INIT_TX_INTERRUPTS_MASK;
        #endif /*End Sensors_TX_INTERRUPT_ENABLED*/

    #endif /* End Sensors_TX_ENABLED */

    #if(Sensors_PARITY_TYPE_SW)  /* Write Parity to Control Register */
        Sensors_WriteControlRegister( \
            (Sensors_ReadControlRegister() & (uint8)~Sensors_CTRL_PARITY_TYPE_MASK) | \
            (uint8)(Sensors_PARITY_TYPE << Sensors_CTRL_PARITY_TYPE0_SHIFT) );
    #endif /* End Sensors_PARITY_TYPE_SW */
}


/*******************************************************************************
* Function Name: Sensors_Enable
********************************************************************************
*
* Summary:
*  Activates the hardware and begins component operation. It is not necessary
*  to call Sensors_Enable() because the Sensors_Start() API
*  calls this function, which is the preferred method to begin component
*  operation.

* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  Sensors_rxAddressDetected - set to initial state (0).
*
*******************************************************************************/
void Sensors_Enable(void) 
{
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    #if (Sensors_RX_ENABLED || Sensors_HD_ENABLED)
        /* RX Counter (Count7) Enable */
        Sensors_RXBITCTR_CONTROL_REG |= Sensors_CNTR_ENABLE;

        /* Enable the RX Interrupt */
        Sensors_RXSTATUS_ACTL_REG  |= Sensors_INT_ENABLE;

        #if (Sensors_RX_INTERRUPT_ENABLED)
            Sensors_EnableRxInt();

            #if (Sensors_RXHW_ADDRESS_ENABLED)
                Sensors_rxAddressDetected = 0u;
            #endif /* (Sensors_RXHW_ADDRESS_ENABLED) */
        #endif /* (Sensors_RX_INTERRUPT_ENABLED) */
    #endif /* (Sensors_RX_ENABLED || Sensors_HD_ENABLED) */

    #if(Sensors_TX_ENABLED)
        /* TX Counter (DP/Count7) Enable */
        #if(!Sensors_TXCLKGEN_DP)
            Sensors_TXBITCTR_CONTROL_REG |= Sensors_CNTR_ENABLE;
        #endif /* End Sensors_TXCLKGEN_DP */

        /* Enable the TX Interrupt */
        Sensors_TXSTATUS_ACTL_REG |= Sensors_INT_ENABLE;
        #if (Sensors_TX_INTERRUPT_ENABLED)
            Sensors_ClearPendingTxInt(); /* Clear history of TX_NOT_EMPTY */
            Sensors_EnableTxInt();
        #endif /* (Sensors_TX_INTERRUPT_ENABLED) */
     #endif /* (Sensors_TX_INTERRUPT_ENABLED) */

    #if (Sensors_INTERNAL_CLOCK_USED)
        Sensors_IntClock_Start();  /* Enable the clock */
    #endif /* (Sensors_INTERNAL_CLOCK_USED) */

    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: Sensors_Stop
********************************************************************************
*
* Summary:
*  Disables the UART operation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void Sensors_Stop(void) 
{
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    /* Write Bit Counter Disable */
    #if (Sensors_RX_ENABLED || Sensors_HD_ENABLED)
        Sensors_RXBITCTR_CONTROL_REG &= (uint8) ~Sensors_CNTR_ENABLE;
    #endif /* (Sensors_RX_ENABLED || Sensors_HD_ENABLED) */

    #if (Sensors_TX_ENABLED)
        #if(!Sensors_TXCLKGEN_DP)
            Sensors_TXBITCTR_CONTROL_REG &= (uint8) ~Sensors_CNTR_ENABLE;
        #endif /* (!Sensors_TXCLKGEN_DP) */
    #endif /* (Sensors_TX_ENABLED) */

    #if (Sensors_INTERNAL_CLOCK_USED)
        Sensors_IntClock_Stop();   /* Disable the clock */
    #endif /* (Sensors_INTERNAL_CLOCK_USED) */

    /* Disable internal interrupt component */
    #if (Sensors_RX_ENABLED || Sensors_HD_ENABLED)
        Sensors_RXSTATUS_ACTL_REG  &= (uint8) ~Sensors_INT_ENABLE;

        #if (Sensors_RX_INTERRUPT_ENABLED)
            Sensors_DisableRxInt();
        #endif /* (Sensors_RX_INTERRUPT_ENABLED) */
    #endif /* (Sensors_RX_ENABLED || Sensors_HD_ENABLED) */

    #if (Sensors_TX_ENABLED)
        Sensors_TXSTATUS_ACTL_REG &= (uint8) ~Sensors_INT_ENABLE;

        #if (Sensors_TX_INTERRUPT_ENABLED)
            Sensors_DisableTxInt();
        #endif /* (Sensors_TX_INTERRUPT_ENABLED) */
    #endif /* (Sensors_TX_ENABLED) */

    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: Sensors_ReadControlRegister
********************************************************************************
*
* Summary:
*  Returns the current value of the control register.
*
* Parameters:
*  None.
*
* Return:
*  Contents of the control register.
*
*******************************************************************************/
uint8 Sensors_ReadControlRegister(void) 
{
    #if (Sensors_CONTROL_REG_REMOVED)
        return(0u);
    #else
        return(Sensors_CONTROL_REG);
    #endif /* (Sensors_CONTROL_REG_REMOVED) */
}


/*******************************************************************************
* Function Name: Sensors_WriteControlRegister
********************************************************************************
*
* Summary:
*  Writes an 8-bit value into the control register
*
* Parameters:
*  control:  control register value
*
* Return:
*  None.
*
*******************************************************************************/
void  Sensors_WriteControlRegister(uint8 control) 
{
    #if (Sensors_CONTROL_REG_REMOVED)
        if(0u != control)
        {
            /* Suppress compiler warning */
        }
    #else
       Sensors_CONTROL_REG = control;
    #endif /* (Sensors_CONTROL_REG_REMOVED) */
}


#if(Sensors_RX_ENABLED || Sensors_HD_ENABLED)
    /*******************************************************************************
    * Function Name: Sensors_SetRxInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the RX interrupt sources enabled.
    *
    * Parameters:
    *  IntSrc:  Bit field containing the RX interrupts to enable. Based on the 
    *  bit-field arrangement of the status register. This value must be a 
    *  combination of status register bit-masks shown below:
    *      Sensors_RX_STS_FIFO_NOTEMPTY    Interrupt on byte received.
    *      Sensors_RX_STS_PAR_ERROR        Interrupt on parity error.
    *      Sensors_RX_STS_STOP_ERROR       Interrupt on stop error.
    *      Sensors_RX_STS_BREAK            Interrupt on break.
    *      Sensors_RX_STS_OVERRUN          Interrupt on overrun error.
    *      Sensors_RX_STS_ADDR_MATCH       Interrupt on address match.
    *      Sensors_RX_STS_MRKSPC           Interrupt on address detect.
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Enables the output of specific status bits to the interrupt controller
    *
    *******************************************************************************/
    void Sensors_SetRxInterruptMode(uint8 intSrc) 
    {
        Sensors_RXSTATUS_MASK_REG  = intSrc;
    }


    /*******************************************************************************
    * Function Name: Sensors_ReadRxData
    ********************************************************************************
    *
    * Summary:
    *  Returns the next byte of received data. This function returns data without
    *  checking the status. You must check the status separately.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Received data from RX register
    *
    * Global Variables:
    *  Sensors_rxBuffer - RAM buffer pointer for save received data.
    *  Sensors_rxBufferWrite - cyclic index for write to rxBuffer,
    *     checked to identify new data.
    *  Sensors_rxBufferRead - cyclic index for read from rxBuffer,
    *     incremented after each byte has been read from buffer.
    *  Sensors_rxBufferLoopDetect - cleared if loop condition was detected
    *     in RX ISR.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint8 Sensors_ReadRxData(void) 
    {
        uint8 rxData;

    #if (Sensors_RX_INTERRUPT_ENABLED)

        uint8 locRxBufferRead;
        uint8 locRxBufferWrite;

        /* Protect variables that could change on interrupt */
        Sensors_DisableRxInt();

        locRxBufferRead  = Sensors_rxBufferRead;
        locRxBufferWrite = Sensors_rxBufferWrite;

        if( (Sensors_rxBufferLoopDetect != 0u) || (locRxBufferRead != locRxBufferWrite) )
        {
            rxData = Sensors_rxBuffer[locRxBufferRead];
            locRxBufferRead++;

            if(locRxBufferRead >= Sensors_RX_BUFFER_SIZE)
            {
                locRxBufferRead = 0u;
            }
            /* Update the real pointer */
            Sensors_rxBufferRead = locRxBufferRead;

            if(Sensors_rxBufferLoopDetect != 0u)
            {
                Sensors_rxBufferLoopDetect = 0u;
                #if ((Sensors_RX_INTERRUPT_ENABLED) && (Sensors_FLOW_CONTROL != 0u))
                    /* When Hardware Flow Control selected - return RX mask */
                    #if( Sensors_HD_ENABLED )
                        if((Sensors_CONTROL_REG & Sensors_CTRL_HD_SEND) == 0u)
                        {   /* In Half duplex mode return RX mask only in RX
                            *  configuration set, otherwise
                            *  mask will be returned in LoadRxConfig() API.
                            */
                            Sensors_RXSTATUS_MASK_REG  |= Sensors_RX_STS_FIFO_NOTEMPTY;
                        }
                    #else
                        Sensors_RXSTATUS_MASK_REG  |= Sensors_RX_STS_FIFO_NOTEMPTY;
                    #endif /* end Sensors_HD_ENABLED */
                #endif /* ((Sensors_RX_INTERRUPT_ENABLED) && (Sensors_FLOW_CONTROL != 0u)) */
            }
        }
        else
        {   /* Needs to check status for RX_STS_FIFO_NOTEMPTY bit */
            rxData = Sensors_RXDATA_REG;
        }

        Sensors_EnableRxInt();

    #else

        /* Needs to check status for RX_STS_FIFO_NOTEMPTY bit */
        rxData = Sensors_RXDATA_REG;

    #endif /* (Sensors_RX_INTERRUPT_ENABLED) */

        return(rxData);
    }


    /*******************************************************************************
    * Function Name: Sensors_ReadRxStatus
    ********************************************************************************
    *
    * Summary:
    *  Returns the current state of the receiver status register and the software
    *  buffer overflow status.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Current state of the status register.
    *
    * Side Effect:
    *  All status register bits are clear-on-read except
    *  Sensors_RX_STS_FIFO_NOTEMPTY.
    *  Sensors_RX_STS_FIFO_NOTEMPTY clears immediately after RX data
    *  register read.
    *
    * Global Variables:
    *  Sensors_rxBufferOverflow - used to indicate overload condition.
    *   It set to one in RX interrupt when there isn't free space in
    *   Sensors_rxBufferRead to write new data. This condition returned
    *   and cleared to zero by this API as an
    *   Sensors_RX_STS_SOFT_BUFF_OVER bit along with RX Status register
    *   bits.
    *
    *******************************************************************************/
    uint8 Sensors_ReadRxStatus(void) 
    {
        uint8 status;

        status = Sensors_RXSTATUS_REG & Sensors_RX_HW_MASK;

    #if (Sensors_RX_INTERRUPT_ENABLED)
        if(Sensors_rxBufferOverflow != 0u)
        {
            status |= Sensors_RX_STS_SOFT_BUFF_OVER;
            Sensors_rxBufferOverflow = 0u;
        }
    #endif /* (Sensors_RX_INTERRUPT_ENABLED) */

        return(status);
    }


    /*******************************************************************************
    * Function Name: Sensors_GetChar
    ********************************************************************************
    *
    * Summary:
    *  Returns the last received byte of data. Sensors_GetChar() is
    *  designed for ASCII characters and returns a uint8 where 1 to 255 are values
    *  for valid characters and 0 indicates an error occurred or no data is present.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Character read from UART RX buffer. ASCII characters from 1 to 255 are valid.
    *  A returned zero signifies an error condition or no data available.
    *
    * Global Variables:
    *  Sensors_rxBuffer - RAM buffer pointer for save received data.
    *  Sensors_rxBufferWrite - cyclic index for write to rxBuffer,
    *     checked to identify new data.
    *  Sensors_rxBufferRead - cyclic index for read from rxBuffer,
    *     incremented after each byte has been read from buffer.
    *  Sensors_rxBufferLoopDetect - cleared if loop condition was detected
    *     in RX ISR.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint8 Sensors_GetChar(void) 
    {
        uint8 rxData = 0u;
        uint8 rxStatus;

    #if (Sensors_RX_INTERRUPT_ENABLED)
        uint8 locRxBufferRead;
        uint8 locRxBufferWrite;

        /* Protect variables that could change on interrupt */
        Sensors_DisableRxInt();

        locRxBufferRead  = Sensors_rxBufferRead;
        locRxBufferWrite = Sensors_rxBufferWrite;

        if( (Sensors_rxBufferLoopDetect != 0u) || (locRxBufferRead != locRxBufferWrite) )
        {
            rxData = Sensors_rxBuffer[locRxBufferRead];
            locRxBufferRead++;
            if(locRxBufferRead >= Sensors_RX_BUFFER_SIZE)
            {
                locRxBufferRead = 0u;
            }
            /* Update the real pointer */
            Sensors_rxBufferRead = locRxBufferRead;

            if(Sensors_rxBufferLoopDetect != 0u)
            {
                Sensors_rxBufferLoopDetect = 0u;
                #if( (Sensors_RX_INTERRUPT_ENABLED) && (Sensors_FLOW_CONTROL != 0u) )
                    /* When Hardware Flow Control selected - return RX mask */
                    #if( Sensors_HD_ENABLED )
                        if((Sensors_CONTROL_REG & Sensors_CTRL_HD_SEND) == 0u)
                        {   /* In Half duplex mode return RX mask only if
                            *  RX configuration set, otherwise
                            *  mask will be returned in LoadRxConfig() API.
                            */
                            Sensors_RXSTATUS_MASK_REG |= Sensors_RX_STS_FIFO_NOTEMPTY;
                        }
                    #else
                        Sensors_RXSTATUS_MASK_REG |= Sensors_RX_STS_FIFO_NOTEMPTY;
                    #endif /* end Sensors_HD_ENABLED */
                #endif /* Sensors_RX_INTERRUPT_ENABLED and Hardware flow control*/
            }

        }
        else
        {   rxStatus = Sensors_RXSTATUS_REG;
            if((rxStatus & Sensors_RX_STS_FIFO_NOTEMPTY) != 0u)
            {   /* Read received data from FIFO */
                rxData = Sensors_RXDATA_REG;
                /*Check status on error*/
                if((rxStatus & (Sensors_RX_STS_BREAK | Sensors_RX_STS_PAR_ERROR |
                                Sensors_RX_STS_STOP_ERROR | Sensors_RX_STS_OVERRUN)) != 0u)
                {
                    rxData = 0u;
                }
            }
        }

        Sensors_EnableRxInt();

    #else

        rxStatus =Sensors_RXSTATUS_REG;
        if((rxStatus & Sensors_RX_STS_FIFO_NOTEMPTY) != 0u)
        {
            /* Read received data from FIFO */
            rxData = Sensors_RXDATA_REG;

            /*Check status on error*/
            if((rxStatus & (Sensors_RX_STS_BREAK | Sensors_RX_STS_PAR_ERROR |
                            Sensors_RX_STS_STOP_ERROR | Sensors_RX_STS_OVERRUN)) != 0u)
            {
                rxData = 0u;
            }
        }
    #endif /* (Sensors_RX_INTERRUPT_ENABLED) */

        return(rxData);
    }


    /*******************************************************************************
    * Function Name: Sensors_GetByte
    ********************************************************************************
    *
    * Summary:
    *  Reads UART RX buffer immediately, returns received character and error
    *  condition.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  MSB contains status and LSB contains UART RX data. If the MSB is nonzero,
    *  an error has occurred.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint16 Sensors_GetByte(void) 
    {
        
    #if (Sensors_RX_INTERRUPT_ENABLED)
        uint16 locErrorStatus;
        /* Protect variables that could change on interrupt */
        Sensors_DisableRxInt();
        locErrorStatus = (uint16)Sensors_errorStatus;
        Sensors_errorStatus = 0u;
        Sensors_EnableRxInt();
        return ( (uint16)(locErrorStatus << 8u) | Sensors_ReadRxData() );
    #else
        return ( ((uint16)Sensors_ReadRxStatus() << 8u) | Sensors_ReadRxData() );
    #endif /* Sensors_RX_INTERRUPT_ENABLED */
        
    }


    /*******************************************************************************
    * Function Name: Sensors_GetRxBufferSize
    ********************************************************************************
    *
    * Summary:
    *  Returns the number of received bytes available in the RX buffer.
    *  * RX software buffer is disabled (RX Buffer Size parameter is equal to 4): 
    *    returns 0 for empty RX FIFO or 1 for not empty RX FIFO.
    *  * RX software buffer is enabled: returns the number of bytes available in 
    *    the RX software buffer. Bytes available in the RX FIFO do not take to 
    *    account.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  uint8: Number of bytes in the RX buffer. 
    *    Return value type depends on RX Buffer Size parameter.
    *
    * Global Variables:
    *  Sensors_rxBufferWrite - used to calculate left bytes.
    *  Sensors_rxBufferRead - used to calculate left bytes.
    *  Sensors_rxBufferLoopDetect - checked to decide left bytes amount.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to find out how full the RX Buffer is.
    *
    *******************************************************************************/
    uint8 Sensors_GetRxBufferSize(void)
                                                            
    {
        uint8 size;

    #if (Sensors_RX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt */
        Sensors_DisableRxInt();

        if(Sensors_rxBufferRead == Sensors_rxBufferWrite)
        {
            if(Sensors_rxBufferLoopDetect != 0u)
            {
                size = Sensors_RX_BUFFER_SIZE;
            }
            else
            {
                size = 0u;
            }
        }
        else if(Sensors_rxBufferRead < Sensors_rxBufferWrite)
        {
            size = (Sensors_rxBufferWrite - Sensors_rxBufferRead);
        }
        else
        {
            size = (Sensors_RX_BUFFER_SIZE - Sensors_rxBufferRead) + Sensors_rxBufferWrite;
        }

        Sensors_EnableRxInt();

    #else

        /* We can only know if there is data in the fifo. */
        size = ((Sensors_RXSTATUS_REG & Sensors_RX_STS_FIFO_NOTEMPTY) != 0u) ? 1u : 0u;

    #endif /* (Sensors_RX_INTERRUPT_ENABLED) */

        return(size);
    }


    /*******************************************************************************
    * Function Name: Sensors_ClearRxBuffer
    ********************************************************************************
    *
    * Summary:
    *  Clears the receiver memory buffer and hardware RX FIFO of all received data.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  Sensors_rxBufferWrite - cleared to zero.
    *  Sensors_rxBufferRead - cleared to zero.
    *  Sensors_rxBufferLoopDetect - cleared to zero.
    *  Sensors_rxBufferOverflow - cleared to zero.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Setting the pointers to zero makes the system believe there is no data to
    *  read and writing will resume at address 0 overwriting any data that may
    *  have remained in the RAM.
    *
    * Side Effects:
    *  Any received data not read from the RAM or FIFO buffer will be lost.
    *
    *******************************************************************************/
    void Sensors_ClearRxBuffer(void) 
    {
        uint8 enableInterrupts;

        /* Clear the HW FIFO */
        enableInterrupts = CyEnterCriticalSection();
        Sensors_RXDATA_AUX_CTL_REG |= (uint8)  Sensors_RX_FIFO_CLR;
        Sensors_RXDATA_AUX_CTL_REG &= (uint8) ~Sensors_RX_FIFO_CLR;
        CyExitCriticalSection(enableInterrupts);

    #if (Sensors_RX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        Sensors_DisableRxInt();

        Sensors_rxBufferRead = 0u;
        Sensors_rxBufferWrite = 0u;
        Sensors_rxBufferLoopDetect = 0u;
        Sensors_rxBufferOverflow = 0u;

        Sensors_EnableRxInt();

    #endif /* (Sensors_RX_INTERRUPT_ENABLED) */

    }


    /*******************************************************************************
    * Function Name: Sensors_SetRxAddressMode
    ********************************************************************************
    *
    * Summary:
    *  Sets the software controlled Addressing mode used by the RX portion of the
    *  UART.
    *
    * Parameters:
    *  addressMode: Enumerated value indicating the mode of RX addressing
    *  Sensors__B_UART__AM_SW_BYTE_BYTE -  Software Byte-by-Byte address
    *                                               detection
    *  Sensors__B_UART__AM_SW_DETECT_TO_BUFFER - Software Detect to Buffer
    *                                               address detection
    *  Sensors__B_UART__AM_HW_BYTE_BY_BYTE - Hardware Byte-by-Byte address
    *                                               detection
    *  Sensors__B_UART__AM_HW_DETECT_TO_BUFFER - Hardware Detect to Buffer
    *                                               address detection
    *  Sensors__B_UART__AM_NONE - No address detection
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  Sensors_rxAddressMode - the parameter stored in this variable for
    *   the farther usage in RX ISR.
    *  Sensors_rxAddressDetected - set to initial state (0).
    *
    *******************************************************************************/
    void Sensors_SetRxAddressMode(uint8 addressMode)
                                                        
    {
        #if(Sensors_RXHW_ADDRESS_ENABLED)
            #if(Sensors_CONTROL_REG_REMOVED)
                if(0u != addressMode)
                {
                    /* Suppress compiler warning */
                }
            #else /* Sensors_CONTROL_REG_REMOVED */
                uint8 tmpCtrl;
                tmpCtrl = Sensors_CONTROL_REG & (uint8)~Sensors_CTRL_RXADDR_MODE_MASK;
                tmpCtrl |= (uint8)(addressMode << Sensors_CTRL_RXADDR_MODE0_SHIFT);
                Sensors_CONTROL_REG = tmpCtrl;

                #if(Sensors_RX_INTERRUPT_ENABLED && \
                   (Sensors_RXBUFFERSIZE > Sensors_FIFO_LENGTH) )
                    Sensors_rxAddressMode = addressMode;
                    Sensors_rxAddressDetected = 0u;
                #endif /* End Sensors_RXBUFFERSIZE > Sensors_FIFO_LENGTH*/
            #endif /* End Sensors_CONTROL_REG_REMOVED */
        #else /* Sensors_RXHW_ADDRESS_ENABLED */
            if(0u != addressMode)
            {
                /* Suppress compiler warning */
            }
        #endif /* End Sensors_RXHW_ADDRESS_ENABLED */
    }


    /*******************************************************************************
    * Function Name: Sensors_SetRxAddress1
    ********************************************************************************
    *
    * Summary:
    *  Sets the first of two hardware-detectable receiver addresses.
    *
    * Parameters:
    *  address: Address #1 for hardware address detection.
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    void Sensors_SetRxAddress1(uint8 address) 
    {
        Sensors_RXADDRESS1_REG = address;
    }


    /*******************************************************************************
    * Function Name: Sensors_SetRxAddress2
    ********************************************************************************
    *
    * Summary:
    *  Sets the second of two hardware-detectable receiver addresses.
    *
    * Parameters:
    *  address: Address #2 for hardware address detection.
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    void Sensors_SetRxAddress2(uint8 address) 
    {
        Sensors_RXADDRESS2_REG = address;
    }

#endif  /* Sensors_RX_ENABLED || Sensors_HD_ENABLED*/


#if( (Sensors_TX_ENABLED) || (Sensors_HD_ENABLED) )
    /*******************************************************************************
    * Function Name: Sensors_SetTxInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the TX interrupt sources to be enabled, but does not enable the
    *  interrupt.
    *
    * Parameters:
    *  intSrc: Bit field containing the TX interrupt sources to enable
    *   Sensors_TX_STS_COMPLETE        Interrupt on TX byte complete
    *   Sensors_TX_STS_FIFO_EMPTY      Interrupt when TX FIFO is empty
    *   Sensors_TX_STS_FIFO_FULL       Interrupt when TX FIFO is full
    *   Sensors_TX_STS_FIFO_NOT_FULL   Interrupt when TX FIFO is not full
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Enables the output of specific status bits to the interrupt controller
    *
    *******************************************************************************/
    void Sensors_SetTxInterruptMode(uint8 intSrc) 
    {
        Sensors_TXSTATUS_MASK_REG = intSrc;
    }


    /*******************************************************************************
    * Function Name: Sensors_WriteTxData
    ********************************************************************************
    *
    * Summary:
    *  Places a byte of data into the transmit buffer to be sent when the bus is
    *  available without checking the TX status register. You must check status
    *  separately.
    *
    * Parameters:
    *  txDataByte: data byte
    *
    * Return:
    * None.
    *
    * Global Variables:
    *  Sensors_txBuffer - RAM buffer pointer for save data for transmission
    *  Sensors_txBufferWrite - cyclic index for write to txBuffer,
    *    incremented after each byte saved to buffer.
    *  Sensors_txBufferRead - cyclic index for read from txBuffer,
    *    checked to identify the condition to write to FIFO directly or to TX buffer
    *  Sensors_initVar - checked to identify that the component has been
    *    initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void Sensors_WriteTxData(uint8 txDataByte) 
    {
        /* If not Initialized then skip this function*/
        if(Sensors_initVar != 0u)
        {
        #if (Sensors_TX_INTERRUPT_ENABLED)

            /* Protect variables that could change on interrupt. */
            Sensors_DisableTxInt();

            if( (Sensors_txBufferRead == Sensors_txBufferWrite) &&
                ((Sensors_TXSTATUS_REG & Sensors_TX_STS_FIFO_FULL) == 0u) )
            {
                /* Add directly to the FIFO. */
                Sensors_TXDATA_REG = txDataByte;
            }
            else
            {
                if(Sensors_txBufferWrite >= Sensors_TX_BUFFER_SIZE)
                {
                    Sensors_txBufferWrite = 0u;
                }

                Sensors_txBuffer[Sensors_txBufferWrite] = txDataByte;

                /* Add to the software buffer. */
                Sensors_txBufferWrite++;
            }

            Sensors_EnableTxInt();

        #else

            /* Add directly to the FIFO. */
            Sensors_TXDATA_REG = txDataByte;

        #endif /*(Sensors_TX_INTERRUPT_ENABLED) */
        }
    }


    /*******************************************************************************
    * Function Name: Sensors_ReadTxStatus
    ********************************************************************************
    *
    * Summary:
    *  Reads the status register for the TX portion of the UART.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Contents of the status register
    *
    * Theory:
    *  This function reads the TX status register, which is cleared on read.
    *  It is up to the user to handle all bits in this return value accordingly,
    *  even if the bit was not enabled as an interrupt source the event happened
    *  and must be handled accordingly.
    *
    *******************************************************************************/
    uint8 Sensors_ReadTxStatus(void) 
    {
        return(Sensors_TXSTATUS_REG);
    }


    /*******************************************************************************
    * Function Name: Sensors_PutChar
    ********************************************************************************
    *
    * Summary:
    *  Puts a byte of data into the transmit buffer to be sent when the bus is
    *  available. This is a blocking API that waits until the TX buffer has room to
    *  hold the data.
    *
    * Parameters:
    *  txDataByte: Byte containing the data to transmit
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  Sensors_txBuffer - RAM buffer pointer for save data for transmission
    *  Sensors_txBufferWrite - cyclic index for write to txBuffer,
    *     checked to identify free space in txBuffer and incremented after each byte
    *     saved to buffer.
    *  Sensors_txBufferRead - cyclic index for read from txBuffer,
    *     checked to identify free space in txBuffer.
    *  Sensors_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to transmit any byte of data in a single transfer
    *
    *******************************************************************************/
    void Sensors_PutChar(uint8 txDataByte) 
    {
    #if (Sensors_TX_INTERRUPT_ENABLED)
        /* The temporary output pointer is used since it takes two instructions
        *  to increment with a wrap, and we can't risk doing that with the real
        *  pointer and getting an interrupt in between instructions.
        */
        uint8 locTxBufferWrite;
        uint8 locTxBufferRead;

        do
        { /* Block if software buffer is full, so we don't overwrite. */

        #if ((Sensors_TX_BUFFER_SIZE > Sensors_MAX_BYTE_VALUE) && (CY_PSOC3))
            /* Disable TX interrupt to protect variables from modification */
            Sensors_DisableTxInt();
        #endif /* (Sensors_TX_BUFFER_SIZE > Sensors_MAX_BYTE_VALUE) && (CY_PSOC3) */

            locTxBufferWrite = Sensors_txBufferWrite;
            locTxBufferRead  = Sensors_txBufferRead;

        #if ((Sensors_TX_BUFFER_SIZE > Sensors_MAX_BYTE_VALUE) && (CY_PSOC3))
            /* Enable interrupt to continue transmission */
            Sensors_EnableTxInt();
        #endif /* (Sensors_TX_BUFFER_SIZE > Sensors_MAX_BYTE_VALUE) && (CY_PSOC3) */
        }
        while( (locTxBufferWrite < locTxBufferRead) ? (locTxBufferWrite == (locTxBufferRead - 1u)) :
                                ((locTxBufferWrite - locTxBufferRead) ==
                                (uint8)(Sensors_TX_BUFFER_SIZE - 1u)) );

        if( (locTxBufferRead == locTxBufferWrite) &&
            ((Sensors_TXSTATUS_REG & Sensors_TX_STS_FIFO_FULL) == 0u) )
        {
            /* Add directly to the FIFO */
            Sensors_TXDATA_REG = txDataByte;
        }
        else
        {
            if(locTxBufferWrite >= Sensors_TX_BUFFER_SIZE)
            {
                locTxBufferWrite = 0u;
            }
            /* Add to the software buffer. */
            Sensors_txBuffer[locTxBufferWrite] = txDataByte;
            locTxBufferWrite++;

            /* Finally, update the real output pointer */
        #if ((Sensors_TX_BUFFER_SIZE > Sensors_MAX_BYTE_VALUE) && (CY_PSOC3))
            Sensors_DisableTxInt();
        #endif /* (Sensors_TX_BUFFER_SIZE > Sensors_MAX_BYTE_VALUE) && (CY_PSOC3) */

            Sensors_txBufferWrite = locTxBufferWrite;

        #if ((Sensors_TX_BUFFER_SIZE > Sensors_MAX_BYTE_VALUE) && (CY_PSOC3))
            Sensors_EnableTxInt();
        #endif /* (Sensors_TX_BUFFER_SIZE > Sensors_MAX_BYTE_VALUE) && (CY_PSOC3) */

            if(0u != (Sensors_TXSTATUS_REG & Sensors_TX_STS_FIFO_EMPTY))
            {
                /* Trigger TX interrupt to send software buffer */
                Sensors_SetPendingTxInt();
            }
        }

    #else

        while((Sensors_TXSTATUS_REG & Sensors_TX_STS_FIFO_FULL) != 0u)
        {
            /* Wait for room in the FIFO */
        }

        /* Add directly to the FIFO */
        Sensors_TXDATA_REG = txDataByte;

    #endif /* Sensors_TX_INTERRUPT_ENABLED */
    }


    /*******************************************************************************
    * Function Name: Sensors_PutString
    ********************************************************************************
    *
    * Summary:
    *  Sends a NULL terminated string to the TX buffer for transmission.
    *
    * Parameters:
    *  string[]: Pointer to the null terminated string array residing in RAM or ROM
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  Sensors_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  If there is not enough memory in the TX buffer for the entire string, this
    *  function blocks until the last character of the string is loaded into the
    *  TX buffer.
    *
    *******************************************************************************/
    void Sensors_PutString(const char8 string[]) 
    {
        uint16 bufIndex = 0u;

        /* If not Initialized then skip this function */
        if(Sensors_initVar != 0u)
        {
            /* This is a blocking function, it will not exit until all data is sent */
            while(string[bufIndex] != (char8) 0)
            {
                Sensors_PutChar((uint8)string[bufIndex]);
                bufIndex++;
            }
        }
    }


    /*******************************************************************************
    * Function Name: Sensors_PutArray
    ********************************************************************************
    *
    * Summary:
    *  Places N bytes of data from a memory array into the TX buffer for
    *  transmission.
    *
    * Parameters:
    *  string[]: Address of the memory array residing in RAM or ROM.
    *  byteCount: Number of bytes to be transmitted. The type depends on TX Buffer
    *             Size parameter.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  Sensors_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  If there is not enough memory in the TX buffer for the entire string, this
    *  function blocks until the last character of the string is loaded into the
    *  TX buffer.
    *
    *******************************************************************************/
    void Sensors_PutArray(const uint8 string[], uint8 byteCount)
                                                                    
    {
        uint8 bufIndex = 0u;

        /* If not Initialized then skip this function */
        if(Sensors_initVar != 0u)
        {
            while(bufIndex < byteCount)
            {
                Sensors_PutChar(string[bufIndex]);
                bufIndex++;
            }
        }
    }


    /*******************************************************************************
    * Function Name: Sensors_PutCRLF
    ********************************************************************************
    *
    * Summary:
    *  Writes a byte of data followed by a carriage return (0x0D) and line feed
    *  (0x0A) to the transmit buffer.
    *
    * Parameters:
    *  txDataByte: Data byte to transmit before the carriage return and line feed.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  Sensors_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void Sensors_PutCRLF(uint8 txDataByte) 
    {
        /* If not Initialized then skip this function */
        if(Sensors_initVar != 0u)
        {
            Sensors_PutChar(txDataByte);
            Sensors_PutChar(0x0Du);
            Sensors_PutChar(0x0Au);
        }
    }


    /*******************************************************************************
    * Function Name: Sensors_GetTxBufferSize
    ********************************************************************************
    *
    * Summary:
    *  Returns the number of bytes in the TX buffer which are waiting to be 
    *  transmitted.
    *  * TX software buffer is disabled (TX Buffer Size parameter is equal to 4): 
    *    returns 0 for empty TX FIFO, 1 for not full TX FIFO or 4 for full TX FIFO.
    *  * TX software buffer is enabled: returns the number of bytes in the TX 
    *    software buffer which are waiting to be transmitted. Bytes available in the
    *    TX FIFO do not count.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Number of bytes used in the TX buffer. Return value type depends on the TX 
    *  Buffer Size parameter.
    *
    * Global Variables:
    *  Sensors_txBufferWrite - used to calculate left space.
    *  Sensors_txBufferRead - used to calculate left space.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to find out how full the TX Buffer is.
    *
    *******************************************************************************/
    uint8 Sensors_GetTxBufferSize(void)
                                                            
    {
        uint8 size;

    #if (Sensors_TX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        Sensors_DisableTxInt();

        if(Sensors_txBufferRead == Sensors_txBufferWrite)
        {
            size = 0u;
        }
        else if(Sensors_txBufferRead < Sensors_txBufferWrite)
        {
            size = (Sensors_txBufferWrite - Sensors_txBufferRead);
        }
        else
        {
            size = (Sensors_TX_BUFFER_SIZE - Sensors_txBufferRead) +
                    Sensors_txBufferWrite;
        }

        Sensors_EnableTxInt();

    #else

        size = Sensors_TXSTATUS_REG;

        /* Is the fifo is full. */
        if((size & Sensors_TX_STS_FIFO_FULL) != 0u)
        {
            size = Sensors_FIFO_LENGTH;
        }
        else if((size & Sensors_TX_STS_FIFO_EMPTY) != 0u)
        {
            size = 0u;
        }
        else
        {
            /* We only know there is data in the fifo. */
            size = 1u;
        }

    #endif /* (Sensors_TX_INTERRUPT_ENABLED) */

    return(size);
    }


    /*******************************************************************************
    * Function Name: Sensors_ClearTxBuffer
    ********************************************************************************
    *
    * Summary:
    *  Clears all data from the TX buffer and hardware TX FIFO.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  Sensors_txBufferWrite - cleared to zero.
    *  Sensors_txBufferRead - cleared to zero.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Setting the pointers to zero makes the system believe there is no data to
    *  read and writing will resume at address 0 overwriting any data that may have
    *  remained in the RAM.
    *
    * Side Effects:
    *  Data waiting in the transmit buffer is not sent; a byte that is currently
    *  transmitting finishes transmitting.
    *
    *******************************************************************************/
    void Sensors_ClearTxBuffer(void) 
    {
        uint8 enableInterrupts;

        enableInterrupts = CyEnterCriticalSection();
        /* Clear the HW FIFO */
        Sensors_TXDATA_AUX_CTL_REG |= (uint8)  Sensors_TX_FIFO_CLR;
        Sensors_TXDATA_AUX_CTL_REG &= (uint8) ~Sensors_TX_FIFO_CLR;
        CyExitCriticalSection(enableInterrupts);

    #if (Sensors_TX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        Sensors_DisableTxInt();

        Sensors_txBufferRead = 0u;
        Sensors_txBufferWrite = 0u;

        /* Enable Tx interrupt. */
        Sensors_EnableTxInt();

    #endif /* (Sensors_TX_INTERRUPT_ENABLED) */
    }


    /*******************************************************************************
    * Function Name: Sensors_SendBreak
    ********************************************************************************
    *
    * Summary:
    *  Transmits a break signal on the bus.
    *
    * Parameters:
    *  uint8 retMode:  Send Break return mode. See the following table for options.
    *   Sensors_SEND_BREAK - Initialize registers for break, send the Break
    *       signal and return immediately.
    *   Sensors_WAIT_FOR_COMPLETE_REINIT - Wait until break transmission is
    *       complete, reinitialize registers to normal transmission mode then return
    *   Sensors_REINIT - Reinitialize registers to normal transmission mode
    *       then return.
    *   Sensors_SEND_WAIT_REINIT - Performs both options: 
    *      Sensors_SEND_BREAK and Sensors_WAIT_FOR_COMPLETE_REINIT.
    *      This option is recommended for most cases.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  Sensors_initVar - checked to identify that the component has been
    *     initialized.
    *  txPeriod - static variable, used for keeping TX period configuration.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  SendBreak function initializes registers to send 13-bit break signal. It is
    *  important to return the registers configuration to normal for continue 8-bit
    *  operation.
    *  There are 3 variants for this API usage:
    *  1) SendBreak(3) - function will send the Break signal and take care on the
    *     configuration returning. Function will block CPU until transmission
    *     complete.
    *  2) User may want to use blocking time if UART configured to the low speed
    *     operation
    *     Example for this case:
    *     SendBreak(0);     - initialize Break signal transmission
    *         Add your code here to use CPU time
    *     SendBreak(1);     - complete Break operation
    *  3) Same to 2) but user may want to initialize and use the interrupt to
    *     complete break operation.
    *     Example for this case:
    *     Initialize TX interrupt with "TX - On TX Complete" parameter
    *     SendBreak(0);     - initialize Break signal transmission
    *         Add your code here to use CPU time
    *     When interrupt appear with Sensors_TX_STS_COMPLETE status:
    *     SendBreak(2);     - complete Break operation
    *
    * Side Effects:
    *  The Sensors_SendBreak() function initializes registers to send a
    *  break signal.
    *  Break signal length depends on the break signal bits configuration.
    *  The register configuration should be reinitialized before normal 8-bit
    *  communication can continue.
    *
    *******************************************************************************/
    void Sensors_SendBreak(uint8 retMode) 
    {

        /* If not Initialized then skip this function*/
        if(Sensors_initVar != 0u)
        {
            /* Set the Counter to 13-bits and transmit a 00 byte */
            /* When that is done then reset the counter value back */
            uint8 tmpStat;

        #if(Sensors_HD_ENABLED) /* Half Duplex mode*/

            if( (retMode == Sensors_SEND_BREAK) ||
                (retMode == Sensors_SEND_WAIT_REINIT ) )
            {
                /* CTRL_HD_SEND_BREAK - sends break bits in HD mode */
                Sensors_WriteControlRegister(Sensors_ReadControlRegister() |
                                                      Sensors_CTRL_HD_SEND_BREAK);
                /* Send zeros */
                Sensors_TXDATA_REG = 0u;

                do /* Wait until transmit starts */
                {
                    tmpStat = Sensors_TXSTATUS_REG;
                }
                while((tmpStat & Sensors_TX_STS_FIFO_EMPTY) != 0u);
            }

            if( (retMode == Sensors_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == Sensors_SEND_WAIT_REINIT) )
            {
                do /* Wait until transmit complete */
                {
                    tmpStat = Sensors_TXSTATUS_REG;
                }
                while(((uint8)~tmpStat & Sensors_TX_STS_COMPLETE) != 0u);
            }

            if( (retMode == Sensors_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == Sensors_REINIT) ||
                (retMode == Sensors_SEND_WAIT_REINIT) )
            {
                Sensors_WriteControlRegister(Sensors_ReadControlRegister() &
                                              (uint8)~Sensors_CTRL_HD_SEND_BREAK);
            }

        #else /* Sensors_HD_ENABLED Full Duplex mode */

            static uint8 txPeriod;

            if( (retMode == Sensors_SEND_BREAK) ||
                (retMode == Sensors_SEND_WAIT_REINIT) )
            {
                /* CTRL_HD_SEND_BREAK - skip to send parity bit at Break signal in Full Duplex mode */
                #if( (Sensors_PARITY_TYPE != Sensors__B_UART__NONE_REVB) || \
                                    (Sensors_PARITY_TYPE_SW != 0u) )
                    Sensors_WriteControlRegister(Sensors_ReadControlRegister() |
                                                          Sensors_CTRL_HD_SEND_BREAK);
                #endif /* End Sensors_PARITY_TYPE != Sensors__B_UART__NONE_REVB  */

                #if(Sensors_TXCLKGEN_DP)
                    txPeriod = Sensors_TXBITCLKTX_COMPLETE_REG;
                    Sensors_TXBITCLKTX_COMPLETE_REG = Sensors_TXBITCTR_BREAKBITS;
                #else
                    txPeriod = Sensors_TXBITCTR_PERIOD_REG;
                    Sensors_TXBITCTR_PERIOD_REG = Sensors_TXBITCTR_BREAKBITS8X;
                #endif /* End Sensors_TXCLKGEN_DP */

                /* Send zeros */
                Sensors_TXDATA_REG = 0u;

                do /* Wait until transmit starts */
                {
                    tmpStat = Sensors_TXSTATUS_REG;
                }
                while((tmpStat & Sensors_TX_STS_FIFO_EMPTY) != 0u);
            }

            if( (retMode == Sensors_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == Sensors_SEND_WAIT_REINIT) )
            {
                do /* Wait until transmit complete */
                {
                    tmpStat = Sensors_TXSTATUS_REG;
                }
                while(((uint8)~tmpStat & Sensors_TX_STS_COMPLETE) != 0u);
            }

            if( (retMode == Sensors_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == Sensors_REINIT) ||
                (retMode == Sensors_SEND_WAIT_REINIT) )
            {

            #if(Sensors_TXCLKGEN_DP)
                Sensors_TXBITCLKTX_COMPLETE_REG = txPeriod;
            #else
                Sensors_TXBITCTR_PERIOD_REG = txPeriod;
            #endif /* End Sensors_TXCLKGEN_DP */

            #if( (Sensors_PARITY_TYPE != Sensors__B_UART__NONE_REVB) || \
                 (Sensors_PARITY_TYPE_SW != 0u) )
                Sensors_WriteControlRegister(Sensors_ReadControlRegister() &
                                                      (uint8) ~Sensors_CTRL_HD_SEND_BREAK);
            #endif /* End Sensors_PARITY_TYPE != NONE */
            }
        #endif    /* End Sensors_HD_ENABLED */
        }
    }


    /*******************************************************************************
    * Function Name: Sensors_SetTxAddressMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the transmitter to signal the next bytes is address or data.
    *
    * Parameters:
    *  addressMode: 
    *       Sensors_SET_SPACE - Configure the transmitter to send the next
    *                                    byte as a data.
    *       Sensors_SET_MARK  - Configure the transmitter to send the next
    *                                    byte as an address.
    *
    * Return:
    *  None.
    *
    * Side Effects:
    *  This function sets and clears Sensors_CTRL_MARK bit in the Control
    *  register.
    *
    *******************************************************************************/
    void Sensors_SetTxAddressMode(uint8 addressMode) 
    {
        /* Mark/Space sending enable */
        if(addressMode != 0u)
        {
        #if( Sensors_CONTROL_REG_REMOVED == 0u )
            Sensors_WriteControlRegister(Sensors_ReadControlRegister() |
                                                  Sensors_CTRL_MARK);
        #endif /* End Sensors_CONTROL_REG_REMOVED == 0u */
        }
        else
        {
        #if( Sensors_CONTROL_REG_REMOVED == 0u )
            Sensors_WriteControlRegister(Sensors_ReadControlRegister() &
                                                  (uint8) ~Sensors_CTRL_MARK);
        #endif /* End Sensors_CONTROL_REG_REMOVED == 0u */
        }
    }

#endif  /* EndSensors_TX_ENABLED */

#if(Sensors_HD_ENABLED)


    /*******************************************************************************
    * Function Name: Sensors_LoadRxConfig
    ********************************************************************************
    *
    * Summary:
    *  Loads the receiver configuration in half duplex mode. After calling this
    *  function, the UART is ready to receive data.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Side Effects:
    *  Valid only in half duplex mode. You must make sure that the previous
    *  transaction is complete and it is safe to unload the transmitter
    *  configuration.
    *
    *******************************************************************************/
    void Sensors_LoadRxConfig(void) 
    {
        Sensors_WriteControlRegister(Sensors_ReadControlRegister() &
                                                (uint8)~Sensors_CTRL_HD_SEND);
        Sensors_RXBITCTR_PERIOD_REG = Sensors_HD_RXBITCTR_INIT;

    #if (Sensors_RX_INTERRUPT_ENABLED)
        /* Enable RX interrupt after set RX configuration */
        Sensors_SetRxInterruptMode(Sensors_INIT_RX_INTERRUPTS_MASK);
    #endif /* (Sensors_RX_INTERRUPT_ENABLED) */
    }


    /*******************************************************************************
    * Function Name: Sensors_LoadTxConfig
    ********************************************************************************
    *
    * Summary:
    *  Loads the transmitter configuration in half duplex mode. After calling this
    *  function, the UART is ready to transmit data.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Side Effects:
    *  Valid only in half duplex mode. You must make sure that the previous
    *  transaction is complete and it is safe to unload the receiver configuration.
    *
    *******************************************************************************/
    void Sensors_LoadTxConfig(void) 
    {
    #if (Sensors_RX_INTERRUPT_ENABLED)
        /* Disable RX interrupts before set TX configuration */
        Sensors_SetRxInterruptMode(0u);
    #endif /* (Sensors_RX_INTERRUPT_ENABLED) */

        Sensors_WriteControlRegister(Sensors_ReadControlRegister() | Sensors_CTRL_HD_SEND);
        Sensors_RXBITCTR_PERIOD_REG = Sensors_HD_TXBITCTR_INIT;
    }

#endif  /* Sensors_HD_ENABLED */


/* [] END OF FILE */
