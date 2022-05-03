/**
 * UART High Level Services
 * service_uart.h
 *
 *  Created on: Apr 21, 2022
 *      Author: Matheus Sozza
 */

#ifndef INC_CORE_HL_SERVICES_UART_HL_SERVICES_H_
#define INC_CORE_HL_SERVICES_UART_HL_SERVICES_H_

/* Includes ------------------------------------------------------------------*/
#include "cmsis_os.h"


/* Defines -------------------------------------------------------------------*/
#define UART_USE_IT 1


/* Typedef declare -----------------------------------------------------------*/
typedef struct{
	char message[10];
} USART_message_t;


/* Function prototypes -------------------------------------------------------*/
extern int USART2_sendString(char *msg);

#endif /* INC_CORE_HL_SERVICES_UART_HL_SERVICES_H_ */




