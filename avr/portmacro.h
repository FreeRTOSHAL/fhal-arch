/*
 * FreeRTOS Kernel V10.2.1
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */


#ifndef PORTMACRO_H
#define PORTMACRO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <avr/interrupt.h>

/*-----------------------------------------------------------
 * Port specific definitions.
 *
 * The settings in this file configure FreeRTOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */

/* Type definitions. */
#define portCHAR		char
#define portFLOAT		float
#define portDOUBLE		double
#define portLONG		long
#define portSHORT		short
#define portSTACK_TYPE	uint32_t
#define portBASE_TYPE	long

typedef portSTACK_TYPE StackType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;

#if( configUSE_16_BIT_TICKS == 1 )
	typedef uint16_t TickType_t;
	#define portMAX_DELAY ( TickType_t ) 0xffff
#else
	typedef uint32_t TickType_t;
	#define portMAX_DELAY ( TickType_t ) 0xffffffffUL

	/* 32-bit tick type on a 32-bit architecture, so reads of the tick count do
	not need to be guarded with a critical section. */
	#define portTICK_TYPE_IS_ATOMIC 1
#endif
/*-----------------------------------------------------------*/

/* Architecture specifics. */
#define portSTACK_GROWTH			( -1 )
#if configTICK_RATE_HZ > 1000
#error "don't to tick rate >1000 with an avr!"
#else
# define portTICK_PERIOD_MS                    ( ( TickType_t ) 1000 / configTICK_RATE_HZ )
#endif
#if CONFIG_TICK_RATE_HZ > 1000
#error "don't to tick rate >1000 with an avr!"
#else
# define portTICK_PERIOD_US                     TICK_PERIOD_US_NOT_SUPPORED
#endif
#define portBYTE_ALIGNMENT			8
#define F_CPU					CONFIG_CPU_CLOCK_HZ
/* Start tasks with interrupts enables. */
#define portFLAGS_INT_ENABLED                                   ( ( StackType_t ) 0x80 )

/* Hardware constants for timer 0. */
#define portCLEAR_COUNTER_ON_MATCH                              ( ( uint8_t ) 0x08 )
#define portPRESCALE_1024                                       ( ( uint8_t ) 0x05 )
#define portPRESCALE_256                                        ( ( uint8_t ) 0x04 )
#define portPRESCALE_64                                         ( ( uint8_t ) 0x03 )
#define portPRESCALE_8                                          ( ( uint8_t ) 0x02 )
#define portPRESCALE_1                                          ( ( uint8_t ) 0x01 )
#define portCLOCK_PRESCALER                                     ( ( uint32_t ) 1024 )
#define portCLOCK_PRESCALER_BITS                portPRESCALE_1024
#define portCOMPARE_MATCH_A_INTERRUPT_ENABLE    ( ( uint8_t ) 0x02 )
/*-----------------------------------------------------------*/


/* Scheduler utilities. */
extern void vPortYield( void ) __attribute__ ( ( naked ) );
#define portYIELD()					vPortYield()
/*-----------------------------------------------------------*/


/* Critical section management. */
extern void vPortEnterCritical( void );
extern void vPortExitCritical( void );

/* Critical section management. */
#define portENTER_CRITICAL()            asm volatile ( "in              __tmp_reg__, __SREG__" :: );    \
                                        asm volatile ( "cli" :: );                                                              \
                                        asm volatile ( "push    __tmp_reg__" :: )

#define portEXIT_CRITICAL()                     asm volatile ( "pop             __tmp_reg__" :: );                              \
                                                asm volatile ( "out             __SREG__, __tmp_reg__" :: )

#define portDISABLE_INTERRUPTS()        asm volatile ( "cli" :: );
#define portENABLE_INTERRUPTS()         asm volatile ( "sei" :: );

/*-----------------------------------------------------------*/

/* Task function macros as described on the FreeRTOS.org WEB site. */
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) void vFunction( void *pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters ) void vFunction( void *pvParameters )

#define portNOP()

#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */

