/*
 * UART High Level Services
 * service_uart.c
 *
 *  Created on: Apr 21, 2022
 *      Author: Matheus Sozza
 */

/* Private includes ----------------------------------------------------------*/
#include "service_uart.h"
#include "usart.h"
#include "freertos.h"
#include <string.h>
#include <stdlib.h>

/* External variables includes -----------------------------------------------*/
extern osMessageQId queueUSART2Handle;
extern osPoolId  mPoolUSART2Handle;

/* Internal variables includes -----------------------------------------------*/
USART_message_t *USART2_msg2Queue;

/* Functions implementation --------------------------------------------------*/
int USART2_addToQueue(char *msg){
	//Add to queue
	USART2_msg2Queue = (USART_message_t*) osPoolAlloc(mPoolUSART2Handle); // Allocate a memory pool with the size of the USART message
	strcpy(USART2_msg2Queue->message, msg); // Put the contents of the message into the allocated pool
	osMessagePut(queueUSART2Handle, (uint32_t) USART2_msg2Queue, 0); // Put the address of the pool into the queue

	return 0;
}

int USART2_consumeFromQueue(){
	//Check if USART is not busy before attempt to consume from queue
	if(huart2.gState != HAL_UART_STATE_BUSY_TX && huart2.gState != HAL_UART_STATE_BUSY_TX_RX ){
		//Check queue and consume if available
		osEvent evt = osMessageGet(queueUSART2Handle, osWaitForever);

		//If there's data to consume, proceed
		if (evt.status == osEventMessage)
		{
			USART_message_t *msgFromQueue = (USART_message_t*) evt.value.p;
			osPoolFree(mPoolUSART2Handle, msgFromQueue);

			// Transmit prepared message to USART2
			#if(UART_USE_IT == 0)	//Blocking mode (no interruption)
				HAL_UART_Transmit( &huart2,	(uint8_t*) msgFromQueue, strlen((char*)msgFromQueue), 5000);
			#else 					//Non-blocking mode (Interruption)
				HAL_UART_Transmit_IT(&huart2,(uint8_t*) msgFromQueue, strlen((char*)msgFromQueue));
			#endif
		}
	}

	return 0;
}
