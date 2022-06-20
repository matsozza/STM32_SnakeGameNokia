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
LCD_displayBuffer_t *LCD_displayBuffer01;

/* Functions implementation --------------------------------------------------*/

int LCD_SPI_sendByte(uint8_t data, uint8_t DC)
{
	// D/C -> Data mode
	HAL_GPIO_WritePin(SPI_LCD_DC_PORT, SPI_LCD_DC_PIN, DC);
	// Chip enable LCD -> Enable
	HAL_GPIO_WritePin(SPI_LCD_CE_PORT, SPI_LCD_CE_PIN, GPIO_PIN_RESET);

	// Prepare command
	uint8_t* pData = malloc(sizeof(uint8_t));
	*pData = data;
	
	// Transmit command to SPI
	#if(SPI_USE_IT == 0)
		HAL_SPI_Transmit(&SPI_LCD_HANDLE, pData, 1, osWaitForever);
	#else
		HAL_SPI_Transmit_IT(&SPI_LCD_HANDLE, pData, 1);
	#endif

	free(pData);

	//Chip enable LCD -> Disable
	HAL_GPIO_WritePin(SPI_LCD_CE_PORT, SPI_LCD_CE_PIN, GPIO_PIN_SET);

	return 0;
}

int LCD_SPI_Cmd_setRowGroupIdx(uint8_t rowGroupIdx, uint8_t skipBasicIS)
{
	if (!skipBasicIS)
	{
		LCD_SPI_sendCommand(0x20); //Set basic instruction set (BasicIS)
	}
	LCD_SPI_sendCommand(0x40 + rowGroupIdx);
	return 0;
}

int LCD_SPI_Cmd_setColIdx(uint8_t colIdx, uint8_t skipBasicIS)
{
	if (!skipBasicIS)
	{
		LCD_SPI_sendCommand(0x20); // Set basic instruction set (BasicIS)
	}
	LCD_SPI_sendCommand(0x80 + colIdx);
	return 0;
}

LCD_displayBuffer_t* LCD_SPI_Cmd_initDisplay(LCD_displayBuffer_t *LCD_displayBuffer)
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
	LCD_displayBuffer01 = calloc(1, sizeof(LCD_displayBuffer_t));

	// Set all update flags to perform a first clear of the display
	for (uint8_t idx = 0; idx < 84;idx++)
	{
		LCD_displayBuffer01->updateStatus[idx] = 0x3F;
	}

	return LCD_displayBuffer01;
}

int LCD_Queue_addByte(uint8_t data, uint8_t DC)
{
	// Add to queue
	LCD_dataPackage_t *LCD_dataPackage;
	LCD_dataPackage = osPoolAlloc(mPoolLCDHandle);		 // alloc mempos
	LCD_dataPackage->dataByte = data; // set value
	LCD_dataPackage->DC = DC;			 // set value
	osMessagePut(queueLCDHandle, (uint32_t)LCD_dataPackage, 0);
	
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
		if(evt.status == osEventMessage)
		{
			LCD_dataPackage_t *LCD_dataPackage =  evt.value.p;
			osPoolFree(mPoolLCDHandle, LCD_dataPackage);
			LCD_SPI_sendByte(LCD_dataPackage->dataByte, LCD_dataPackage->DC);
		}
	}
	return 0;
}

int LCD_Queue_Cmd_setRowGroupIdx(uint8_t rowGroupIdx, uint8_t skipBasicIS)
{
	if (!skipBasicIS)
	{
		LCD_Queue_addCommand(0x20);
	}
	LCD_Queue_addCommand(0x40 + rowGroupIdx);
	return 0;
}

int LCD_Queue_Cmd_setColIdx(uint8_t colIdx, uint8_t skipBasicIS)
{
	if (!skipBasicIS)
	{
		LCD_Queue_addCommand(0x20);
	}
	LCD_Queue_addCommand(0x80 + colIdx);
	return 0;
}

