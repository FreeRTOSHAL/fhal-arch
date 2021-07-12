/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2021
 */
#ifndef PLIC_H_
#define PLIC_H_

#include <stdint.h>
struct plic_irqTarget {
	uint32_t threshold;
	uint32_t claim_complete;
	uint32_t reserved[1022];
};

struct plic_enable {
	uint32_t enables[32];
};

struct plic {
	uint32_t prio[1024]; /* 0x0000 - 0x0FFF */
	uint32_t pending[32]; /* 0x1000 - 0x107F */
	uint32_t reserved[992]; /* 0x1080 - 0x1FFF */
	struct plic_enable targetEnable[15872]; /* 0x2000 - 0x1F1FFF */
	uint32_t reserved2[14336]; /* 0x1F2000 - 0x1FFFFF */
	struct plic_irqTarget target[15872]; /* 0x200000 - 0x3FFFFFF */
};
typedef void (*vector_table_entry_t)(void);
extern vector_table_entry_t vector[CONFIG_MACH_IRQ_COUNT];
extern volatile struct plic *PLIC;

#endif
