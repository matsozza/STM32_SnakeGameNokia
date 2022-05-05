/*
 * LCD High Level Services
 * service_lcd.c
 *
 * Designed to work with NOKIA 5110 display via SPI
 *
 *  Created on: May 05, 2022
 *      Author: Matheus Sozza
 */

/* Private includes ----------------------------------------------------------*/
#include "service_lcd.h"
#include "spi.h"
#include "freertos.h"
#include <string.h>
#include <stdlib.h>

/* External variables includes -----------------------------------------------*/
extern osMessageQId queueUSART2Handle;
extern osPoolId  mPoolUSART2Handle;

/* Internal variables includes -----------------------------------------------*/
//USART_message_t *USART2_msg2Queue;

/* Functions implementation --------------------------------------------------*/
int LCD_sendData(byte *data)
{
	return 0;
}

int LCD_sendCommand(byte *command)
{
	return 0;
}

