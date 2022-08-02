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
#include "service_flashMem.h"
#include <stdio.h>

/* External variables includes -----------------------------------------------*/

/* Internal variables includes -----------------------------------------------*/
LCD_displayBuffer_t *moduleSnake_LCD_displayBuffer;
enum moduleSnakeState_e moduleSnakeState = MODSNAKE_NOT_INIT; // State machine current state
enum moduleSnakeStateTrans_e moduleSnakeStateTrans = MODSNAKE_IDLE; // State machine internal transition
char moduleSnake_keyPressedGlb;
uint8_t moduleSnake_boardPixels[5][84][3];

snakeObj_t snakeObj; // Internal type
foodObj_t foodObj; // Internal type

/* Internal constant includes -----------------------------------------------*/
const uint8_t moduleSnake_splashScreen1[] = MODSNAKE_SPLASH_SCREEN_01

/* Internal functions includes -----------------------------------------------*/
void _moduleSnake_stateTransition(uint8_t Activate);
void _moduleSnake_stateFunction(LCD_displayBuffer_t *LCD_displayBuffer, char keyPressed);

void _moduleSnake_initGame(LCD_displayBuffer_t *LCD_displayBuffer);
void _moduleSnake_runGame();
void _moduleSnake_splashScreen();

void _key_consumeKey();

void _IO_sendToLCD();

void _snake_initSnakeObj();
void _snake_updateSnakePos();
void _snake_printSnakeToBoard();
void _snake_changeDirection(enum direction_e newDir);
void _snake_checkViolation();

void _food_initFoodObj();
void _food_updateFood();
void _food_printFoodToBoard();

void _board_initLayers();
void _board_setPixel(uint8_t rowIdx, uint8_t colIdx, uint8_t pixelVal, uint8_t boardLayer);
uint8_t _board_getPixel(uint8_t rowIdx, uint8_t colIdx, uint8_t boardLayer);

/* Functions implementation --------------------------------------------------*/
void moduleSnake_runTask(LCD_displayBuffer_t *LCD_displayBuffer, char keyPressed, uint8_t Activate)
{
	// State-machine instructions
	_moduleSnake_stateFunction(LCD_displayBuffer, keyPressed);

	// State-machine transitions
	_moduleSnake_stateTransition(Activate);
}

void moduleSnake_autoPlay()
{
		//Simple logic to pursuit food
		uint8_t foodRow = foodObj.foodComponent[0].posRow;
		uint8_t foodCol = foodObj.foodComponent[0].posCol;
		uint8_t snakeRow = snakeObj.bodyComponent[0].posRow;
		uint8_t snakeCol = snakeObj.bodyComponent[0].posCol;
		if (foodRow < snakeRow)
		{
			_snake_changeDirection(UP);
		}
		else if (foodRow > snakeRow)
		{
			_snake_changeDirection(DOWN);
		}
		else
		{
			if (foodCol < snakeCol)
			{
				_snake_changeDirection(LEFT);
			}
			else if (foodCol > snakeCol)
			{
				_snake_changeDirection(RIGHT);
			}
		}
}

void _moduleSnake_stateTransition(uint8_t Activate)
{
	if(Activate && moduleSnakeState == MODSNAKE_NOT_INIT)
	{
		if(Activate) moduleSnakeState = MODSNAKE_INIT_STOPPED;
	}
	else if(Activate && moduleSnakeState == MODSNAKE_INIT_STOPPED)
	{
		if(Activate) moduleSnakeState = MODSNAKE_INIT_SPLASH;
	}
	else if(moduleSnakeState == MODSNAKE_INIT_SPLASH)
	{
		if(Activate && moduleSnakeStateTrans == MODSNAKE_SPLASH_RUN)
		{
			moduleSnakeState = MODSNAKE_INIT_RUNNING;
		}
	}
	else if(moduleSnakeState == MODSNAKE_INIT_RUNNING)
	{
		if(!Activate)
		{
			moduleSnakeState = MODSNAKE_INIT_STOPPED;
		}
		else if(moduleSnakeStateTrans == MODSNAKE_RUN_GAMEOVER)
		{
			moduleSnakeState = MODSNAKE_INIT_GAMEOVER;
		}
	}
	else if(moduleSnakeState == MODSNAKE_INIT_GAMEOVER)
	{
		if(moduleSnakeStateTrans == MODSNAKE_GAMEOVER_STOPPED)
		{
			moduleSnakeState = MODSNAKE_INIT_STOPPED;
		}
	}
}

