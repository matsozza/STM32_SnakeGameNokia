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
// Level 0
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

/*
uint8_t LCD_getBufferValue(posX, posY)
{
	return LCD_displayCtrl->displayMatrix[posX][posY];
}

uint8_t LCD_setBufferValue(posX,posY,val)
{
	LCD_displayCtrl->displayMatrix[posX][posY] = val;
}*/

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

	/*
	LCD_writeBufferASCIIChar('H');
	LCD_writeBufferASCIIChar('e');
	LCD_writeBufferASCIIChar('l');
	LCD_writeBufferASCIIChar('l');
	LCD_writeBufferASCIIChar('o');
	LCD_writeBufferASCIIChar('W');
	LCD_writeBufferASCIIChar('o');
	LCD_writeBufferASCIIChar('r');
	LCD_writeBufferASCIIChar('l');
	LCD_writeBufferASCIIChar('d');
*/

	LCD_writeBufferASCIIChar('A');
	LCD_sendBufferToDisplay();

	//LCD_SPI_setCursorY(4);
	//LCD_writeBufferASCIIChar('X');
	//LCD_SPI_setCursorY(12);
	//LCD_writeBufferASCIIChar('X');

	return 0;
}

// Level 1

/**
 * @brief Write an ASCII char to a pre-defined position of the
 * LCD display, and increments automatically the position and line breaks
 * 
 * @param ASCII_char 
 * @param posX 
 * @param posY 
 * @return int 
 */
int LCD_writeBufferASCIIChar(char ASCII_char){
	//Get char from library
	LCD_Char_t LCD_CharSel = findCorrespondingChar(ASCII_char); // Find ASCII in library

	// Write ASCII to LCD Buffer
	// ASCII char width is 'bitmap_total_bytes'
	uint8_t bitmapWidth =  LCD_CharSel.bitmap_total_bytes;
	for (int colIdx = 0; colIdx < bitmapWidth; colIdx++)
	{
		// ASCII lib. char height is 8
		for (int rowIdx = 0; rowIdx < 8; rowIdx++)
		{
			uint8_t bitValue = (uint8_t) ((LCD_CharSel.bitmap[colIdx] >> (7-rowIdx)) & 0b1);
			LCD_setBufferValue(rowIdx, colIdx, bitValue);
		}
	}

	return 0;
}

int LCD_sendBufferToDisplay(){

	LCD_SPI_setCursorX(0);
	LCD_SPI_setCursorY(0);

	/**
	 * @brief The LCD contains 48 rows per 84 columns
	 * To write in it, we send data in groups of 8bits / 1byte.
	 *
	 * When we send a byte (byte2Send), we manipulate a vertical strip (8x1).
	 * For example, the first byte sent will write in column '1', rows '1 to 8';
	 * The seond byte sent will write in column '2', rows '1 to 8'.
	 *
	 * To refresh the entire display, we follow the loop below, sweeping the display
	 * horizontaly (left to right, 0 to 83) and when reaching the rightmost position,
	 * repeating in the line below (up to down, 0 to 5).
	 *
	 */
	for (uint8_t rowIdx = 0; rowIdx < 6; rowIdx++) //LCD Rows: 6*8(byte)= 48 pixels
	{
		for (uint8_t colIdx = 0; colIdx < 84; colIdx++) //LCD Cols: 84 pixels
		{
			
			uint8_t byte2Send = 0;
			for (uint8_t vertBytePos = 0; vertBytePos < 8; vertBytePos++)
			{
				byte2Send = LCD_displayCtrl->displayMatrix[8 * rowIdx + vertBytePos][colIdx] == 1 ?	byte2Send | (1 << (7 - vertBytePos)) :	byte2Send | (0 << (7 - vertBytePos));
			}
			LCD_SPI_sendData(byte2Send);
		}
	}
}
