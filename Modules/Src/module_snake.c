/**
 * Snake Game module - runs in 500ms task
 * module_snake.c
 * 
 * The game is intended to run in the 500ms task.
 * 
 * A board of 40 rows per 84 columns (pixels) will be created.
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
#include <stdlib.h>
#include "main.h"

/* External variables includes -----------------------------------------------*/
extern LCD_displayBuffer_t *LCD_displayBuffer01;

/* Internal variables includes -----------------------------------------------*/
snakeObj_t snakeObj;
foodObj_t foodObj;
uint8_t boardPixels[5][84][3];

/* Functions implementation --------------------------------------------------*/
void moduleSnake_initGame()
{
	snake_initSnakeObj();
	
	food_initFoodObj();

	board_initLayers();

	IO_sendToLCD();
}

void moduleSnake_runGame()
{
	food_updateFood();
	food_printFoodToBoard();

	snake_updateSnakePos();
	snake_printSnakeToBoard();

	IO_sendToLCD();
}

void IO_sendToLCD()
{
	// Print all the pixels related to the board
	for (uint8_t rowIdx = 0; rowIdx < 40; rowIdx++)
	{
		for (uint8_t colIdx = 0; colIdx < 84; colIdx++)
		{
			// Print data according to layers hierarchy
			uint8_t layer0 = board_getPixel(rowIdx, colIdx, 0);
			uint8_t layer1 = board_getPixel(rowIdx, colIdx, 1);
			uint8_t layer2 = board_getPixel(rowIdx, colIdx, 2);

			if (layer0 || layer1 || layer2)
			{
				(void)LCD_Buffer_setPixel(LCD_displayBuffer01, rowIdx+8, colIdx, 1);
			}
			else if ((layer0 || layer1 || layer2) && rowIdx == 24 && colIdx == 3)
			{
				(void)LCD_Buffer_setPixel(LCD_displayBuffer01, rowIdx+8, colIdx, 1);
			}
			else if (!layer0 && !layer1 && !layer2)
			{
				(void)LCD_Buffer_setPixel(LCD_displayBuffer01, rowIdx+8, colIdx, 0);
			}
		}
	}

	return;
}

void snake_initSnakeObj()
{
	snakeObj.size = 7; // Start size
	snakeObj.bodyComponent[0].posRow = 20;
	snakeObj.bodyComponent[0].posCol = 40;
	snakeObj.bodyComponent[1].posRow = 19;
	snakeObj.bodyComponent[1].posCol = 40;
	snakeObj.movementDir = DOWN; // First movement 
}

void snake_updateSnakePos()
{
	//Set snake head movement
	boardPos_t prevSnakeHeadPos = snakeObj.bodyComponent[0];
	switch(snakeObj.movementDir)
	{
		case RIGHT:
			snakeObj.bodyComponent[0].posCol+=1;
			break;
		case UP:
			snakeObj.bodyComponent[0].posRow-=1;
			break;
		case LEFT:
			snakeObj.bodyComponent[0].posCol-=1;
			break;
		case DOWN:
			snakeObj.bodyComponent[0].posRow+=1;
			break;
		default:
			return;
	}

	//Set snake body movement
	for (uint8_t bodyPart = 1; bodyPart < snakeObj.size; bodyPart++)
	{
		boardPos_t prevSnakeBodyPart, prevSnakeBodyPart2;

		if (bodyPart == 1) // Follow head movement
		{
			prevSnakeBodyPart = snakeObj.bodyComponent[1];
			snakeObj.bodyComponent[1] = prevSnakeHeadPos;
		}
		else // Update body positions
		{
			prevSnakeBodyPart2 = snakeObj.bodyComponent[bodyPart];
			snakeObj.bodyComponent[bodyPart] = prevSnakeBodyPart;

			//If is '0,0' uninit. position, do not propagate; use last valid
			if(prevSnakeBodyPart2.posRow != 0 && prevSnakeBodyPart2.posRow != 0)
			{
				prevSnakeBodyPart = prevSnakeBodyPart2;
			}

		}
	}
}

void snake_printSnakeToBoard()
{
	// Clear all pixels from snake layer
	for (int rowGroupIdx = 0; rowGroupIdx < 5; rowGroupIdx++)
	{
		for (int colIdx = 0; colIdx < 84; colIdx++)
		{
			boardPixels[rowGroupIdx][colIdx][0] = 0x00;
			boardPixels[rowGroupIdx][colIdx][0] = 0x00;
		}
	}

	// Set the pixels for the new snake position
	for (uint8_t idxSize = 0; idxSize < snakeObj.size; idxSize++)
	{
		uint8_t rowIdx = snakeObj.bodyComponent[idxSize].posRow;
		uint8_t colIdx = snakeObj.bodyComponent[idxSize].posCol;

		// Layer 0 - Snake Layer - Snake "Core"
		board_setPixel(rowIdx, colIdx, 1, 0); 

		// Layer 0 - Snake Layer - Snake "Thick Contours"
		board_setPixel(rowIdx+1, colIdx, 1, 0);
		board_setPixel(rowIdx-1, colIdx, 1, 0);
		board_setPixel(rowIdx, colIdx+1, 1, 0);
		board_setPixel(rowIdx, colIdx-1, 1, 0);
		
		board_setPixel(rowIdx-1, colIdx-1, 1, 0);
		board_setPixel(rowIdx+1, colIdx-1, 1, 0);
		board_setPixel(rowIdx-1, colIdx+1, 1, 0);
		board_setPixel(rowIdx+1, colIdx+1, 1, 0);

	}
}