void _moduleSnake_stateFunction(LCD_displayBuffer_t *LCD_displayBuffer, char keyPressed)
{
	moduleSnake_keyPressedGlb = keyPressed;
	
	if(moduleSnakeState == MODSNAKE_NOT_INIT)
	{
		// Empty
	}
	else if(moduleSnakeState == MODSNAKE_INIT_STOPPED)
	{
		_moduleSnake_initGame(LCD_displayBuffer);
	}
	else if(moduleSnakeState == MODSNAKE_INIT_SPLASH)
	{
		_moduleSnake_splashScreen();
	}
	else if(moduleSnakeState == MODSNAKE_INIT_RUNNING)
	{
		_moduleSnake_runGame();
	}
	else if(moduleSnakeState == MODSNAKE_INIT_GAMEOVER)
	{
		_moduleSnake_gameOver();
	}
}

void _moduleSnake_initGame(LCD_displayBuffer_t *LCD_displayBuffer)
{
	moduleSnake_LCD_displayBuffer = LCD_displayBuffer;
	_snake_initSnakeObj();
	_food_initFoodObj();
	_board_initLayers();
	_IO_sendToLCD();
}

void _moduleSnake_splashScreen()
{
	static uint8_t splashTic = 0;

	// Show background image always
	for(uint8_t rowIdx = 8; rowIdx < 48; rowIdx++)
	{
		for(uint8_t colIdx = 0; colIdx < 84; colIdx++)
		{
			uint8_t pixelVal = moduleSnake_splashScreen1[colIdx + 84*(rowIdx-8)];
			LCD_Buffer_setPixel(moduleSnake_LCD_displayBuffer, rowIdx, colIdx, pixelVal);
		}
	}

	if(splashTic>9)
	{
		//Outer border + inner board
		for(uint8_t rowIdx = 20; rowIdx < 32; rowIdx++)
		{
			for(uint8_t colIdx = 4; colIdx < 80; colIdx++)
			{
				uint8_t pixelVal = rowIdx==20||rowIdx==31||colIdx==4||colIdx == 79 ? 1 : 0;
				LCD_Buffer_setPixel(moduleSnake_LCD_displayBuffer, rowIdx, colIdx, pixelVal);
			}
		}
		
		//Text 'Press Key'
		LCD_Buffer_writeASCIIChar(moduleSnake_LCD_displayBuffer, 'P', 22, 14);
		LCD_Buffer_writeASCIIChar(moduleSnake_LCD_displayBuffer, 'r', 22, 20);
		LCD_Buffer_writeASCIIChar(moduleSnake_LCD_displayBuffer, 'e', 22, 26);
		LCD_Buffer_writeASCIIChar(moduleSnake_LCD_displayBuffer, 's', 22, 32);
		LCD_Buffer_writeASCIIChar(moduleSnake_LCD_displayBuffer, 's', 22, 38);
		LCD_Buffer_writeASCIIChar(moduleSnake_LCD_displayBuffer, ' ', 22, 44);
		LCD_Buffer_writeASCIIChar(moduleSnake_LCD_displayBuffer, 'K', 22, 50);
		LCD_Buffer_writeASCIIChar(moduleSnake_LCD_displayBuffer, 'e', 22, 56);
		LCD_Buffer_writeASCIIChar(moduleSnake_LCD_displayBuffer, 'y', 22, 62);
		LCD_Buffer_writeASCIIChar(moduleSnake_LCD_displayBuffer, '!', 22, 68);
	}

	if(moduleSnake_keyPressedGlb != 'x') // If any key pressed, start game
	{
		moduleSnakeStateTrans = MODSNAKE_SPLASH_RUN;
	}

	splashTic = (splashTic+1)%20;
}

