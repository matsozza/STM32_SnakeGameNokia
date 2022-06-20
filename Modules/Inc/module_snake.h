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
#include "service_lcd.h"
#include "service_randomGen.h"
#include <stdlib.h>

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
void moduleSnake_runTask(LCD_displayBuffer_t *LCD_displayBuffer, uint8_t Activate);
void moduleSnake_autoPlay();

#endif /* INC_CORE_MODULE_SNAKE_H_ */
