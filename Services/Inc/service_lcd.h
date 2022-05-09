/**
 * LCD High Level Services
 * service_lcd.h
 *
 * Designed to work with NOKIA 5110 display via SPI
 *
 *  Created on: May 05, 2022
 *      Author: Matheus Sozza
 */

#ifndef INC_CORE_SERVICES_LCD_SERVICES_H_
#define INC_CORE_SERVICES_LCD_SERVICES_H_

/* Includes ------------------------------------------------------------------*/
#include "cmsis_os.h"
#include "service_lcd_fonts.h"

/* Defines -------------------------------------------------------------------*/
#define SPI_USE_IT 1
#define LCD_sendCommand(cmd)	 LCD_sendByteSPI(cmd,GPIO_PIN_RESET);
#define LCD_sendData(cmd)		 LCD_sendByteSPI(cmd,GPIO_PIN_SET);

/* Typedef declare -----------------------------------------------------------*/
typedef struct{
	char message[16];
} USART_message_t;

/* Function prototypes -------------------------------------------------------*/
//Level 0 - SPI communication & startup
extern int LCD_sendByteSPI(uint8_t data, uint8_t DC);
extern int LCD_initializeConfigs();

//Level 1 - Graph manipulation & ASCII Chars
extern int LCD_writeASCIIChar(char ASCII_char);

#endif /* INC_CORE_SERVICES_LCD_SERVICES_H_ */