void _moduleSnake_gameOver()
{
	static uint8_t gameOverTic = 0;

	// Show background image always
	for(uint8_t rowIdx = 8; rowIdx < 48; rowIdx++)
	{
		for(uint8_t colIdx = 0; colIdx < 84; colIdx++)
		{
			uint8_t pixelVal = moduleSnake_splashScreen1[colIdx + 84*(rowIdx-8)];
			LCD_Buffer_setPixel(moduleSnake_LCD_displayBuffer, rowIdx, colIdx, pixelVal);
		}
	}

	//Outer border + inner board
	for(uint8_t rowIdx = 10; rowIdx < 40; rowIdx++)
	{
		for(uint8_t colIdx = 4; colIdx < 80; colIdx++)
		{
			uint8_t pixelVal = rowIdx==10||rowIdx==39||colIdx==4||colIdx == 79 ? 1 : 0;
			LCD_Buffer_setPixel(moduleSnake_LCD_displayBuffer, rowIdx, colIdx, pixelVal);
		}
	}
	
	// Write gameover text
	uint8_t snakeRecordSize = flashMem_getByte(FLASHMEM_START_ADDRESS, MODSNAKE_EEPROM_RECORD);
	char snakeRecordStr[4] = {'\0','\0','\0','\0'};
	sprintf(snakeRecordStr, "%d", snakeRecordSize);

	char snakeCurrSize[4] = {'\0','\0','\0','\0'};;
	sprintf(snakeCurrSize, "%d", snakeObj.size);

	LCD_Buffer_writeASCIIChar(moduleSnake_LCD_displayBuffer, 'G', 12, 14);
	LCD_Buffer_writeASCIIChar(moduleSnake_LCD_displayBuffer, 'A', 12, 20);
	LCD_Buffer_writeASCIIChar(moduleSnake_LCD_displayBuffer, 'M', 12, 26);
	LCD_Buffer_writeASCIIChar(moduleSnake_LCD_displayBuffer, 'E', 12, 32);
	LCD_Buffer_writeASCIIChar(moduleSnake_LCD_displayBuffer, ' ', 12, 38);
	LCD_Buffer_writeASCIIChar(moduleSnake_LCD_displayBuffer, 'O', 12, 44);
	LCD_Buffer_writeASCIIChar(moduleSnake_LCD_displayBuffer, 'V', 12, 50);
	LCD_Buffer_writeASCIIChar(moduleSnake_LCD_displayBuffer, 'E', 12, 56);
	LCD_Buffer_writeASCIIChar(moduleSnake_LCD_displayBuffer, 'R', 12, 62);
	LCD_Buffer_writeASCIIChar(moduleSnake_LCD_displayBuffer, '!', 12, 68);

	LCD_Buffer_writeASCIIChar(moduleSnake_LCD_displayBuffer, 'S', 20, 12);
	LCD_Buffer_writeASCIIChar(moduleSnake_LCD_displayBuffer, 'c', 20, 18);
	LCD_Buffer_writeASCIIChar(moduleSnake_LCD_displayBuffer, 'o', 20, 24);
	LCD_Buffer_writeASCIIChar(moduleSnake_LCD_displayBuffer, 'r', 20, 30);
	LCD_Buffer_writeASCIIChar(moduleSnake_LCD_displayBuffer, 'e', 20, 36);
	LCD_Buffer_writeASCIIChar(moduleSnake_LCD_displayBuffer, ':', 20, 42);
	LCD_Buffer_writeASCIIChar(moduleSnake_LCD_displayBuffer, snakeCurrSize[0], 20, 48);
	LCD_Buffer_writeASCIIChar(moduleSnake_LCD_displayBuffer, snakeCurrSize[1], 20, 54);
	LCD_Buffer_writeASCIIChar(moduleSnake_LCD_displayBuffer, snakeCurrSize[2], 20, 60);
	LCD_Buffer_writeASCIIChar(moduleSnake_LCD_displayBuffer, snakeCurrSize[3], 20, 66);

	LCD_Buffer_writeASCIIChar(moduleSnake_LCD_displayBuffer, 'R', 28, 12);
	LCD_Buffer_writeASCIIChar(moduleSnake_LCD_displayBuffer, 'e', 28, 18);
	LCD_Buffer_writeASCIIChar(moduleSnake_LCD_displayBuffer, 'c', 28, 24);
	LCD_Buffer_writeASCIIChar(moduleSnake_LCD_displayBuffer, 'o', 28, 30);
	LCD_Buffer_writeASCIIChar(moduleSnake_LCD_displayBuffer, 'r', 28, 36);
	LCD_Buffer_writeASCIIChar(moduleSnake_LCD_displayBuffer, 'd', 28, 42);
	LCD_Buffer_writeASCIIChar(moduleSnake_LCD_displayBuffer, ':', 28, 48);
	LCD_Buffer_writeASCIIChar(moduleSnake_LCD_displayBuffer, snakeRecordStr[0], 28, 54);
	LCD_Buffer_writeASCIIChar(moduleSnake_LCD_displayBuffer, snakeRecordStr[1], 28, 60);
	LCD_Buffer_writeASCIIChar(moduleSnake_LCD_displayBuffer, snakeRecordStr[2], 28, 66);
	LCD_Buffer_writeASCIIChar(moduleSnake_LCD_displayBuffer, snakeRecordStr[3], 28, 72);
	gameOverTic++;

	if(gameOverTic>50)
	{
		// Save record in non-volatile memory (NVM)
		if(snakeRecordSize < snakeObj.size)
		{
			flashMem_writeByte(snakeObj.size,FLASHMEM_START_ADDRESS, MODSNAKE_EEPROM_RECORD);
		}

		moduleSnakeStateTrans = MODSNAKE_GAMEOVER_STOPPED;
		gameOverTic=0;
	}
}

