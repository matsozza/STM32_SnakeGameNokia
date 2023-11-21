/**
 * Main Menu module - runs in 100ms task
 * module_mainMenu.c
 * 
 * The main menu is a parent module where all the other modules can be called from
 *
 *
 *  Created on: November 20, 2023
 *      Author: Matheus Sozza
 */


 /* Private includes ----------------------------------------------------------*/
#include "module_mainMenu.h"
#include "module_snake.h"

/* External variables includes -----------------------------------------------*/

/* Internal variables includes -----------------------------------------------*/
LCD_displayBuffer_t *moduleMainMenu_LCD_displayBuffer;
enum moduleMainMenuState_e moduleMainMenuState = MODMENU_NOT_STARTED; // State machine current state
enum moduleMainMenuStateTrans_e moduleMainMenuStateTrans = MODMENU_STARTING; // State machine internal transition
uint8_t moduleMainMenu_actvModule = 0x0; // Main menu module active
char moduleMainMenu_keyPressedGlb;

/* Internal constant includes -----------------------------------------------*/

/* Internal functions includes -----------------------------------------------*/
void _moduleMainMenu_stateTransition();
void _moduleMainMenu_stateFunction(LCD_displayBuffer_t *LCD_displayBuffer, char keyPressed);
void _moduleMainMenu_init(LCD_displayBuffer_t *LCD_displayBuffer);
void _moduleMainMenu_menuActive();
void _moduleMainMenu_subTaskRunning();
void _IO_printMenu();
void _IO_printLoading();

/* Functions implementation --------------------------------------------------*/
void moduleMainMenu_runTask(LCD_displayBuffer_t *LCD_displayBuffer, char keyPressed, uint8_t Activate)
{
	// State-machine instructions
	_moduleMainMenu_stateFunction(LCD_displayBuffer, keyPressed);

    // State-machine transitions
	_moduleMainMenu_stateTransition();
}

void _moduleMainMenu_stateTransition()
{
	if(moduleMainMenuState == MODMENU_NOT_STARTED) // Initialize Main Menu
    {
        moduleMainMenuState = MODMENU_ACTIVE;
    }
    else if(moduleMainMenuState == MODMENU_ACTIVE && moduleMainMenu_actvModule != 0) // Menu to Task start
	{
		moduleMainMenuState = MODMENU_SUBTASK_STARTING;
	}
	else if (moduleMainMenuState == MODMENU_SUBTASK_STARTING && moduleMainMenu_actvModule != 0) // Task starting to task running
    {
        static uint16_t tickCnt = 0;

        if(tickCnt > 16)
        {
            moduleMainMenuState = MODMENU_SUBTASK_RUNNING;
            tickCnt = 0;
        }
        else
        {
            _IO_printLoading();
            tickCnt++;
        }        
    }
    else if (moduleMainMenu_actvModule == 0) // Back to main menu
    {
        moduleMainMenuState = MODMENU_ACTIVE;
    }
}


void _moduleMainMenu_stateFunction(LCD_displayBuffer_t *LCD_displayBuffer, char keyPressed)
{
	moduleMainMenu_keyPressedGlb = keyPressed;

    if(moduleMainMenuState == MODMENU_NOT_STARTED)
    {
        _moduleMainMenu_init(LCD_displayBuffer);
    }
    else if(moduleMainMenuState == MODMENU_ACTIVE) // If main menu is active
    {
        _moduleMainMenu_menuActive();
    }
    else if(moduleMainMenuState == MODMENU_SUBTASK_STARTING) // If a specific task is running
    {
        // Empty transition
    }
    else if(moduleMainMenuState == MODMENU_SUBTASK_RUNNING)
    {
        _moduleMainMenu_subTaskRunning();
    }
}

void _moduleMainMenu_init(LCD_displayBuffer_t *LCD_displayBuffer)
{
    moduleMainMenu_LCD_displayBuffer = LCD_displayBuffer;
}

void _moduleMainMenu_menuActive()
{
    switch(moduleMainMenu_keyPressedGlb)
    {
        case '1':
            moduleMainMenu_actvModule = 0b00000001; // Run Task #01
            break;
        case '2':
            moduleMainMenu_actvModule = 0b00000010; // Run task #02
            break;
        default:
            // Display main menu normally
            _IO_printMenu();
    }
}

