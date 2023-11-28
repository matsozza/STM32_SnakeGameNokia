/*
 * Flash Memory handling services
 * service_flashMem.h
 *
 *
 *  Created on: July 20, 2022
 *      Author: Matheus Sozza
 */

#ifndef INC_CORE_SERVICES_FLASHMEM_H_
#define INC_CORE_SERVICES_FLASHMEM_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "service_uart.h"

/* Defines -------------------------------------------------------------------*/
#define FLASHMEM_DEFAULT_SECTOR FLASH_SECTOR_11

// Sector 11, size of 0x1FFFF bytes (~128kb)
#define FLASHMEM_START_ADDRESS 0x080E0000
#define FLASHMEM_END_USED_ADDRESS 0x080E00FF
#define FLASHMEM_END_ADDRESS 0x080FFFFF

#define FLASHMEM_SECTOR_USED_SIZE (FLASHMEM_END_USED_ADDRESS - FLASHMEM_START_ADDRESS) + 1
#define FLASHMEM_DEFAULT_VRANGE FLASH_VOLTAGE_RANGE_3

#define FLASHMEM_DEBUG_LVL_USART 1

/* Typedef declare -----------------------------------------------------------*/


/* Function prototypes -------------------------------------------------------*/
int8_t flashMem_initService();
int8_t flashMem_write2flash();
int8_t flashMem_writeByte(uint8_t Data, uint32_t Offset);
int8_t flashMem_writeHalfWord(int16_t Data,  uint32_t Offset);

uint8_t flashMem_getByte(uint32_t Offset);
uint16_t flashMem_getHalfWord(uint32_t Offset);

int8_t flashMem_eraseSector();


#endif /* INC_CORE_SERVICES_FLASHMEM_H_ */


