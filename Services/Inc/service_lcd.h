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
#define SPI_USE_IT 			0 //Define to use interruptions or not in SPI interf.

#define SPI_LCD_HANDLE		hspi1
#define SPI_LCD_DC_PIN 		LCD_DC_Pin
#define SPI_LCD_CE_PIN 		LCD_CE_Pin
#define SPI_LCD_DC_PORT		LCD_CE_GPIO_Port
#define SPI_LCD_CE_PORT		LCD_CE_GPIO_Port


/* Typedef declare -----------------------------------------------------------*/
typedef struct{
	uint8_t rowIdx;
	uint8_t colIdx;
	uint8_t displayMatrix[6][84];
	uint8_t updateStatus[84];
} LCD_displayBuffer_t;

typedef struct{
	uint8_t DC;
	uint8_t dataByte;
} LCD_dataPackage_t;

/* Function prototypes -------------------------------------------------------*/

// ***** LCD_SPI - SPI to LCD (direct communication) + macros *****
int LCD_SPI_sendByte(uint8_t data, uint8_t DC);
#define LCD_SPI_sendCommand(cmd) LCD_SPI_sendByte(cmd, 0);
#define LCD_SPI_sendData(cmd) LCD_SPI_sendByte(cmd, 1);

// ***** LCD_DirCmd - LCD controller commands (direct communicaiton) and start routine *****
int LCD_DirCmd_setRowIdx(uint8_t rowIdx);
int LCD_DirCmd_setColIdx(uint8_t colIdx);
int LCD_DirCmd_initDisplay();

// ***** LCD_Queue - MCU to LCD Queue (indirect communication)
int LCD_Queue_addByte(uint8_t data, uint8_t DC);
#define LCD_Queue_addCommand(cmd) LCD_Queue_addByte(cmd, 0);
#define LCD_Queue_addData(cmd) LCD_Queue_addByte(cmd, 1);
int LCD_Queue_consumeBytes();

// ***** LCD_IndCmd = MCU to Queue commands (indirect communication)
int LCD_IndCmd_setRowIdx(uint8_t rowIdx);
int LCD_IndCmd_setColIdx(uint8_t colIdx);

// ***** LCD_Buffer - Buffer manipulation and high level interface with LCD  *****
int LCD_Buffer_setValue(LCD_displayBuffer_t *LCD_displayBuffer, uint8_t rowIdx, uint8_t colIdx, uint8_t val);
int LCD_Buffer_getValue(LCD_displayBuffer_t *LCD_displayBuffer, uint8_t rowIdx, uint8_t colIdx, uint8_t clearUpdtFlg);

int LCD_Buffer_setCursor(LCD_displayBuffer_t *LCD_displayBuffer, uint8_t rowIdx, uint8_t colIdx);

int LCD_Buffer_getCursorRow(LCD_displayBuffer_t *LCD_displayBuffer);
int LCD_Buffer_getCursorCol(LCD_displayBuffer_t *LCD_displayBuffer);

int LCD_Buffer_writeASCIIChar(LCD_displayBuffer_t *LCD_displayBuffer, char ASCII_char);

int LCD_Buffer_sendToQueue(LCD_displayBuffer_t *LCD_displayBuffer);

#endif /* INC_CORE_SERVICES_LCD_H_ */




