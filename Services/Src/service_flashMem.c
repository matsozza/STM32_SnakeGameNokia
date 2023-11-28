/*
 * Flash Memory handling services
 * service_flashMem.c
 *
 *
 *  Created on: July 20, 2022
 *      Author: Matheus Sozza
 */

/* Private includes ----------------------------------------------------------*/
#include "service_flashMem.h"
#include <stdlib.h>

/* External variables includes -----------------------------------------------*/

/* Internal variables includes -----------------------------------------------*/
uint8_t* memoryImage = 0;

/* Functions implementation --------------------------------------------------*/

int8_t flashMem_initService()
{
	// Allocate heap space for memory image creation
	memoryImage = malloc(sizeof(uint8_t) * FLASHMEM_SECTOR_USED_SIZE);
	for(uint32_t idx = 0; idx < FLASHMEM_END_USED_ADDRESS - FLASHMEM_START_ADDRESS; idx++)
	{
		*(uint8_t*)(memoryImage + idx) = *(uint8_t*)(FLASHMEM_START_ADDRESS + idx);
	}
}

int8_t flashMem_write2flash()
{
	#if FLASHMEM_DEBUG_LVL_USART >=1
	char debugMsg[25];
	#endif

	// Unlock memory + clear error flags
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGSERR );
	volatile HAL_StatusTypeDef halStatus;
	halStatus = HAL_FLASH_Unlock();
	FLASH_Erase_Sector(FLASHMEM_DEFAULT_SECTOR, FLASHMEM_DEFAULT_VRANGE);

	if(halStatus == HAL_OK)
	{
		uint32_t finalAddr = FLASHMEM_START_ADDRESS;
		
		for(uint32_t idx = 0; idx < FLASHMEM_END_USED_ADDRESS - FLASHMEM_START_ADDRESS; idx++)
		{

			while (halStatus == HAL_BUSY)
			{
				// Flash is busy, wait until it becomes available
				halStatus = HAL_FLASH_GetError();  // Check for the Flash status
			}

			halStatus = HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, finalAddr + idx, (uint64_t) *(uint8_t*)(memoryImage + idx));
		}

		HAL_FLASH_Lock();
	
		#if FLASHMEM_DEBUG_LVL_USART >=1
		sprintf(debugMsg, "FLASH_OK\n\r");
		USART2_addToQueue(debugMsg);
		#endif

		return 0;
	}
	HAL_FLASH_Lock();

	#if FLASHMEM_DEBUG_LVL_USART >=1
	sprintf(debugMsg, "FLASH_NOK_%d\n\r", (uint8_t)halStatus);
	USART2_addToQueue(debugMsg);
	#endif

	return -1; // NOK
}

int8_t flashMem_writeByte(uint8_t Data,  uint32_t Offset)
{
	*(uint8_t*)(memoryImage + Offset) = (uint8_t) Data;
	return 0;
}


int8_t flashMem_writeHalfWord(int16_t Data, uint32_t Offset)
{
	*(uint16_t*)(memoryImage + Offset) = (uint16_t) Data;
	return 0;
}

uint8_t flashMem_getByte(uint32_t Offset)
{
	return *((uint8_t*) (memoryImage + Offset));
}

uint16_t flashMem_getHalfWord(uint32_t Offset)
{
	return *((uint16_t*) (memoryImage + Offset));
}

int8_t flashMem_eraseSector()
{
	HAL_StatusTypeDef halStatus;
	halStatus = HAL_FLASH_Unlock();
	
	#if FLASHMEM_DEBUG_LVL_USART >=1
	char debugMsg[24];
	sprintf(debugMsg, "FLASH_ERASE_%d\n\r", halStatus);
	USART2_addToQueue(debugMsg);
	#endif

	if(halStatus == HAL_OK)
	{
		FLASH_Erase_Sector(FLASHMEM_DEFAULT_SECTOR, FLASHMEM_DEFAULT_VRANGE);
		HAL_FLASH_Lock();
		return 0;
	}
	return -1;
}


