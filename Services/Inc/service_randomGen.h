/**
 * Random number generator
 * service_randomGen.h
 *
 *  Created on: June 17, 2022
 *      Author: Matheus Sozza
 */

#ifndef INC_CORE_SERVICES_RANDOMGEN_H_
#define INC_CORE_SERVICES_RANDOMGEN_H_

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Defines -------------------------------------------------------------------*/
#define randomGen_random32b() DWT->CYCCNT // Generates random num. based on CYCCNT

/* Typedef declare -----------------------------------------------------------*/


/* Function prototypes -------------------------------------------------------*/


#endif /* INC_CORE_SERVICES_RANDOMGEN_H_ */