void _moduleSnake_runGame()
{
	_key_consumeKey();

	_food_updateFood();
	_food_printFoodToBoard();

	_snake_updateSnakePos();
	_snake_checkViolation();
	_snake_printSnakeToBoard();
	_snake_checkViolation();

	_IO_sendToLCD();

}

void _key_consumeKey()
{
	switch(moduleSnake_keyPressedGlb)
	{
		// Direct / direction independent cases
		case '2':
			if(snakeObj.movementDir != DOWN) _snake_changeDirection(UP);
			break;
		case '4':
			if(snakeObj.movementDir != RIGHT) _snake_changeDirection(LEFT);
			break;
		case '6':
			if(snakeObj.movementDir != LEFT) _snake_changeDirection(RIGHT);
			break;
		case '8':
			if(snakeObj.movementDir != UP) _snake_changeDirection(DOWN);
			break;
		// Indirect / direction dependent case
		case '1':
			if(snakeObj.movementDir == LEFT)
			{
				_snake_changeDirection(UP);
			}
			else if((snakeObj.movementDir == UP))
			{
				_snake_changeDirection(LEFT);
			}
			break;
		case '3':
			if(snakeObj.movementDir == RIGHT)
			{
				_snake_changeDirection(UP);
			}
			else if((snakeObj.movementDir == UP))
			{
				_snake_changeDirection(RIGHT);
			}
			break;
		case '7':
			if(snakeObj.movementDir == LEFT)
			{
				_snake_changeDirection(DOWN);
			}
			else if((snakeObj.movementDir == DOWN))
			{
				_snake_changeDirection(LEFT);
			}
			break;
		case '9':
			if(snakeObj.movementDir == RIGHT)
			{
				_snake_changeDirection(DOWN);
			}
			else if((snakeObj.movementDir == DOWN))
			{
				_snake_changeDirection(RIGHT);
			}
			break;
	}
}

void _IO_sendToLCD()
{
	// Print all the pixels related to the board
	for (uint8_t rowIdx = 0; rowIdx < 40; rowIdx++)
	{
		for (uint8_t colIdx = 0; colIdx < 84; colIdx++)
		{
			// Print data according to layers hierarchy
			uint8_t layer0 = _board_getPixel(rowIdx, colIdx, 0);
			uint8_t layer1 = _board_getPixel(rowIdx, colIdx, 1);
			uint8_t layer2 = _board_getPixel(rowIdx, colIdx, 2);

			if (layer0 || layer1 || layer2)
			{
				(void)LCD_Buffer_setPixel(moduleSnake_LCD_displayBuffer, rowIdx+8, colIdx, 1);
			}
			else if (!layer0 && !layer1 && !layer2)
			{
				(void)LCD_Buffer_setPixel(moduleSnake_LCD_displayBuffer, rowIdx+8, colIdx, 0);
			}
		}
	}

	return;
}

void _snake_initSnakeObj()
{
	snakeObj.size = 7; // Start size
	snakeObj.bodyComponent[0].posRow = 20;
	snakeObj.bodyComponent[0].posCol = 40;
	snakeObj.bodyComponent[1].posRow = 19;
	snakeObj.bodyComponent[1].posCol = 40;
	snakeObj.movementDir = DOWN; // First movement 
}

