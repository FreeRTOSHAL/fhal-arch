/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2021
 */
#ifndef FREEROTS_ARCH_CONFIG_H_
#define FREEROTS_ARCH_CONFIG_H_

#define portasmHANDLE_INTERRUPT irq_handler

#ifdef CONFIG_ARCH_RISCV_HAS_MTIME
# define portasmHAS_MTIME 1
# define configMTIME_BASE_ADDRESS CONFIG_MACH_MTIME_BASE_ADDRESS
# define configMTIMECMP_BASE_ADDRESS CONFIG_MACH_MTIMECMP_BASE_ADDRESS
#endif

#ifdef CONFIG_ARCH_RISCV_SEP_IRQ_STACK
# define configISR_STACK_SIZE_WORDS CONFIG_ARCH_RISCV_ISR_STACK_SIZE_WORDS
#endif

#ifdef CONFIG_ARCH_RISCV_HAS_CLINT
# define portasmHAS_SIFIVE_CLINT
#endif

#endif
