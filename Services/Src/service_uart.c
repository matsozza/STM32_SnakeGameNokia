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
extern osPoolId  mpool;

/* Internal variables includes -----------------------------------------------*/
USART_message_t *USART2_msg2Queue;

/* Functions implementation --------------------------------------------------*/
int USART2_sendString(char *msg)
{
	//Add to queue
	USART_message_t *a = (USART_message_t*) osPoolAlloc(mpool);
	strcpy(a->message, "test123");
	osMessagePut(queueUSART2Handle, (uint32_t) a, 0);

	//Consume from queue
	osEvent evt = osMessageGet(queueUSART2Handle, osWaitForever);

	if (evt.status == osEventMessage) {
			USART_message_t *message = (USART_message_t*) evt.value.p;
			osPoolFree(mpool, message);
		}

	// Transmit prepared message to USART2

#if(UART_USE_IT == 0)
	/*
	//Do not use UART interruption to send data (blocking)
	HAL_UART_Transmit( &huart2,
								(uint8_t*) preparedStr,
								strlen(preparedStr),
								5000);
								*/
#else
	/*
	//Use UART interruption to send data (not blocking)
	HAL_UART_Transmit_IT( &huart2,
								(uint8_t*) preparedStr,
								strlen(preparedStr));
								*/
#endif
	return 0;
}

int USART2_addToQueue(char *msg){
	//Add to queue
	USART2_msg2Queue = (USART_message_t*) osPoolAlloc(mpool);
	strcpy(USART2_msg2Queue->message, msg); // Random str
	osMessagePut(queueUSART2Handle, (uint32_t) USART2_msg2Queue, 0);

	return 0;
}

int USART2_printFromQueue(char *msg){

	return 0;
}
