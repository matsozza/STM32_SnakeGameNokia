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
int LCD_sendByteSPI(uint8_t data, uint8_t DC){
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

int LCD_initializeConfigs()
{
	// Toggle RESET Pin - Clear LCD memory
	HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET);

	LCD_sendCommand(0x21); // Function set - Extended
	LCD_sendCommand(0xB9); // VOP - Contrast set
	LCD_sendCommand(0x04); // Temp. coefficient
	LCD_sendCommand(0x14); // Bias system

	LCD_sendCommand(0x20); // Function set - Basic
	LCD_sendCommand(0x0C); // Display config. - Normal Mode
	LCD_sendCommand(0x40); // Addressing - Y = 0
	LCD_sendCommand(0x80); // Addressing - X = 0


	//Test - write 2 squares
	for (int index = 0; index < (84*48) / 8; index++)
	{
		//LCD_sendData(grap[index]);
		//LCD_sendData(0xF0);
	}

	LCD_writeASCIIChar('O');
	LCD_writeASCIIChar('i');
	LCD_writeASCIIChar(' ');
	LCD_writeASCIIChar('J');
	LCD_writeASCIIChar('e');
	LCD_writeASCIIChar('n');
	LCD_writeASCIIChar('n');
	LCD_writeASCIIChar('y');
	LCD_writeASCIIChar('!');

	return 0;
}

int LCD_writeASCIIChar(char ASCII_char){
	//Test - Write chars
	LCD_Char_t LCD_CharSel = findCorrespondingChar(ASCII_char); // Find ASCII in library

	// Iterate through all vertical components (bytes) of the ASCII char
	for(int j=0; j<LCD_CharSel.bitmap_total_bytes; j++){
		uint8_t LCD_vertArray = LCD_CharSel.bitmap[j];
		LCD_sendData(LCD_vertArray); // Send vertical byte
	}
	LCD_sendData(0x00); // Send 1 strip of space
}

