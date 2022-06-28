/*
 * Delay Service based on clock cycles
 * service_cycleDelay.h
 *
 * Delay function with empty assembly instructions, for hardware operations
 * that need time to be acomplished.
 *
 *  Created on: June 27, 2022
 *      Author: Matheus Sozza
 */

#ifndef INC_CORE_SERVICES_CYCLEDELAY_H_
#define INC_CORE_SERVICES_CYCLEDELAY_H_

    #define CYCLE_DELAY_1_TICK() asm("nop")
    #define CYCLE_DELAY_2_TICKS() asm("nop"); CYCLE_DELAY_1_TICK()
    #define CYCLE_DELAY_3_TICKS() asm("nop"); CYCLE_DELAY_2_TICKS()
    #define CYCLE_DELAY_4_TICKS() asm("nop"); CYCLE_DELAY_3_TICKS()
    #define CYCLE_DELAY_5_TICKS() asm("nop"); CYCLE_DELAY_4_TICKS()

    #define CYCLE_DELAY_10_TICKS() CYCLE_DELAY_5_TICKS(); CYCLE_DELAY_5_TICKS()
    #define CYCLE_DELAY_20_TICKS() CYCLE_DELAY_10_TICKS(); CYCLE_DELAY_10_TICKS()
    #define CYCLE_DELAY_40_TICKS() CYCLE_DELAY_20_TICKS(); CYCLE_DELAY_20_TICKS()
    #define CYCLE_DELAY_80_TICKS() CYCLE_DELAY_40_TICKS(); CYCLE_DELAY_40_TICKS()

#endif /* INC_CORE_SERVICES_CYCLEDELAY_H_ */
