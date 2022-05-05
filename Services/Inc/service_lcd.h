/**
 * LCD High Level Services
 * service_lcd.h
 *
 * Designed to work with NOKIA 5110 display via SPI
 *
 *  Created on: May 05, 2022
 *      Author: Matheus Sozza
 */

#ifndef INC_CORE_HL_SERVICES_UART_HL_SERVICES_H_
#define INC_CORE_HL_SERVICES_UART_HL_SERVICES_H_

/* Includes ------------------------------------------------------------------*/
#include "cmsis_os.h"


/* Defines -------------------------------------------------------------------*/
#define SPI_USE_IT 1


/* Typedef declare -----------------------------------------------------------*/
typedef struct{
	char message[16];
} USART_message_t;


/* Function prototypes -------------------------------------------------------*/
extern int LCD_sendData(byte* data);
extern int LCD_sendCommand(byte* command);
#endif /* INC_CORE_HL_SERVICES_UART_HL_SERVICES_H_ */




