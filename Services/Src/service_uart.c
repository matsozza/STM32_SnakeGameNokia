/*
 * UART High Level Services
 * service_uart.c
 *
 *  Created on: Apr 21, 2022
 *      Author: Matheus Sozza
 */

#include "service_uart.h"
#include "usart.h"
#include <string.h>
#include <stdlib.h>

#define UART_USE_IT true

int USART2_sendString(char *msg)
{
	//Check for busy state
	if(huart2.gState == HAL_UART_STATE_READY){
		//Allocate memory for string preparation
		char *preparedStr = malloc(sizeof(char)* (strlen(msg) + 32));

		//Prepare a string with a line break + carriage return
		strcpy(preparedStr, msg);
		strcat(preparedStr, "\n\r");

		// Transmit prepared message to USART2
#if !UART_USE_IT
		HAL_StatusTypeDef txStatus = HAL_UART_Transmit( &huart2,
														(uint8_t*) preparedStr,
														strlen(preparedStr),
														5000);
#else
		HAL_StatusTypeDef txStatus = HAL_UART_Transmit_IT( &huart2,
												(uint8_t*) preparedStr,
														strlen(preparedStr));
#endif

		free(preparedStr);
		if(txStatus == HAL_OK) { return 0; }
		else{ return -1; }
	}
	else{
		return -1;
	}
}
