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
enum direction
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
    enum direction movementDir; // 0-right 1-up 2-left 3-down
    boardPos_t bodyComponent[50]; // 48 - 8 - 1 - 1 = 38 of height per  82 of width
} snakeObj_t;


/* Function prototypes -------------------------------------------------------*/
void moduleSnake_initGame();
void moduleSnake_runGame();
void IO_sendToLCD();
void snake_initSnakeObj();
void snake_updateSnakePos();
void snake_printSnakeToBoard();
void snake_changeDirection(enum direction newDir);
void board_initLayers();
void board_setPixel(uint8_t rowIdx, uint8_t colIdx, uint8_t pixelVal, uint8_t boardLayer);
uint8_t board_getPixel(uint8_t rowIdx, uint8_t colIdx, uint8_t boardLayer);
#endif /* INC_CORE_MODULE_SNAKE_H_ */