int LCD_Buffer_setPixel(LCD_displayBuffer_t *LCD_displayBuffer, uint8_t rowIdx, uint8_t colIdx, uint8_t val)
{
	uint8_t rowGroupIdx = (uint8_t)rowIdx / 8; // 'major' row
	uint8_t rowPixelIdx = (uint8_t)rowIdx % 8; // 'minor' row

	// Check if there's any change in the value before setting / clearing
	uint8_t currVal = (LCD_displayBuffer->displayMatrix[rowGroupIdx][colIdx] >> (rowPixelIdx)) & (0b1);

	if (currVal != val)
	{
		// Set or clear value in the 'displayMatrix' LCD Buffer
		if (val)
		{
			LCD_displayBuffer->displayMatrix[rowGroupIdx][colIdx] |= (1 << (rowPixelIdx));
		}
		else
		{
			LCD_displayBuffer->displayMatrix[rowGroupIdx][colIdx] &= ~((uint8_t)(1 << (rowPixelIdx)));
		}

		// Set the 'updateStatus' position
		LCD_Buffer_setUpdateStatus(LCD_displayBuffer, rowGroupIdx, colIdx, 1); 
	}

	return 0;
}

int LCD_Buffer_getPixel(LCD_displayBuffer_t *LCD_displayBuffer, uint8_t rowIdx, uint8_t colIdx, uint8_t clearUpdtFlg)
{
	uint8_t rowGroupIdx = (uint8_t)rowIdx / 8; // 'major' row
	uint8_t rowPixelIdx = (uint8_t)rowIdx % 8; // 'minor' row

	// If 'clearUpdtFlg' is true, clear the read/update flag + return value
	if (clearUpdtFlg)
	{
		uint8_t rowGroupIdx = rowIdx / 8;
		LCD_Buffer_setUpdateStatus(LCD_displayBuffer, rowGroupIdx, colIdx, 0); //Clear updt Flg
	}

	return (int)((LCD_displayBuffer->displayMatrix[rowGroupIdx][colIdx] & (1 << (rowPixelIdx)))>0); // Return value
}

int LCD_Buffer_setByte(LCD_displayBuffer_t *LCD_displayBuffer, uint8_t rowGroupIdx, uint8_t colIdx, uint8_t val)
{
	// Check if there's any change in the value before copying new value
	uint8_t currVal = LCD_displayBuffer->displayMatrix[rowGroupIdx][colIdx];
	if (currVal != val)
	{
		// Copy byte to the 'displayMatrix' LCD Buffer
		LCD_displayBuffer->displayMatrix[rowGroupIdx][colIdx] = val;

		// Set the 'updateStatus' position
		LCD_Buffer_setUpdateStatus(LCD_displayBuffer, rowGroupIdx, colIdx, 1);
	}

	return 0;
}

int LCD_Buffer_getByte(LCD_displayBuffer_t *LCD_displayBuffer, uint8_t rowGroupIdx, uint8_t colIdx, uint8_t clearUpdtFlg)
{
	// If 'clearUpdtFlg' is true, clear the read/update flag + return value
	if (clearUpdtFlg)
	{
		LCD_Buffer_setUpdateStatus(LCD_displayBuffer, rowGroupIdx, colIdx, 0); //Clear updt Flg
		return LCD_displayBuffer->displayMatrix[rowGroupIdx][colIdx]; // Return value
	}
	else
	{
		return LCD_displayBuffer->displayMatrix[rowGroupIdx][colIdx]; // Just return data at the req. position ('peek')
	}
}

int LCD_Buffer_setUpdateStatus(LCD_displayBuffer_t *LCD_displayBuffer, uint8_t rowGroupIdx, uint8_t colIdx, uint8_t val)
{
	if(val) // Set
	{
		LCD_displayBuffer->updateStatus[colIdx] |= (1 << (rowGroupIdx));
	}
	else // Clear
	{
		LCD_displayBuffer->updateStatus[colIdx] &= ~((uint8_t)(1 << (rowGroupIdx)));
	}

	return 0;
}

