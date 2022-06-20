/**
 * Snake Game module - runs in 500ms task
 * module_snake.h
 *
 *  Created on: May 30, 2022
 *      Author: Matheus Sozza
 */

#ifndef INC_CORE_MODULE_SNAKE_H_
#define INC_CORE_MODULE_SNAKE_H_

/* Includes ------------------------------------------------------------------*/
#include "cmsis_os.h"

/* Defines ------------------------------------------------------------------*/


/* Typedef declare -----------------------------------------------------------*/
enum moduleSnakeState_e
{
    MODSNAKE_NOT_INIT,
    MODSNAKE_INIT_STOPPED,
    MODSNAKE_INIT_RUNNING
};

enum direction_e
{
    RIGHT,
    UP,
    LEFT,
    DOWN
};

typedef struct
{
    uint8_t posRow; // From 0 to 39 (40 rows)
    uint8_t posCol; // From 0 to 83 (84 cols)
} boardPos_t;

typedef struct{
    uint8_t size;
    enum direction_e movementDir; // 0-right 1-up 2-left 3-down
    boardPos_t bodyComponent[500]; // 48 - 8 - 1 - 1 = 38 of height per  82 of width
} snakeObj_t;

typedef struct{
    uint8_t numFood;
    boardPos_t foodComponent[5];
} foodObj_t;


/* Function prototypes -------------------------------------------------------*/
void moduleSnake_runTask(uint8_t Activate);

void _moduleSnake_initGame();
void _moduleSnake_runGame();

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

#endif /* INC_CORE_MODULE_SNAKE_H_ */
