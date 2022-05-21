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

/* Typedef declare -----------------------------------------------------------*/
typedef struct{
	uint8_t rowIdx;
	uint8_t colIdx;
	uint8_t displayMatrix[6][84];
} LCD_displayBuffer_t;

/* Function prototypes -------------------------------------------------------*/

// ***** LCD_SPI - SPI to LCD communication + macros *****
int LCD_SPI_sendByte(uint8_t data, uint8_t DC);
#define LCD_SPI_sendCommand(cmd) LCD_SPI_sendByte(cmd, GPIO_PIN_RESET);
#define LCD_SPI_sendData(cmd) LCD_SPI_sendByte(cmd, GPIO_PIN_SET);

// ***** LCD_Command - LCD controller commands and start routine *****
int LCD_Command_setRowIdx(uint8_t rowIdx);
int LCD_Command_setColIdx(uint8_t colIdx);
int LCD_Command_initializeConfigs();

// ***** LCD_Buffer - Buffer manipulation and high level interface with LCD  *****
int LCD_Buffer_setValue(LCD_displayBuffer_t *LCD_displayBuffer, uint8_t rowIdx, uint8_t colIdx, uint8_t val);
int LCD_Buffer_getValue(LCD_displayBuffer_t *LCD_displayBuffer, uint8_t rowIdx, uint8_t colIdx);

int LCD_Buffer_setCursor(LCD_displayBuffer_t *LCD_displayBuffer, uint8_t rowIdx, uint8_t colIdx);

int LCD_Buffer_getCursorRow(LCD_displayBuffer_t *LCD_displayBuffer);
int LCD_Buffer_getCursorCol(LCD_displayBuffer_t *LCD_displayBuffer);

int LCD_Buffer_sendToDisplay(LCD_displayBuffer_t *LCD_displayBuffer);

int LCD_Buffer_writeASCIIChar(LCD_displayBuffer_t *LCD_displayBuffer, char ASCII_char);

#endif /* INC_CORE_SERVICES_LCD_H_ */




