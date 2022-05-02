/**
 * UART High Level Services
 * service_uart.h
 *
 *  Created on: Apr 21, 2022
 *      Author: Matheus Sozza
 */

#ifndef INC_CORE_HL_SERVICES_UART_HL_SERVICES_H_
#define INC_CORE_HL_SERVICES_UART_HL_SERVICES_H_

#include "cmsis_os.h"

// ** Typedef declares **
typedef struct{
	char message[10];
} USART_message_t;

// ** Function headers declare **
extern int USART2_sendString(char *msg);

#endif /* INC_CORE_HL_SERVICES_UART_HL_SERVICES_H_ */




