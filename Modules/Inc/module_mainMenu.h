/**
 * Main Menu module - runs in 100ms task
 * module_mainMenu.h
 *
 *  Created on: November 20, 2023
 *      Author: Matheus Sozza
 */

#ifndef INC_CORE_MODULE_MAINMENU_H_
#define INC_CORE_MODULE_MAINMENU_H_

/* Includes ------------------------------------------------------------------*/
#include "cmsis_os.h"
#include "service_lcd.h"
#include "service_lcd_fonts.h"
#include "service_randomGen.h"
#include "service_flashMem.h"
#include "service_uart.h"
#include <stdlib.h>
#include <stdio.h>

/* Defines ------------------------------------------------------------------*/

/* Typedef declare -----------------------------------------------------------*/
enum moduleMainMenuState_e
{
    MODMENU_NOT_STARTED,
    MODMENU_ACTIVE,
    MODMENU_SUBTASK_STARTING,
    MODMENU_SUBTASK_RUNNING,
};

enum moduleMainMenuStateTrans_e
{
    MODMENU_IDLE,
    MODMENU_STARTING,
    MODMENU_RETURN_MENU,
    MODMENU_START_TASK,    
};


/* Function prototypes -------------------------------------------------------*/
void moduleMainMenu_runTask(LCD_displayBuffer_t *LCD_displayBuffer, char keyPressed, uint8_t Activate);

#endif /* INC_CORE_MODULE_MAINMENU_H_ */