void snake_changeDirection(enum direction newDir)
{
	snakeObj.movementDir = newDir;
}

void food_initFoodObj()
{
	foodObj.numFood = 0;
}

void food_updateFood()
{
	// Check if food shall be 'eaten'
	uint8_t foodRow = foodObj.foodComponent[0].posRow;
	uint8_t foodCol = foodObj.foodComponent[0].posCol;
	uint8_t snakeRow = snakeObj.bodyComponent[0].posRow;
	uint8_t snakeCol = snakeObj.bodyComponent[0].posCol;

	if(foodRow == snakeRow && foodCol == snakeCol)
	{
		foodObj.numFood = 0; // Remove food
		snakeObj.size+=7;

		// ***Delete after 
		if(snakeObj.size > 140)
		{
			snakeObj.size = 7;
		}
	}
	// If no food present, add one
	else if(foodObj.numFood == 0) // Just place one food per time
	{
		// Get a random position on the table
		//srand(snakeRow*foodRow+snakeCol*foodCol);		  // Init random number generator
		//uint8_t randomCol = (uint8_t)(rand() % 80) + 1;	  // Between col. 2 and 81
		uint8_t randomCol = (uint8_t)(DWT->CYCCNT % 80) + 1; // Between col. 2 and 81
		// uint8_t randomRow = (uint8_t)(rand() % 36) + 1;	  // Between col. 2 and 37
		uint8_t randomRow = (uint8_t)(DWT->CYCCNT % 36) + 1; // Between col. 2 and 37

		randomCol += randomCol % 2; // ensure is even
		randomRow += randomRow % 2; // ensure is even

		foodObj.foodComponent[0].posCol = randomCol;
		foodObj.foodComponent[0].posRow = randomRow;

		foodObj.numFood = 1;
	}
}

void food_printFoodToBoard()
{
	// Clear all pixels from food layer (1)
	for (int rowGroupIdx = 0; rowGroupIdx < 5; rowGroupIdx++)
	{
		for (int colIdx = 0; colIdx < 84; colIdx++)
		{
			boardPixels[rowGroupIdx][colIdx][1] = 0x00;
			boardPixels[rowGroupIdx][colIdx][1] = 0x00;
		}
	}

	// Set the pixels for the new food position
	for (uint8_t idxFood = 0; idxFood < foodObj.numFood; idxFood++)
	{
		uint8_t rowIdx = foodObj.foodComponent[idxFood].posRow;
		uint8_t colIdx = foodObj.foodComponent[idxFood].posCol;

		// Layer 1 - Food Layer - Food Shape
		board_setPixel(rowIdx + 1, colIdx, 1, 1);
		board_setPixel(rowIdx - 1, colIdx, 1, 1);
		board_setPixel(rowIdx, colIdx + 1, 1, 1);
		board_setPixel(rowIdx, colIdx - 1, 1, 1);
	}
}

void board_initLayers()
{
	// Clear all layers data
	for (uint8_t rowGroupIdx = 0; rowGroupIdx < 5; rowGroupIdx++)
	{
		for (uint8_t colIdx = 0; colIdx < 84; colIdx++)
		{
			for (uint8_t layerIdx = 0; layerIdx < 3; layerIdx++)
			{
				boardPixels[rowGroupIdx][colIdx][layerIdx] = 0x00;
				boardPixels[rowGroupIdx][colIdx][layerIdx] = 0x00;
			}
		}
	}

	// Creates the boardPixels lines - vertical
	for (uint8_t rowGroupIdx = 0; rowGroupIdx < 5; rowGroupIdx++)
	{
		boardPixels[rowGroupIdx][0][2] = 0xFF;
		boardPixels[rowGroupIdx][83][2] = 0xFF;
	}

	// Creates the boardPixels lines - horizontal
	for (uint8_t colIdx = 0; colIdx < 84; colIdx++)
	{
		boardPixels[0][colIdx][2] |= 0x01;
		boardPixels[4][colIdx][2] |= 0x80;
	}
}

void board_setPixel(uint8_t rowIdx, uint8_t colIdx, uint8_t pixelVal, uint8_t boardLayer)
{
	uint8_t rowGroupIdx = (uint8_t)(rowIdx / 8); // 'major' row of board - skip first display rowGroup
	uint8_t rowPixelIdx = (uint8_t)(rowIdx % 8); // 'minor' row of board

	// Set a value to a pixel of the board game
	if (pixelVal)
	{
		boardPixels[rowGroupIdx][colIdx][boardLayer] |= (1 << (rowPixelIdx));
	}
	else
	{
		boardPixels[rowGroupIdx][colIdx][boardLayer] &= ~((uint8_t)(1 << (rowPixelIdx)));
	}
}

uint8_t board_getPixel(uint8_t rowIdx, uint8_t colIdx, uint8_t boardLayer)
{
	uint8_t rowGroupIdx = (uint8_t)(rowIdx / 8); // 'major' row
	uint8_t rowPixelIdx = (uint8_t)(rowIdx % 8); // 'minor' row
	return (uint8_t) ((boardPixels[rowGroupIdx][colIdx][boardLayer] & (1 << (rowPixelIdx))) > 0); // Return value
}


