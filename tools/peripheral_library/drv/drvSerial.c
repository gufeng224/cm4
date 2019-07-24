/*This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief FreeRTOS Serial Port management example for AVR32 UC3.
 *
 * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32
 * - Supported devices:  All AVR32 devices can be used.
 * - AppNote:
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support and FAQ: http://support.atmel.no/
 *
 *****************************************************************************/

/* Copyright (c) 2007, Atmel Corporation All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of ATMEL may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


/*
  BASIC INTERRUPT DRIVEN SERIAL PORT DRIVER FOR USART.
*/

/* Kernel includes. */
#include "mosWrapper.h"

/* Demo application includes. */
#include "serial.h"
#include "halUART.h"

/*-----------------------------------------------------------*/

/* Constants to setup and access the USART. */
#define serINVALID_COMPORT_HANDLER        ( ( xComPortHandle ) 0 )
#define serINVALID_QUEUE                  ( ( QueueHandle_t ) 0 )
#define serHANDLE                         ( ( xComPortHandle ) 1 )
#define serNO_BLOCK                       ( ( TickType_t ) 0 )

/*-----------------------------------------------------------*/

/* Queues used to hold received characters, and characters waiting to be
transmitted. */
static QueueHandle_t xRxedChars;
static QueueHandle_t xCharsForTx;

/*-----------------------------------------------------------*/

/* Forward declaration. */
static void vprvSerialCreateQueues( unsigned portBASE_TYPE uxQueueLength,
									QueueHandle_t *pxRxedChars,
									QueueHandle_t *pxCharsForTx );

/*
 * UART interrupt service routine.
 */
#define UART_INT_PRIORITY		( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1 )


void NVIC_Handler_UART(void)
{
	/* Now we can declare the local variables. */
	signed char     cChar;
	portBASE_TYPE     xHigherPriorityTaskWoken = pdFALSE;
	//portBASE_TYPE retstatus;

	U8     nSource;

	/* What caused the interrupt? */
  nSource = uart_int_source();

  //UartSendTrace("NVIC_Handler_UART0 nSource = %d\n", nSource);

  while(nSource != UART_IIR_NO_INT)
  {
  	if (nSource == UART_IIR_THRI)
  	{
      //UartSendTrace("UART_IIR_THRI\n");

#if 0
  		/* The interrupt was caused by the THR becoming empty.  Are there any
  		more characters to transmit?
  		Because FreeRTOS is not supposed to run with nested interrupts, put all OS
  		calls in a critical section . */
  		portENTER_CRITICAL();
  			retstatus = xQueueReceiveFromISR( xCharsForTx, &cChar, &xHigherPriorityTaskWoken );
  		portEXIT_CRITICAL();

  		if (retstatus == pdTRUE)
  		{
  			/* A character was retrieved from the queue so can be sent to the
  			 THR now. */
  			UART_REG8(UART_TX) = cChar;
  		}
  		else
  		{
  			/* Queue empty, nothing to send so turn off the Tx interrupt. */
        UART_REG8(UART_IER) &= ~UART_IER_THRI ;

  		}
#endif
  	}

  	if ((nSource == UART_IIR_RDI) || (nSource == UART_IIR_TOI))
  	{
      //UartSendTrace("UART_IIR_RDI\n");
  		/* The interrupt was caused by the receiver getting data. */
  		cChar = uart_read_byte();

  		/* Because FreeRTOS is not supposed to run with nested interrupts, put all OS
  		calls in a critical section . */
  		/*
  		portENTER_CRITICAL();
  			xQueueSendFromISR(xRxedChars, &cChar, &xHigherPriorityTaskWoken);
  		portEXIT_CRITICAL();
  		*/
      if(xQueueIsQueueFullFromISR(xRxedChars) != pdTRUE)
      {
        UBaseType_t uxSavedInterruptStatus;
        uxSavedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();
        xQueueSendFromISR(xRxedChars, &cChar, &xHigherPriorityTaskWoken);
        portCLEAR_INTERRUPT_MASK_FROM_ISR(uxSavedInterruptStatus);
      }
  	}

    if(nSource == UART_IIR_MSI)
    {
      //UartSendTrace("UART_IIR_MSI\n");
      uart_clear_msr();
    }

    if(nSource == UART_IIR_RLSI)
    {
      //UartSendTrace("UART_IIR_RLSI\n");
      uart_clear_lsr();
    }

    nSource = uart_int_source();
  }

  /* Call the IntQ test function for this channel. */
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

}

