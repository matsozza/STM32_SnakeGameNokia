/**
 * UART High Level Services
 * service_uart.h
 *
 *  Created on: Apr 21, 2022
 *      Author: Matheus Sozza
 */

#ifndef INC_CORE_SERVICES_UART_H_
#define INC_CORE_SERVICES_UART_H_

/* Includes ------------------------------------------------------------------*/
#include "cmsis_os.h"


/* Defines -------------------------------------------------------------------*/
#define UART_USE_IT 1


/* Typedef declare -----------------------------------------------------------*/
typedef struct{
	char message[16];
} USART_message_t;


/* Function prototypes -------------------------------------------------------*/
extern int USART2_addToQueue(char *msg);
extern int USART2_consumeFromQueue();

#endif /* INC_CORE_SERVICES_UART_H_ */




