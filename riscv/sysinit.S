/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2021
 */
/* Primary Hardware thread ID */
#include <riscv.h>
#define PRIM_HART CONFIG_ARCH_ID

/* Define LOAD and STOR */
#if __riscv_xlen == 32
# define REGSIZE	4
# define REGSHIFT	2
# define LOAD		lw
# define STOR		sw
#elif __riscv_xlen == 64
# define REGSIZE	8
# define REGSHIFT	3
# define LOAD		ld
# define STOR		sd
#endif

#ifdef CONFIG_ARCH_RISCV_SMODE
	.extern ulHartId
#endif
	.section .init
	.globl reset_handler
	.type reset_handler,@function
reset_handler:
	.cfi_startproc
	.cfi_undefined ra

	/* disable irq and clear pending interrupts */
#ifdef CONFIG_ARCH_RISCV_MMODE
	csrw	mie, zero
	csrw	mip, zero
#endif
#ifdef CONFIG_ARCH_RISCV_SMODE
	csrw	sie, zero
	csrw	sip, zero
#endif

	li a1, PRIM_HART
/* in S-Mode this code is not needed */
#ifdef CONFIG_ARCH_RISCV_MMODE
	// Get Hardware thread ID if in S-Mode a0 after reset is the HartID
	csrr a0, mhartid // Read status register
	// If Secondary Threads jump to secondary startup
	bne  a0, a1, secondary
#endif
#ifdef CONFIG_ARCH_BOOT_UNDER_UBOOT
	bne a0, a1, secondary
#endif
#if defined(CONFIG_ARCH_SBI) && !defined(CONFIG_ARCH_BOOT_UNDER_UBOOT)
	bne a0, a1, startOtherHarts
#endif
#ifdef CONFIG_ARCH_RISCV_SMODE
	mv s1, a0 /* save HartID */
#endif

	/* clear registers */
	mv x1, zero // ra Return address Caller-saved
	mv x2, zero // sp Stack pointer Callee-saved
	mv x3, zero // gp Global pointer
	mv x4, zero // tp Thread pointer
	mv x5, zero // t0 Temporary/alternate link register Caller-saved
	mv x6, zero // t1 Temporaries Caller-saved
	mv x7, zero // t2 Temporaries Caller-saved
	mv x8, zero // s0/fp Saved register/frame pointer Callee-saved
#ifndef CONFIG_ARCH_RISCV_SMODE /* we use this register to save HartID in S-Mode */
	mv x9, zero // s1 Saved register Callee-saved 
#endif
	mv x10, zero // a0 Function arguments/return values Caller-saved
	mv x11, zero // a1 Function arguments/return values Caller-saved
	mv x12, zero // a2 Function arguments Caller-saved
	mv x13, zero // a3 Function arguments Caller-saved
	mv x14, zero // a4 Function arguments Caller-saved
	mv x15, zero // a5 Function arguments Caller-saved
/* RV32E Base Integer Instruction Set has no x16-x31 registers */
#ifndef __riscv_32e
	mv x16, zero // a6 Function arguments Caller-saved
	mv x17, zero // a7 Function arguments Caller-saved
	mv x18, zero // s2 Saved registers Callee-saved
	mv x19, zero // s3 Saved registers Callee-saved
	mv x20, zero // s4 Saved registers Callee-saved
	mv x21, zero // s5 Saved registers Callee-saved
	mv x22, zero // s6 Saved registers Callee-saved
	mv x23, zero // s7 Saved registers Callee-saved
	mv x24, zero // s8 Saved registers Callee-saved
	mv x25, zero // s9 Saved registers Callee-saved
	mv x26, zero // s10 Saved registers Callee-saved
	mv x27, zero // s11 Saved registers Callee-saved
	mv x28, zero // t3 Temporaries Caller-saved
	mv x29, zero // t4 Temporaries Caller-saved
	mv x30, zero // t5 Temporaries Caller-saved
	mv x31, zero // t6 Temporaries Caller-saved
#endif
#ifdef CONFIG_ARCH_RISCV_HARDWARE_FLOAD
	li t0, SR_FS_INITIAL
#ifdef CONFIG_ARCH_RISCV_MMODE
	csrs mstatus, t0
#endif
#ifdef CONFIG_ARCH_RISCV_SMODE
	csrs sstatus, t0
#endif
#endif

	/* Load Global Pointer  */
.option push
.option norelax
	la  gp, __global_pointer$
.option pop


	// Set Stack Pointer
	la sp, _end_stack
#ifdef ARCH_SYSINIT_EARLY_INIT
	call sysinit_early_init
#endif
	
	// Init Data and BSS
	call datainit
	call bssinit
	call irq_init
	call clock_init

#ifdef CONFIG_ARCH_RISCV_SMODE
	la t0, ulHartId	
	sw s1, 0(t0)
#endif

	// Jump Main
	call main
1:
	wfi
	j 1b

/* if we run under U-Boot all Harts are started */
#if defined(CONFIG_ARCH_SBI) && !defined(CONFIG_ARCH_BOOT_UNDER_UBOOT)
startOtherHarts:
	/* check PRIM_HART is allredy started */
	/* sbi_hart_status */
	li a0, PRIM_HART /* HartID */
	li a6, 2 /* fid */
	li a7, 0x48534D /* ext */
	ecall
	blt a0, zero, error /* check request failed */
	li t0, 0 /* Already started */
	beq a1, t0, secondary /* Already started so we can sleep */
	li t0, 2 /* A start request pending */
	beq a1, t0, secondary /* Already started so we can sleep */

	/* sbi_hart_start */
	li a0, PRIM_HART /* HartID */
	la a1, reset_handler /* start_addr */
	li a2, __riscv_xlen /* XLEN */
	li a6, 0 /* fid */
	li a7, 0x48534D /* ext */
	/* after this we jump to secondary core */
	ecall
	blt a0, zero, error /* check request failed */
	/* finaly jump to secondary code */
	j secondary

error:
	wfi
	j error
#endif

secondary:
#ifndef CONFIG_ARCH_SBI
	/* sbi_hart_stop */
	li a6, 1 /* fid */
	li a7, 0x48534D /* ext */
	ecall
#endif
1:
	wfi
	j 1b
	.cfi_endproc
