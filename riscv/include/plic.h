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
	uint32_t prio[1024];
	uint32_t pending[32];
	uint32_t reserved[992];
	struct plic_enable targetEnable[15808];
	uint32_t reserved2[16384];
	struct plic_irqTarget target[15872];
};
typedef void (*vector_table_entry_t)(void);
extern vector_table_entry_t vector[CONFIG_MACH_IRQ_COUNT];
extern volatile struct plic *PLIC;

#endif