void _snake_updateSnakePos()
{
	//Move snake once each 2 tasks (slow-down)
	static uint8_t snakeTic = 0;

	if(!snakeTic)
	{
		snakeTic=1;
		return;
	}
	else{
		snakeTic=0;
	}
	
	//Set snake head movement
	boardPos_t prevSnakeHeadPos = snakeObj.bodyComponent[0];
	switch(snakeObj.movementDir)
	{
		case RIGHT:
			snakeObj.bodyComponent[0].posCol+=2;
			break;
		case UP:
			snakeObj.bodyComponent[0].posRow-=2;
			break;
		case LEFT:
			snakeObj.bodyComponent[0].posCol-=2;
			break;
		case DOWN:
			snakeObj.bodyComponent[0].posRow+=2;
			break;
		default:
			return;
	}

	// Avoid negatives / out of bounds, LCD may glitch
	if(snakeObj.bodyComponent[0].posCol < 0)
	{
		snakeObj.bodyComponent[0].posCol = 0;
	} 
	else if(snakeObj.bodyComponent[0].posCol > 83)
	{
		snakeObj.bodyComponent[0].posCol = 83;
	}

	if(snakeObj.bodyComponent[0].posRow < 0)
	{
		snakeObj.bodyComponent[0].posRow = 0;
	} 
	else if(snakeObj.bodyComponent[0].posRow > 39)
	{
		snakeObj.bodyComponent[0].posRow = 39;
	}

	//Set snake body movement
	for (uint8_t bodyPart = 1; bodyPart < snakeObj.size; bodyPart++)
	{
		boardPos_t prevSnakeBodyPart, prevSnakeBodyPart2;

		if (bodyPart==1) // Follow head movement
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

void _snake_printSnakeToBoard()
{
	// Clear all pixels from snake layer
	for (int rowGroupIdx = 0; rowGroupIdx < 5; rowGroupIdx++)
	{
		for (int colIdx = 0; colIdx < 84; colIdx++)
		{
			moduleSnake_boardPixels[rowGroupIdx][colIdx][0] = 0x00;
			moduleSnake_boardPixels[rowGroupIdx][colIdx][0] = 0x00;
		}
	}

	// Set the pixels for the new snake position
	for (uint8_t idxSize = 0; idxSize < snakeObj.size; idxSize++)
	{
		uint8_t rowIdx = snakeObj.bodyComponent[idxSize].posRow;
		uint8_t colIdx = snakeObj.bodyComponent[idxSize].posCol;

		// Layer 0 - Snake Layer - Snake "Core"
		_board_setPixel(rowIdx, colIdx, 1, 0); 

		// Layer 0 - Snake Layer - Snake "Thick Contours"
		_board_setPixel(rowIdx+1, colIdx, 1, 0);
		_board_setPixel(rowIdx-1, colIdx, 1, 0);
		_board_setPixel(rowIdx, colIdx+1, 1, 0);
		_board_setPixel(rowIdx, colIdx-1, 1, 0);
		
		_board_setPixel(rowIdx-1, colIdx-1, 1, 0);
		_board_setPixel(rowIdx+1, colIdx-1, 1, 0);
		_board_setPixel(rowIdx-1, colIdx+1, 1, 0);
		_board_setPixel(rowIdx+1, colIdx+1, 1, 0);

	}
}

void _snake_changeDirection(enum direction_e newDir)
{
	snakeObj.movementDir = newDir;
}

void _snake_checkViolation()
{
	// Check if the snake touched itself
	for(uint8_t bodyPart = 1; bodyPart < snakeObj.size; bodyPart++)
	{
		if((snakeObj.bodyComponent[0].posRow == snakeObj.bodyComponent[bodyPart].posRow) &&
			(snakeObj.bodyComponent[0].posCol == snakeObj.bodyComponent[bodyPart].posCol))
			{
				moduleSnakeStateTrans = MODSNAKE_RUN_GAMEOVER;
				return;
			}
	}

	// Check if the snake touched the wall
	if(snakeObj.bodyComponent[0].posRow <= 0 ||
	 	snakeObj.bodyComponent[0].posRow >= 39 ||
	 	snakeObj.bodyComponent[0].posCol <= 0 ||
	 	snakeObj.bodyComponent[0].posCol >= 83)
		{
			moduleSnakeStateTrans = MODSNAKE_RUN_GAMEOVER;
		}

	return;
}

void _food_initFoodObj()
{
	foodObj.numFood = 0;
}

void _food_updateFood()
{
	// Check if food shall be 'eaten'
	uint8_t foodRow = foodObj.foodComponent[0].posRow;
	uint8_t foodCol = foodObj.foodComponent[0].posCol;
	uint8_t snakeRow = snakeObj.bodyComponent[0].posRow;
	uint8_t snakeCol = snakeObj.bodyComponent[0].posCol;

	if(foodRow == snakeRow && foodCol == snakeCol)
	{
		foodObj.numFood = 0; // Remove food
		snakeObj.size+=5;

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
		uint8_t randomCol = (uint8_t)(randomGen_random32b() % 80) + 1; // Between col. 2 and 81
		uint8_t randomRow = (uint8_t)(randomGen_random32b() % 36) + 1; // Between col. 2 and 37

		randomCol += randomCol % 2; // ensure is even
		randomRow += randomRow % 2; // ensure is even

		foodObj.foodComponent[0].posCol = randomCol;
		foodObj.foodComponent[0].posRow = randomRow;

		foodObj.numFood = 1;
	}
}

void _food_printFoodToBoard()
{
	// Clear all pixels from food layer (1)
	for (int rowGroupIdx = 0; rowGroupIdx < 5; rowGroupIdx++)
	{
		for (int colIdx = 0; colIdx < 84; colIdx++)
		{
			moduleSnake_boardPixels[rowGroupIdx][colIdx][1] = 0x00;
			moduleSnake_boardPixels[rowGroupIdx][colIdx][1] = 0x00;
		}
	}

	// Set the pixels for the new food position
	for (uint8_t idxFood = 0; idxFood < foodObj.numFood; idxFood++)
	{
		uint8_t rowIdx = foodObj.foodComponent[idxFood].posRow;
		uint8_t colIdx = foodObj.foodComponent[idxFood].posCol;

		// Layer 1 - Food Layer - Food Shape
		_board_setPixel(rowIdx + 1, colIdx, 1, 1);
		_board_setPixel(rowIdx - 1, colIdx, 1, 1);
		_board_setPixel(rowIdx, colIdx + 1, 1, 1);
		_board_setPixel(rowIdx, colIdx - 1, 1, 1);
	}
}

void _board_initLayers()
{
	// Clear all layers data
	for (uint8_t rowGroupIdx = 0; rowGroupIdx < 5; rowGroupIdx++)
	{
		for (uint8_t colIdx = 0; colIdx < 84; colIdx++)
		{
			for (uint8_t layerIdx = 0; layerIdx < 3; layerIdx++)
			{
				moduleSnake_boardPixels[rowGroupIdx][colIdx][layerIdx] = 0x00;
				moduleSnake_boardPixels[rowGroupIdx][colIdx][layerIdx] = 0x00;
			}
		}
	}

	// Creates the moduleSnake_boardPixels lines - vertical
	for (uint8_t rowGroupIdx = 0; rowGroupIdx < 5; rowGroupIdx++)
	{
		moduleSnake_boardPixels[rowGroupIdx][0][2] = 0xFF;
		moduleSnake_boardPixels[rowGroupIdx][83][2] = 0xFF;
	}

	// Creates the moduleSnake_boardPixels lines - horizontal
	for (uint8_t colIdx = 0; colIdx < 84; colIdx++)
	{
		moduleSnake_boardPixels[0][colIdx][2] |= 0x01;
		moduleSnake_boardPixels[4][colIdx][2] |= 0x80;
	}
}

void _board_setPixel(uint8_t rowIdx, uint8_t colIdx, uint8_t pixelVal, uint8_t boardLayer)
{
	uint8_t rowGroupIdx = (uint8_t)(rowIdx / 8); // 'major' row of board - skip first display rowGroup
	uint8_t rowPixelIdx = (uint8_t)(rowIdx % 8); // 'minor' row of board

	// Set a value to a pixel of the board game
	if (pixelVal)
	{
		moduleSnake_boardPixels[rowGroupIdx][colIdx][boardLayer] |= (1 << (rowPixelIdx));
	}
	else
	{
		moduleSnake_boardPixels[rowGroupIdx][colIdx][boardLayer] &= ~((uint8_t)(1 << (rowPixelIdx)));
	}
}

uint8_t _board_getPixel(uint8_t rowIdx, uint8_t colIdx, uint8_t boardLayer)
{
	uint8_t rowGroupIdx = (uint8_t)(rowIdx / 8); // 'major' row
	uint8_t rowPixelIdx = (uint8_t)(rowIdx % 8); // 'minor' row
	return (uint8_t) ((moduleSnake_boardPixels[rowGroupIdx][colIdx][boardLayer] & (1 << (rowPixelIdx))) > 0); // Return value
}


