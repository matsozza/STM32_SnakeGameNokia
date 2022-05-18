/**
 * LCD High Level Services
 * service_lcd.h
 *
 * Designed to work with NOKIA 5110 display via SPI
 *
 *  Created on: May 05, 2022
 *      Author: Matheus Sozza
 */

#ifndef INC_CORE_SERVICES_LCD_H_
#define INC_CORE_SERVICES_LCD_H_

/* Includes ------------------------------------------------------------------*/
#include "cmsis_os.h"

/* Defines -------------------------------------------------------------------*/
#define SPI_USE_IT 1

#define LCD_SPI_sendCommand(cmd)	LCD_SPI_sendByte(cmd,GPIO_PIN_RESET);
#define LCD_SPI_sendData(cmd)		LCD_SPI_sendByte(cmd,GPIO_PIN_SET);

// ******** Functions / macros to handle directly the LCD ********
// Define 'X' cursor position in the LCD (hardware)
#define LCD_SPI_setCursorX(pos) 			\
	{										\
		LCD_SPI_sendCommand(0x20);  		\
		LCD_SPI_sendCommand(0x80 + pos); 	\
	}
// Define 'Y' cursor position in the LCD (hardware)
#define LCD_SPI_setCursorY(pos)    			\
	{                          				\
		LCD_SPI_sendCommand(0x20); 			\
		LCD_SPI_sendCommand(0x40 + pos);	\
	}


//  ******** Functions / macros to handle the buffer (LCD_displayBuffer_t) ********
#define LCD_setBufferCursorX(pos) LCD_displayCtrl->cursorX = pos;
#define LCD_setBufferCursorY(pos) LCD_displayCtrl->cursorY = pos;
#define LCD_setBufferValue(posX,posY,val) LCD_displayCtrl->displayMatrix[posX][posY] = val;
#define LCD_getBufferValue(posX,posY) return LCD_displayCtrl->displayMatrix[posX][posY];

/* Typedef declare -----------------------------------------------------------*/
typedef struct{
	uint8_t cursorX;
	uint8_t cursorY;
	uint8_t displayMatrix[48][84];
} LCD_displayBuffer_t;

/* Function prototypes -------------------------------------------------------*/
//Level 0 - SPI communication & startup
int LCD_SPI_sendByte(uint8_t data, uint8_t DC);
int LCD_initializeConfigs();

//Level 1 - Graph manipulation & ASCII Chars
int LCD_writeBufferASCIIChar(char ASCII_char);

#endif /* INC_CORE_SERVICES_LCD_H_ */