int LCD_Buffer_getUpdateStatus(LCD_displayBuffer_t *LCD_displayBuffer, uint8_t rowGroupIdx, uint8_t colIdx){
	return ((LCD_displayBuffer->updateStatus[colIdx] >> (rowGroupIdx)) && 0b1);
}

// TODO eliminate buffer 'cursor' operations, make atomic functions
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
	// Get char from library
	LCD_Char_t LCD_CharSel = findCorrespondingChar(ASCII_char); // Find ASCII in library

	// Get current buffer cursor to start the char
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
			uint8_t bitValue = (uint8_t)((LCD_CharSel.bitmap[colIdx] >> (rowIdx)) & 0b1);
			LCD_Buffer_setPixel(LCD_displayBuffer, currRow + rowIdx, currCol + colIdx, bitValue);
		}
	}

	return 0;
}

int LCD_Buffer_sendToQueue(LCD_displayBuffer_t *LCD_displayBuffer)
{
	// TODO If queue is full, wait / abort (otherwise LCD Glitch is expected)
	// Count the number of bytes in the LCD that need to be updated
	uint16_t updateCounter = 0;
	uint16_t consecCounter = 0;
	for (uint8_t colIdx = 0; colIdx < 84; colIdx++)
	{
		for (uint8_t rowGroupIdx = 0; rowGroupIdx < 8; rowGroupIdx++)
		{
			if ((LCD_displayBuffer->updateStatus[colIdx] >> rowGroupIdx) & 0b01)
			{
				updateCounter++; // Number of new values (bytes) to be updated
				
				if (colIdx<83 && (LCD_displayBuffer->updateStatus[colIdx+1] >> rowGroupIdx) & 0b01)
				{
					consecCounter++; //Consecutive columns (bytes) being updated
				}
			}
		}
	}

	// If less than 25% of the LCD needs to be updated, perform specific refreshes.
	// Otherwise, perform complete refresh.
	if(updateCounter - (3*consecCounter) < 84*6/3)
	{
		uint8_t updateConsecutive = 0;
		// Send buffer bytes to LCD - 6x84 sets of 1 byte of type 'D - data'
		for (uint8_t rowGroupIdx = 0; rowGroupIdx < 6; rowGroupIdx++) // LCD Rows: 6*8(byte)= 48 pixels
		{
			for (uint8_t colIdx = 0; colIdx < 84; colIdx++) // LCD Cols: 84 pixels
			{
				if (!LCD_Buffer_getUpdateStatus(LCD_displayBuffer,rowGroupIdx,colIdx)) // If new data IS NOT AVAILABLE at this position
				{
					updateConsecutive = 0;
				}
				else // If new data IS AVAILABLE in this position
				{
					uint8_t bufferByte = LCD_Buffer_getByte(LCD_displayBuffer, rowGroupIdx, colIdx, 1);
					if(!updateConsecutive)
					{
						LCD_Queue_Cmd_setRowGroupIdx(rowGroupIdx,0);
						LCD_Queue_Cmd_setColIdx(colIdx,1);
					}
					LCD_Queue_addData(bufferByte);
					updateConsecutive = 1;
				}
			}
		}
	}
	else
	{
		// Set display cursors to initial position - complete refresh
		LCD_Queue_Cmd_setRowGroupIdx(0,0);
		LCD_Queue_Cmd_setColIdx(0,1);

		// Send buffer bytes to LCD - 6x84 sets of 1 byte of type 'D - data'
		for (uint8_t rowGroupIdx = 0; rowGroupIdx < 6; rowGroupIdx++) // LCD Rows: 6*8(byte)= 48 pixels
		{
			for (uint8_t colIdx = 0; colIdx < 84; colIdx++) // LCD Cols: 84 pixels
			{
				uint8_t bufferByte = LCD_Buffer_getByte(LCD_displayBuffer, rowGroupIdx, colIdx, 1);
				LCD_Queue_addData(bufferByte);
			}
		}
	}

	return 0;
}


