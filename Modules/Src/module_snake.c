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

void _key_consumeKey();

void _IO_sendToLCD();

void _snake_initSnakeObj();
void _snake_updateSnakePos();
void _snake_printSnakeToBoard();
void _snake_changeDirection(enum direction_e newDir);

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
		moduleSnakeState = MODSNAKE_INIT_STOPPED;
	}
	else if(Activate && moduleSnakeState == MODSNAKE_INIT_STOPPED)
	{
		moduleSnakeState = MODSNAKE_INIT_SPLASH;
	}
	else if(Activate && moduleSnakeState == MODSNAKE_INIT_SPLASH)
	{
		if(moduleSnakeStateTrans == MODSNAKE_SPLASH_RUN)
		{
			moduleSnakeState = MODSNAKE_INIT_RUNNING;
		}
	}
	else if(!Activate && moduleSnakeState == MODSNAKE_INIT_RUNNING)
	{
		moduleSnakeState = MODSNAKE_INIT_STOPPED;
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
	
	for(uint8_t rowIdx = 8; rowIdx < 48; rowIdx++)
	{
		for(uint8_t colIdx = 0; colIdx < 84; colIdx++)
		{
			uint8_t pixelVal = moduleSnake_splashScreen1[colIdx + 84*(rowIdx-8)];
			LCD_Buffer_setPixel(moduleSnake_LCD_displayBuffer, rowIdx, colIdx, pixelVal);
		}
	}

	//moduleSnakeStateTrans = MODSNAKE_SPLASH_RUN;
}

void _moduleSnake_runGame()
{
	_key_consumeKey();

	_food_updateFood();
	_food_printFoodToBoard();

	_snake_updateSnakePos();
	_snake_printSnakeToBoard();

	_IO_sendToLCD();
}

void _key_consumeKey()
{
	switch(moduleSnake_keyPressedGlb)
	{
		case '2':
			_snake_changeDirection(UP);
			break;
		case '4':
			_snake_changeDirection(LEFT);
			break;
		case '6':
			_snake_changeDirection(RIGHT);
			break;
		case '8':
			_snake_changeDirection(DOWN);
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