void _moduleMainMenu_subTaskRunning()
{
    switch(moduleMainMenu_keyPressedGlb)
    {
        case 'A':
            moduleMainMenu_actvModule = 0b00000000; // Return to main menu
            break;
        default:
            // Pass pressed key to sub-task function / run sub task normally
            if(moduleMainMenu_actvModule == 0b1)
            {
                moduleSnake_runTask(moduleMainMenu_LCD_displayBuffer,moduleMainMenu_keyPressedGlb,1);
            }
            else if (moduleMainMenu_actvModule == 0b10)
            {
                moduleSnake_runTask(moduleMainMenu_LCD_displayBuffer,moduleMainMenu_keyPressedGlb,1);
                moduleSnake_autoPlay();
            }
            break;
    }
}

void _IO_printMenu()
{
    // Clear all pixels from menu area
    for (uint8_t rowIdx = 8; rowIdx < 48; rowIdx++)
	{
		for (uint8_t colIdx = 0; colIdx < 84; colIdx++)
		{
            LCD_Buffer_setPixel(moduleMainMenu_LCD_displayBuffer, rowIdx, colIdx, 0);
		}
	}
    
    // Menu name
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 10, 0);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 10, 6);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 10, 12);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 10, 18);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 10, 24);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, 'M', 10, 30);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, 'e', 10, 36);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, 'n', 10, 42);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, 'u', 10, 48);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 10, 54);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 10, 60);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 10, 66);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 10, 72);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 10, 78);

    // Menu option 1
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '1', 18, 4);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 18, 10);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, 'S', 18, 16);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, 'n', 18, 22);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, 'a', 18, 28);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, 'k', 18, 34);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, 'e', 18, 40);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, ' ', 18, 46);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, 'G', 18, 52);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, 'a', 18, 58);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, 'm', 18, 64);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, 'e', 18, 70);

    // Menu option 2
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '2', 26, 4);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 26, 10);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, 'S', 26, 16);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, 'n', 26, 22);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, 'a', 26, 28);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, 'k', 26, 34);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, 'e', 26, 40);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, ' ', 26, 46);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, 'A', 26, 52);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, 'u', 26, 58);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, 't', 26, 64);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, 'o', 26, 70);
    
    // Menu lower border
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 34, 0);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 34, 6);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 34, 12);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 34, 18);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 34, 24);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 34, 30);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 34, 36);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 34, 42);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 34, 48);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 34, 54);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 34, 60);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 34, 66);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 34, 72);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 34, 78);

}

void _IO_printLoading()
{
    // Clear all pixels from menu area
    for (uint8_t rowIdx = 8; rowIdx < 48; rowIdx++)
	{
		for (uint8_t colIdx = 0; colIdx < 84; colIdx++)
		{
            LCD_Buffer_setPixel(moduleMainMenu_LCD_displayBuffer, rowIdx, colIdx, 0);
		}
	}
    

    // Loading upper border
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 18, 0);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 18, 6);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 18, 12);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 18, 18);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 18, 24);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 18, 30);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 18, 36);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 18, 42);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 18, 48);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 18, 54);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 18, 60);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 18, 66);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 18, 72);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 18, 78);

    // Loading text
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, ' ', 26, 4);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, 'L', 26, 10);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, 'o', 26, 16);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, 'a', 26, 22);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, 'd', 26, 28);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, 'i', 26, 34);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, 'n', 26, 40);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, 'g', 26, 46);
    
    static uint8_t tickCnt = 0;
    if(tickCnt > 6)
    {
        (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '.', 26, 52);
        (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '.', 26, 58);
        (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '.', 26, 64);
    }
    else if (tickCnt > 4)
    {
        (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '.', 26, 52);
        (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '.', 26, 58);
        (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, ' ', 26, 64);
    }
    else if (tickCnt > 2)
    {
        (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '.', 26, 52);
        (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, ' ', 26, 58);
        (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, ' ', 26, 64);
    }
    else
    {
        (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, ' ', 26, 52);
        (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, ' ', 26, 58);
        (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, ' ', 26, 64);
    }
    if(tickCnt++ > 10) tickCnt = 0;
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, ' ', 26, 70);
    
    // Loading lower border
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 34, 0);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 34, 6);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 34, 12);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 34, 18);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 34, 24);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 34, 30);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 34, 36);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 34, 42);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 34, 48);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 34, 54);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 34, 60);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 34, 66);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 34, 72);
    (void) LCD_Buffer_writeASCIIChar(moduleMainMenu_LCD_displayBuffer, '-', 34, 78);
}