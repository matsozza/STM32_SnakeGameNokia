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
extern osMessageQId queueLCDHandle;
extern osPoolId mPoolLCDHandle;

/* Internal variables includes -----------------------------------------------*/
//add LCD queue
LCD_displayBuffer_t *LCD_displayCtrl;
LCD_dataPackage_t *LCD_dataPack2Queue;

/* Functions implementation --------------------------------------------------*/

int LCD_SPI_sendByte(uint8_t data, uint8_t DC)
{
	// D/C -> Data mode
	HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, DC);
	// Chip enable LCD -> Enable
	HAL_GPIO_WritePin(LCD_CE_GPIO_Port, LCD_CE_Pin, GPIO_PIN_RESET);

	// Prepare command
	uint8_t* pData = malloc(sizeof(uint8_t));
	*pData = data;
	
	// Transmit command to SPI
	#if(SPI_USE_IT == 0)
		HAL_SPI_Transmit(&hspi1, pData, 1, osWaitForever);
	#else
		HAL_SPI_Transmit_IT(&hspi1, pData, 1);
	#endif

	free(pData);

	//Chip enable LCD -> Disable
	HAL_GPIO_WritePin(LCD_CE_GPIO_Port, LCD_CE_Pin, GPIO_PIN_SET);

	return 0;
}

int LCD_DirCmd_setRowIdx(uint8_t rowIdx)
{
	LCD_SPI_sendCommand(0x20);
	LCD_SPI_sendCommand(0x80 + rowIdx);
	return 0;
}

int LCD_DirCmd_setColIdx(uint8_t colIdx)
{
	LCD_SPI_sendCommand(0x20);
	LCD_SPI_sendCommand(0x40 + colIdx);
	return 0;
}

int LCD_DirCmd_initDisplay()
{
	// Toggle RESET Pin - Clear LCD memory
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

	// Memory allocation for display buffer
	LCD_displayCtrl = calloc(1, sizeof(LCD_displayBuffer_t));

	return 0;
}

int LCD_Queue_addByte(uint8_t data, uint8_t DC)
{
	// Add to queue
	LCD_dataPack2Queue = osPoolAlloc(mPoolLCDHandle);		 // alloc mempos
	LCD_dataPack2Queue->dataByte = data; // set value
	LCD_dataPack2Queue->DC = DC;			 // set value
	osMessagePut(queueLCDHandle, (uint32_t)LCD_dataPack2Queue, 0);
	
	return 0;
}

int LCD_Queue_consumeBytes()
{
	//Check if SPI is not busy before attempt to consume from queue
	if(hspi1.State != HAL_SPI_STATE_BUSY_TX || hspi1.State != HAL_SPI_STATE_BUSY_TX_RX)
	{
		// Check queue and consume if available
		osEvent evt = osMessageGet(queueLCDHandle, osWaitForever);

		//If data was found in queue, proceed sending to SPI
		if(evt.status == osEventMessage){
			LCD_dataPackage_t *LCD_dataPackage =  evt.value.p;
			osPoolFree(mPoolLCDHandle, LCD_dataPackage);
			LCD_SPI_sendByte(LCD_dataPackage->dataByte, LCD_dataPackage->DC);
		}
	}
	return 0;
}

int LCD_IndCmd_setRowIdx(uint8_t rowIdx)
{
	LCD_Queue_addCommand(0x20);
	LCD_Queue_addCommand(0x80 + rowIdx);
	return 0;
}

int LCD_IndCmd_setColIdx(uint8_t colIdx)
{
	LCD_Queue_addCommand(0x20);
	LCD_Queue_addCommand(0x40 + colIdx);
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
	return 0;
}

int LCD_Buffer_getCursorRow(LCD_displayBuffer_t *LCD_displayBuffer)
{
	return LCD_displayBuffer->rowIdx;
}

int LCD_Buffer_getCursorCol(LCD_displayBuffer_t *LCD_displayBuffer)
{
	return LCD_displayBuffer->colIdx;
}

int LCD_Buffer_sendToQueue(LCD_displayBuffer_t *LCD_displayBuffer)
{
	// Set display cursors to initial position
	LCD_IndCmd_setRowIdx(0);
	LCD_IndCmd_setColIdx(0);

	// Send buffer bytes to LCD - 6x84 sets of 1 byte of type 'D - data'
	for (uint8_t rowIdx = 0; rowIdx < 6; rowIdx++) // LCD Rows: 6*8(byte)= 48 pixels
	{
		for (uint8_t colIdx = 0; colIdx < 84; colIdx++) // LCD Cols: 84 pixels
		{
			//LCD_SPI_sendData(LCD_displayBuffer->displayMatrix[rowIdx][colIdx]);
			LCD_Queue_addData(LCD_displayBuffer->displayMatrix[rowIdx][colIdx]);
		}
	}

	return 0;
}

/**
 * @brief Write an ASCII char to a pre-defined position of the
 * LCD display.
 * 
 * @param LCD_displayBuffer
 * @param ASCII_char
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