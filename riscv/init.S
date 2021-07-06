/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2021
 */
	.extern _data_table
	.extern _data_table_end
	.extern __bss_start__
	.extern __bss_end__

/* Define LOAD and STOR */
#if __riscv_xlen == 32
#define REGSIZE		4
#define REGSHIFT	2
#define LOAD		lw
#define STOR		sw
#elif __riscv_xlen == 64
#define REGSIZE		8
#define REGSHIFT	3
#define LOAD		ld
#define STOR		sd
#endif

	.section .text
	.globl datainit
	.type datainit,@function
	/* a0 data_table pointer */
	/* a1 data_table_end */
	/* a2 src pointer */
	/* a3 dst pointer */
	/* a4 len Length*/
	/* t0 data */
	/* t1 i */
datainit:
	la a0, _data_table
	la a1, _data_table_end
1:
	LOAD a2, (a0) /* src */
	addi a0, a0, REGSIZE
	LOAD a3, (a0) /* dst */
	addi a0, a0, REGSIZE
	LOAD a4, (a0) /* len */
	addi a0, a0, REGSIZE

	mv t1, zero /* init i to 0 */
	j 3f
2:
	LOAD t0, (a2)
	STOR t0, (a3)
	addi a2, a2, REGSIZE
	addi a3, a3, REGSIZE
	addi t1, t1, REGSIZE
3:
	bltu t1, a4, 2b /* if more data is to copy */
	bltu a0, a1, 1b /* if there more in data table */

	ret

	.section .text
	.globl bssinit
	.type bssinit,@function
bssinit:
	la a0, __bss_start__
	la a1, __bss_end__
	bgeu a0, a1, 2f
1:
	STOR zero, (a0)
	addi a0, a0, REGSIZE
	bltu a0, a1, 1b
2:
	ret

