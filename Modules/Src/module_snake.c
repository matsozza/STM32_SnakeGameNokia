/**
 * Snake Game module - runs in 500ms task
 * module_snake.c
 * 
 * The game is intended to run in the 500ms task.
 * 
 * A board of 44 rows per 84 columns (pixels) will be created.
 * The board is composed of three layers:
 * 		L0: Snake Layer, where the snake runs
 * 		L1: Food / Object Layer, where any food or object is placed
 * 		L2: Boundaries / Board Layer, where all the game boundaries and
 * 		barriers are placed
 *
 *  L0 has precedence to write a pixel over layers L1 and L2.
 * 	L1 has precedence over layer L2.
 * 
 *  Created on: May 30, 2022
 *      Author: Matheus Sozza
 */


/* Private includes ----------------------------------------------------------*/
#include "module_snake.h"
#include "service_lcd.h"

/* External variables includes -----------------------------------------------*/
extern LCD_displayBuffer_t *LCD_displayBuffer01;

/* Internal variables includes -----------------------------------------------*/
snakeObj_t snakeObj;
uint8_t boardPixels[5][84][3];

/* Functions implementation --------------------------------------------------*/
void module_initGame()
{
	snake_initSnakeObj();
	board_initBGLayer();
}

void module_runGame()
{
	snake_updateSnakePos();
	snake_printSnakeToBoard();

	IO_sendToLCD();
}

void IO_sendToLCD()
{
	for (uint8_t rowIdx = 9; rowIdx < 48; rowIdx++)
	{
		for (uint8_t colIdx = 0; colIdx < 84; colIdx++)
		{
			// Print data according to layers hierarchy
			uint8_t layer0 = board_getPixel(rowIdx,  colIdx,  0);
			uint8_t layer1 = board_getPixel(rowIdx,  colIdx,  1);
			uint8_t layer2 = board_getPixel(rowIdx,  colIdx,  2);

			if (layer0 == 1 || layer1 == 1 || layer2 == 1)
			{
				(void)LCD_Buffer_setPixel(LCD_displayBuffer01,rowIdx,colIdx,1);
			}
			else if (layer0 == 0 && layer1 == 0 && layer2 == 0)
			{
				(void)LCD_Buffer_setPixel(LCD_displayBuffer01, rowIdx, colIdx, 1);
			}
		}
	}

}

void snake_initSnakeObj()
{
	snakeObj.size = 2; // Starts with size = 2
	snakeObj.bodyComponent[0].posRow = 20;
	snakeObj.bodyComponent[0].posCol = 20;
	snakeObj.bodyComponent[1].posRow = 20;
	snakeObj.bodyComponent[1].posCol = 21;
	snakeObj.movementDir = RIGHT; // First movement to the right
}

void snake_updateSnakePos()
{
	//Set snake head movement
	boardPos_t prevSnakePos = snakeObj.bodyComponent[0];
	switch(snakeObj.movementDir)
	{
		case RIGHT:
			snakeObj.bodyComponent[0].posCol++;
			break;
		case UP:
			snakeObj.bodyComponent[0].posRow++;
			break;
		case LEFT:
			snakeObj.bodyComponent[0].posCol--;
			break;
		case DOWN:
			snakeObj.bodyComponent[0].posRow--;
			break;
		default:
			return;
	}

	//Set snake body movement
	for (uint8_t bodyPart = snakeObj.size-1 ; bodyPart > 0; bodyPart--)
	{
		if(bodyPart==1)
		{
			snakeObj.bodyComponent[1] = prevSnakePos;
		}
		else
		{
			snakeObj.bodyComponent[bodyPart] = snakeObj.bodyComponent[bodyPart-1];
		}
	}
}

void snake_printSnakeToBoard()
{
	// Clear all pixels from snake layer
	for (int rowIdx = 0; rowIdx < 5; rowIdx++)
	{
		for (int colIdx = 0; colIdx < 84; colIdx++)
		{
			boardPixels[rowIdx][colIdx][0] |= 0x00;
			boardPixels[rowIdx][colIdx][0] |= 0x00;
		}
	}

	// Set the pixels for the new snake position
	for (uint8_t size = 0; size < snakeObj.size; size++)
	{
		uint8_t rowIdx = snakeObj.bodyComponent[size].posRow;
		uint8_t colIdx = snakeObj.bodyComponent[size].posCol;

		board_setPixel(rowIdx, colIdx, 1, 0); // Layer 0 - Snake Layer
	}
}

void snake_changeDirection(enum direction newDir)
{
	snakeObj.movementDir = newDir;
}

void board_initBGLayer()
{
	// Clear all static / background layer data
	for (int rowIdx = 0; rowIdx < 5; rowIdx++)
	{
		for (int colIdx = 0; colIdx < 84; colIdx++)
		{
			boardPixels[rowIdx][colIdx][2] |= 0x80;
			boardPixels[rowIdx][colIdx][2] |= 0x01;
		}
	}

	// Creates the boardPixels lines - vertical
	for (int rowIdx = 0; rowIdx < 5; rowIdx++)
	{
		boardPixels[rowIdx][0][2] = 0xFF;
		boardPixels[rowIdx][84][2] = 0xFF;
	}

	// Creates the boardPixels lines - horizontal
	for (int colIdx = 0; colIdx < 84; colIdx++)
	{
		boardPixels[0][colIdx][2] |= 0x80;
		boardPixels[5][colIdx][2] |= 0x01;
	}
}

void board_setPixel(uint8_t rowIdx, uint8_t colIdx, uint8_t pixelVal, uint8_t boardLayer)
{
	uint8_t rowGroupIdx = (uint8_t)rowIdx / 8; // 'major' row
	uint8_t rowPixelIdx = (uint8_t)rowIdx % 8; // 'minor' row

	// Set a value to a pixel of the board game
	if (pixelVal)
	{
		boardPixels[rowGroupIdx][colIdx][boardLayer] |= (1 << (7 - rowPixelIdx));
	}
	else
	{
		boardPixels[rowGroupIdx][colIdx][boardLayer] &= ~((uint8_t)(1 << (7 - rowPixelIdx)));
	}
}

uint8_t board_getPixel(uint8_t rowIdx, uint8_t colIdx, uint8_t boardLayer)
{
	uint8_t rowGroupIdx = (uint8_t)rowIdx / 8; // 'major' row
	uint8_t rowPixelIdx = (uint8_t)rowIdx % 8; // 'minor' row
	return boardPixels[rowGroupIdx][colIdx][boardLayer] & (1 << (7 - rowPixelIdx)); // Return value
}


