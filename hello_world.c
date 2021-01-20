/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"

#include "pin_mux.h"
#include "clock_config.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/


/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */

#include "stdio.h"

__attribute__((naked)) void rgb32Torgb565(uint32_t *src, uint16_t *dst, uint32_t len){
	__asm volatile (
	"push {r3-r11, lr} \n"	
	// handle the remaining 8bytes first 
	"loop1: \n"
	"	ldmia r0!, {r4-r11} \n"
	
	"	lsrs r4, #3 \n"      // pix 1
	"	lsls r5, #8 \n"		// pix 2

	"	ubfx r3,r4,#8+2-3, #6 \n"  //green
	"	ubfx ip,r5,#8+2+8, #6 \n"
	
	"	bfi r4,r3,#0+5,#6 \n"
	"	bfi r5,ip,#16+5,#6 \n"
	
	"	ubfx r3,r4,#16+3-3,#5 \n" //red
	"	ubfx ip,r5,#3+8,#5 \n"   //blue

	"	bfi r4,r3,#5+6,#5 \n"
	"	bfi r5,ip,#16,#5 \n"
	
	"	bfi r5,r4,#0,#16 \n"
	"	nop \n"
	
	"	lsrs r6, #3 \n"      // pix 3
	"	lsls r7, #8 \n"		// pix 4

	"	ubfx r3,r6,#8+2-3, #6 \n"  //green
	"	ubfx ip,r7,#8+2+8, #6 \n"
	
	"	bfi r6,r3,#0+5,#6 \n"
	"	bfi r7,ip,#16+5,#6 \n"
	
	"	ubfx r3,r6,#16+3-3,#5 \n" //red
	"	ubfx ip,r7,#3+8,#5 \n"   //blue

	"	bfi r6,r3,#5+6,#5 \n"
	"	bfi r7,ip,#16,#5 \n"
	
	"	bfi r7,r6,#0,#16 \n"
	"	nop \n"
	
	"	lsrs r8, #3 \n"      // pix 5
	"	lsls r9, #8 \n"		// pix 6

	"	ubfx r3,r8,#8+2-3, #6 \n"  //green
	"	ubfx ip,r9,#8+2+8, #6 \n"
	
	"	bfi r8,r3,#0+5,#6 \n"
	"	bfi r9,ip,#16+5,#6 \n"
	
	"	ubfx r3,r8,#16+3-3,#5 \n" //red
	"	ubfx ip,r9,#3+8,#5 \n"   //blue

	"	bfi r8,r3,#5+6,#5 \n"
	"	bfi r9,ip,#16,#5 \n"
	
	"	bfi r9,r8,#0,#16 \n"
	"	nop \n"
	
	"	lsrs r10, #3 \n"      // pix 7
	"	lsls r11, #8 \n"		// pix 8

	"	ubfx r3,r10,#8+2-3, #6 \n"  //green
	"	ubfx ip,r11,#8+2+8, #6 \n"
	
	"	bfi r10,r3,#0+5,#6 \n"
	"	bfi r11,ip,#16+5,#6 \n"
	
	"	ubfx r3,r10,#16+3-3,#5 \n" //red
	"	ubfx ip,r11,#3+8,#5 \n"   //blue

	"	bfi r10,r3,#5+6,#5 \n"
	"	bfi r11,ip,#16,#5 \n"
	
	"	bfi r11,r10,#0,#16 \n"
	"	nop \n"
		
	"	stmia r1!,{r5,r7,r9,r11} \n"
	"	subs r2, #32 \n"
	"	bne loop1 \n"
	"	pop {r3-r11, pc} \n"		
		);
	}

volatile uint32_t tick;
void SysTick_Handler(){
//	__asm volatile (
//		"add %0, #1"
//	::"+r"(tick):"memory");	
	tick++;
}
#define ALIGN(n)  __attribute__((aligned(n)))
#define OFFSET 8
__attribute__((section(".sdram"))) ALIGN(32) uint32_t test_array[1280*720 + OFFSET/sizeof(uint32_t)];
__attribute__((section(".sdram"))) ALIGN(32) uint16_t dst[1280*720];
int main(void)
{
    char ch;

    /* Init board hardware. */
    BOARD_ConfigMPU();
    BOARD_InitPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();
	
	SysTick_Config(CLOCK_GetCpuClkFreq()/1000);
	
	PRINTF("BenchMark for the RGB32TORGB565 ASM function \r\n");
	memset(test_array, 0x56, sizeof(test_array) - OFFSET);
	
	SCB_CleanInvalidateDCache();
	uint32_t align_start = tick;
	rgb32Torgb565((void*)test_array, dst, sizeof(test_array) - OFFSET);
	uint32_t align_end = tick;
	PRINTF("Time costs with align address (0x%x): %d ms\r\n", test_array, align_end - align_start);
	
	SCB_CleanInvalidateDCache();
	uint32_t unalign_start = tick;
	rgb32Torgb565((void*)test_array + OFFSET, dst, sizeof(test_array) - OFFSET);
	uint32_t unalign_end = tick;
	PRINTF("Time costs with unalign address (0x%x): %d ms\r\n", (void*)test_array + OFFSET, unalign_end - unalign_start);
	

    PRINTF("hello world.\r\n");

    while (1)
    {
        ch = GETCHAR();
        PUTCHAR(ch);
    }
}