xComPortHandle xSerialPortInitMinimal( unsigned long ulWantedBaud, unsigned portBASE_TYPE uxQueueLength )
{
xComPortHandle    xReturn = serHANDLE;
//int cd; /* USART Clock Divider. */

	/* Create the rx and tx queues. */
	vprvSerialCreateQueues( uxQueueLength, &xRxedChars, &xCharsForTx );

	/* Configure USART. */
	if( ( xRxedChars != serINVALID_QUEUE ) &&
	  ( xCharsForTx != serINVALID_QUEUE ) &&
	  ( ulWantedBaud != ( unsigned long ) 0 ) )
	{
		portENTER_CRITICAL();
		{
      //uart_init();
      uart_int_enable();
      U8 uartId = uart_id();

//      if (!uart_id())
//      {
//        MOS_IRQn_AttachIsr(NVIC_Handler_UART, UART0_IRQn, UART_INT_PRIORITY);
//        MOS_IRQn_Unmask(UART0_IRQn);
//      }
//      else
//      {
//        MOS_IRQn_AttachIsr(NVIC_Handler_UART, UART1_IRQn, UART_INT_PRIORITY);
//        MOS_IRQn_Unmask(UART1_IRQn);
//      }

      if (uartId == 0)
      {
        MOS_IRQn_AttachIsr(NVIC_Handler_UART, UART0_IRQn, UART_INT_PRIORITY);
        MOS_IRQn_Unmask(UART0_IRQn);
      }
      else if (uartId == 2)
      {
        MOS_IRQn_AttachIsr(NVIC_Handler_UART, UART2_IRQn, UART_INT_PRIORITY);
        MOS_IRQn_Unmask(UART2_IRQn);
      }
      else
      {
        MOS_IRQn_AttachIsr(NVIC_Handler_UART, UART1_IRQn, UART_INT_PRIORITY);
        MOS_IRQn_Unmask(UART1_IRQn);
      }


/*
extern unsigned long __Vectors[];

      __Vectors[UART0_IRQn + 16] = (unsigned long)NVIC_Handler_UART0;

      NVIC_SetPriority(UART0_IRQn, UART0_INT_PRIORITY);
      NVIC_ClearPendingIRQ(UART0_IRQn);
      NVIC_EnableIRQ(UART0_IRQn);
*/
		}
		portEXIT_CRITICAL();
	}
	else
	{
		xReturn = serINVALID_COMPORT_HANDLER;
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

signed portBASE_TYPE xSerialGetChar( xComPortHandle pxPort, signed char *pcRxedChar, TickType_t xBlockTime )
{
	/* The port handle is not required as this driver only supports UART0. */
	( void ) pxPort;

	/* Get the next character from the buffer.  Return false if no characters
	are available, or arrive before xBlockTime expires. */
	if( xQueueReceive( xRxedChars, pcRxedChar, xBlockTime ) )
	{
		return pdTRUE;
	}
	else
	{
		return pdFALSE;
	}
}
/*-----------------------------------------------------------*/

void vSerialPutString( xComPortHandle pxPort, const signed char * const pcString, unsigned short usStringLength )
{
signed char *pxNext;

	/* NOTE: This implementation does not handle the queue being full as no
	block time is used! */

	/* The port handle is not required as this driver only supports UART0. */
	( void ) pxPort;
	( void ) usStringLength;

	/* Send each character in the string, one at a time. */
	pxNext = ( signed char * ) pcString;
	while( *pxNext )
	{
		xSerialPutChar( pxPort, *pxNext, serNO_BLOCK );
		pxNext++;
	}
}
/*-----------------------------------------------------------*/

signed portBASE_TYPE xSerialPutChar( xComPortHandle pxPort, signed char cOutChar, TickType_t xBlockTime )
{
	( void ) pxPort;
	( void ) xBlockTime;

  uart_write_byte(cOutChar);

#if 0
	/* Place the character in the queue of characters to be transmitted. */
	if( xQueueSend( xCharsForTx, &cOutChar, xBlockTime ) != pdPASS )
	{
		return pdFAIL;
	}

	/* Turn on the Tx interrupt so the ISR will remove the character from the
	queue and send it.   This does not need to be in a critical section as
	if the interrupt has already removed the character the next interrupt
	will simply turn off the Tx interrupt again. */
  UART_REG8(UART_IER) |= UART_IER_THRI ;
#endif

	return pdPASS;
}
/*-----------------------------------------------------------*/

void vSerialClose( xComPortHandle xPort )
{
	( void ) xPort;
  /* Not supported as not required by the demo application. */
}
/*-----------------------------------------------------------*/

/*###########################################################*/

/*
 * Create the rx and tx queues.
 */
static void vprvSerialCreateQueues(  unsigned portBASE_TYPE uxQueueLength, QueueHandle_t *pxRxedChars, QueueHandle_t *pxCharsForTx )
{
	/* Create the queues used to hold Rx and Tx characters. */
	xRxedChars = xQueueCreate( uxQueueLength, ( unsigned portBASE_TYPE ) sizeof( signed char ) );
	xCharsForTx = xQueueCreate( uxQueueLength + 1, ( unsigned portBASE_TYPE ) sizeof( signed char ) );

	/* Pass back a reference to the queues so the serial API file can
	post/receive characters. */
	*pxRxedChars = xRxedChars;
	*pxCharsForTx = xCharsForTx;
}
/*-----------------------------------------------------------*/


