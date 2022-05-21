/**
 * LCD High Level Services - Fonts
 * service_lcd_fonts.c
 *
 * Designed to work with NOKIA 5110 display via SPI
 *
 *
 *  Created on: May 08, 2022
 *      Author: Matheus Sozza (based on GitHub @baghayi/Nokia_5110)
 */

/* Private includes ----------------------------------------------------------*/
#include "service_lcd_fonts.h"

/* External variables includes -----------------------------------------------*/

/* Internal variables includes -----------------------------------------------*/
const LCD_Char_t ASCII_Library[] = LCD_ASCII_DICTIONARY

/* Functions implementation --------------------------------------------------*/

LCD_Char_t findCorrespondingByte(uint8_t ASCII_Code){
	uint8_t total_characters = sizeof(ASCII_Library) / sizeof(*ASCII_Library);
	for (uint8_t i = 0; i < total_characters; i++) {
        if (ASCII_Library[i].ASCII_Code == ASCII_Code) return ASCII_Library[i];
	}
	return ASCII_Library[0];
}

LCD_Char_t findCorrespondingChar(char ASCII_Char){
	uint8_t total_characters = sizeof(ASCII_Library) / sizeof(*ASCII_Library);
	for (uint8_t i = 0; i < total_characters; i++) {
        if (ASCII_Library[i].ASCII_Char == ASCII_Char) return ASCII_Library[i];
	}
	return ASCII_Library[0];
}
