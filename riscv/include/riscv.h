/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2021
 */
#ifndef RISC_V_
#define RISC_V_

#ifndef __ASSEMBLY__
#include <stdint.h>
#endif

#if __riscv_xlen == 64
# define MCAUSE_CAUSE           0x7FFFFFFFFFFFFFFF
# define MCAUSE_INT             0x8000000000000000
#else
# define MCAUSE_CAUSE           0x7FFFFFFF
# define MCAUSE_INT             0x80000000
#endif

#if __riscv_xlen == 64
# define CAUSE_CAUSE           0x7FFFFFFFFFFFFFFF
# define CAUSE_INT             0x8000000000000000
#else
# define CAUSE_CAUSE           0x7FFFFFFF
# define CAUSE_INT             0x80000000
#endif

#define SR_FS (3 << 13)
#define SR_FS_INITIAL (1 << 13)
#define SR_FS_CLEAN (2 << 13)
#define SR_FS_DIRTY (3 << 13)

#define IRQ_S_SOFT          1
#define IRQ_H_SOFT          2
#define IRQ_M_SOFT          3
#define IRQ_S_TIMER         5
#define IRQ_H_TIMER         6
#define IRQ_M_TIMER         7
#define IRQ_S_EXT           9
#define IRQ_H_EXT           10
#define IRQ_M_EXT           11

#ifdef CONFIG_ARCH_RISCV_MMODE
# define IRQ_EXT IRQ_M_EXT
# define IRQ_TIMER IRQ_M_TIMER
# define IRQ_SOFT IRQ_M_SOFT
#endif
#ifdef CONFIG_ARCH_RISCV_SMODE
# define IRQ_EXT IRQ_S_EXT
# define IRQ_TIMER IRQ_S_TIMER
# define IRQ_SOFT IRQ_S_SOFT
#endif

#ifndef __ASSEMBLY__
#define read_csr(reg) ({ \
	unsigned long __tmp; \
	asm volatile ("csrr %0, " #reg : "=r"(__tmp)); \
	__tmp; \
})

#define write_csr(reg, val) ({ \
  if (__builtin_constant_p(val) && (unsigned long)(val) < 32) \
    asm volatile ("csrw " #reg ", %0" :: "i"(val)); \
  else \
    asm volatile ("csrw " #reg ", %0" :: "r"(val)); })

#define swap_csr(reg, val) ({ unsigned long __tmp; \
  if (__builtin_constant_p(val) && (unsigned long)(val) < 32) \
    asm volatile ("csrrw %0, " #reg ", %1" : "=r"(__tmp) : "i"(val)); \
  else \
    asm volatile ("csrrw %0, " #reg ", %1" : "=r"(__tmp) : "r"(val)); \
  __tmp; })

#define set_csr(reg, bit) ({ unsigned long __tmp; \
  if (__builtin_constant_p(bit) && (unsigned long)(bit) < 32) \
    asm volatile ("csrrs %0, " #reg ", %1" : "=r"(__tmp) : "i"(bit)); \
  else \
    asm volatile ("csrrs %0, " #reg ", %1" : "=r"(__tmp) : "r"(bit)); \
  __tmp; })

#define clear_csr(reg, bit) ({ unsigned long __tmp; \
  if (__builtin_constant_p(bit) && (unsigned long)(bit) < 32) \
    asm volatile ("csrrc %0, " #reg ", %1" : "=r"(__tmp) : "i"(bit)); \
  else \
    asm volatile ("csrrc %0, " #reg ", %1" : "=r"(__tmp) : "r"(bit)); \
  __tmp; })
#endif

#endif
