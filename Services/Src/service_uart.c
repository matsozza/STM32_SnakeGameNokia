/*
 * UART High Level Services
 * service_uart.c
 *
 *  Created on: Apr 21, 2022
 *      Author: Matheus Sozza
 */

#include "service_uart.h"
#include "usart.h"
#include "freertos.h"
#include <string.h>
#include <stdlib.h>

#define UART_USE_IT 1

extern osMessageQId queueUSART2Handle;
USART_message_t USART2_queue;

int USART2_sendString(char *msg)
{

	//Allocate memory for string preparation
	char *preparedStr = malloc(sizeof(char)* (strlen(msg) + 32));

	//Prepare a string with a line break + carriage return
	strcpy(preparedStr, msg);
	strcat(preparedStr, "\n\r");

	//char *a;
	//a = malloc(sizeof(char));
	//*a = 42;

	USART_message_t *a;
	a = malloc(sizeof(USART_message_t));
	strcpy(a->message, "test123");
	//*a->message= "test123";

	osMessagePut(queueUSART2Handle, (uint32_t) a, 0);

	// Transmit prepared message to USART2
#if(UART_USE_IT == 0)
	//Do not use UART interruption to send data (blocking)
	HAL_UART_Transmit( &huart2,
								(uint8_t*) preparedStr,
								strlen(preparedStr),
								5000);
#else
	//Use UART interruption to send data (not blocking)
	HAL_UART_Transmit_IT( &huart2,
								(uint8_t*) preparedStr,
								strlen(preparedStr));
#endif
	free(preparedStr);

	return 0;
}
