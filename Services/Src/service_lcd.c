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
#include "service_lcd_fonts.h"
#include "service_lcd.h"
#include "spi.h"
#include "freertos.h"
#include <string.h>
#include <stdlib.h>

/* External variables includes -----------------------------------------------*/


/* Internal variables includes -----------------------------------------------*/
//add LCD queue
LCD_displayBuffer_t *LCD_displayCtrl;

/* Functions implementation --------------------------------------------------*/

int LCD_SPI_sendByte(uint8_t data, uint8_t DC){
	// D/C -> Data mode
	HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, DC);
	// Chip enable LCD -> Enable
	HAL_GPIO_WritePin(LCD_CE_GPIO_Port, LCD_CE_Pin, GPIO_PIN_RESET);

	// Prepare and transmit command
	uint8_t* pData = malloc(sizeof(uint8_t));
	*pData = data;
	HAL_SPI_Transmit(&hspi1, pData, 1, osWaitForever);
	free(pData);

	//Chip enable LCD -> Disable
	HAL_GPIO_WritePin(LCD_CE_GPIO_Port, LCD_CE_Pin, GPIO_PIN_SET);

	return 0;
}

int LCD_Command_setRowIdx(uint8_t rowIdx)           
{                                    
	LCD_SPI_sendCommand(0x20);       
	LCD_SPI_sendCommand(0x80 + rowIdx);
	return 0;
}

int LCD_Command_setColIdx(uint8_t colIdx)           
{                                    
	LCD_SPI_sendCommand(0x20);       
	LCD_SPI_sendCommand(0x40 + colIdx);
	return 0;
}
	
int LCD_initializeConfigs()
{
	//Toggle RESET Pin - Clear LCD memory
	HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET);

	LCD_SPI_sendCommand(0x21); // Function set - Extended
	LCD_SPI_sendCommand(0xB9); // VOP - Contrast set
	LCD_SPI_sendCommand(0x04); // Temp. coefficient
	LCD_SPI_sendCommand(0x14); // Bias system

	LCD_SPI_sendCommand(0x20); // Function set - Basic
	LCD_SPI_sendCommand(0x0C); // Display config. - Normal Mode
	LCD_SPI_sendCommand(0x40); // Addressing - Y = 0
	LCD_SPI_sendCommand(0x80); // Addressing - X = 0

	//Memory allocation for display buffer (@TODO - optimize using boolean)
	LCD_displayCtrl = calloc(1,sizeof(LCD_displayBuffer_t));

	//Test - write 2 squares
	for (int index = 0; index < (84*48) / 8; index++)
	{
		LCD_SPI_sendData(0xF0);
	}

	LCD_Buffer_writeASCIIChar(LCD_displayCtrl ,'h');
	LCD_Buffer_setCursor(LCD_displayCtrl, 0, 7);
	LCD_Buffer_writeASCIIChar(LCD_displayCtrl, 'e');
	LCD_Buffer_setCursor(LCD_displayCtrl, 0, 14);
	LCD_Buffer_writeASCIIChar(LCD_displayCtrl, 'l');
	LCD_Buffer_setCursor(LCD_displayCtrl, 0, 21);
	LCD_Buffer_writeASCIIChar(LCD_displayCtrl, 'l');
	LCD_Buffer_setCursor(LCD_displayCtrl, 0, 28);
	LCD_Buffer_writeASCIIChar(LCD_displayCtrl, 'o');
	LCD_Buffer_sendToDisplay(LCD_displayCtrl);

	return 0;
}

int LCD_Buffer_setValue(LCD_displayBuffer_t *LCD_displayBuffer, uint8_t rowIdx, uint8_t colIdx, uint8_t val)
{
	uint8_t rowDisplayIdx = (uint8_t)rowIdx / 8;
	uint8_t rowByteIdx = (uint8_t)rowIdx % 8;

	if(val)
	{
		LCD_displayBuffer->displayMatrix[rowDisplayIdx][colIdx] |= (1 << (7 - rowByteIdx));
	}
	else
	{
		LCD_displayBuffer->displayMatrix[rowDisplayIdx][colIdx] &= ~((uint8_t)(1 << (7 - rowByteIdx)));
	}

	return 0;
}

int LCD_Buffer_setCursor(LCD_displayBuffer_t *LCD_displayBuffer, uint8_t rowIdx, uint8_t colIdx)
{
	LCD_displayBuffer->rowIdx = rowIdx;
	LCD_displayBuffer->colIdx = colIdx;
}

int LCD_Buffer_getCursorRow(LCD_displayBuffer_t *LCD_displayBuffer)
{
	return LCD_displayBuffer->rowIdx;
}

int LCD_Buffer_getCursorCol(LCD_displayBuffer_t *LCD_displayBuffer)
{
	return LCD_displayBuffer->colIdx;
}

int LCD_Buffer_sendToDisplay(LCD_displayBuffer_t *LCD_displayBuffer)
{
	// Set display cursors to initial position
	LCD_Command_setRowIdx(0);
	LCD_Command_setColIdx(0);

	// Send bytes to LCD - 6x84 sets of 1byte
	for (uint8_t rowIdx = 0; rowIdx < 6; rowIdx++) // LCD Rows: 6*8(byte)= 48 pixels
	{
		for (uint8_t colIdx = 0; colIdx < 84; colIdx++) // LCD Cols: 84 pixels
		{
			LCD_SPI_sendData(LCD_displayBuffer->displayMatrix[rowIdx][colIdx]);
		}
	}

	return 0;
}

/**
 * @brief Write an ASCII char to a pre-defined position of the
 * LCD display, and increments automatically the position and line breaks
 * 
 * @param ASCII_char 
 * @param posX 
 * @param posY 
 * @return int 
 */
int LCD_Buffer_writeASCIIChar(LCD_displayBuffer_t *LCD_displayBuffer, char ASCII_char)
{
	//Get char from library
	LCD_Char_t LCD_CharSel = findCorrespondingChar(ASCII_char); // Find ASCII in library

	//Get current buffer cursor to start the char
	uint8_t currRow = LCD_Buffer_getCursorRow(LCD_displayBuffer);
	uint8_t currCol = LCD_Buffer_getCursorCol(LCD_displayBuffer);

	// Write ASCII to LCD Buffer
	// ASCII char width is 'bitmap_total_bytes'
	uint8_t bitmapWidth = LCD_CharSel.bitmap_total_bytes;
	for (int colIdx = 0; colIdx < bitmapWidth; colIdx++)
	{
		// ASCII lib. char height is 8
		for (int rowIdx = 0; rowIdx < 8; rowIdx++)
		{
			uint8_t bitValue = (uint8_t) ((LCD_CharSel.bitmap[colIdx] >> (7-rowIdx)) & 0b1);
			LCD_Buffer_setValue(LCD_displayBuffer, currRow + rowIdx, currCol + colIdx, bitValue);
		}
	}

	return 0;
}


