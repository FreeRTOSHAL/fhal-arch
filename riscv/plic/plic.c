/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2021
 */
#include <FreeRTOS.h>
#include <system.h>
#include <stdint.h>
#include <stdlib.h>
#include <riscv.h>
#include <irq.h>
#include <plic.h>
#include <stdbool.h>

static inline uint32_t PLIC_ClamIRQ() {
	uint32_t hartID = irq_getCPUID();
	return PLIC->target[hartID].claim_complete;
}
static inline void PLIC_CompleteIRQ(uint32_t source) {
	uint32_t hartID = irq_getCPUID();
	PLIC->target[hartID].claim_complete = source;
}

void irq_handler(uintptr_t mcause, uintptr_t mepc) {
	if ((mcause & MCAUSE_INT) && ((mcause & MCAUSE_CAUSE) == IRQ_M_EXT)) {
		uint32_t irqNr = PLIC_ClamIRQ();
		/* no pending IRQ? stop. why? */
		CONFIG_ASSERT(irqNr != 0);
		if (vector[irqNr] != NULL) {
			vector[irqNr]();
		}
		PLIC_CompleteIRQ(irqNr);
	} else {
		CONFIG_ASSERT(0);
	}
}

int32_t irq_init() {
	static bool init = false;
	int i;
	uint32_t hartID = irq_getCPUID();
	if (init) {
		return 0;
	}
	init = true;
	/* Disable all interrupts for the current hart. */
	for(i = 0; i < ((CONFIG_MACH_IRQ_COUNT + 32u) / 32u); i++) {
		PLIC->targetEnable[hartID].enables[i] = 0;
	}
	/* if we the first Hart set up the prio */
	if (hartID == 0) {
		for (i = 0; i < CONFIG_MACH_IRQ_COUNT; i++) {
			PLIC->prio[i] = 0;
		}
	}

	PLIC->target[hartID].threshold = 0;

	set_csr(mie, BIT(IRQ_M_EXT));
	return 0;
}

int32_t irq_enable(int32_t irqnr) {
	uint32_t hartID = irq_getCPUID();
	uint32_t current;
	if (irqnr >= CONFIG_MACH_IRQ_COUNT) {
		return -1;
	}
	current = PLIC->targetEnable[hartID].enables[irqnr / 32];

	current |= BIT(irqnr % 32);

	PLIC->targetEnable[hartID].enables[irqnr / 32] = current;
	return 0;
}

int32_t irq_disable(int32_t irqnr) {
	uint32_t hartID = irq_getCPUID();
	uint32_t current;
	if (irqnr >= CONFIG_MACH_IRQ_COUNT) {
		return -1;
	}
	current = PLIC->targetEnable[hartID].enables[irqnr / 32];

	current &= ~BIT(irqnr % 32);

	PLIC->targetEnable[hartID].enables[irqnr / 32] = current;
	return 0;
}

int32_t irq_notify(int32_t cpuid, int32_t irqnr) {
	//TODO
	return -1;
}

int32_t irq_clear(int32_t irqnr) {
	//TODO
	return -1;
}

int32_t irq_getCPUID() {
	return read_csr(mhartid);
}

int32_t irq_setPrio(int32_t irqnr, int32_t prio) {
	if (irqnr >= CONFIG_MACH_IRQ_COUNT) {
		return -1;
	}
	if (prio >= CONFIG_MACH_IRQ_MAX_PRIO) {
		prio = CONFIG_MACH_IRQ_MAX_PRIO - 1;
	}
	PLIC->prio[irqnr] = prio;
	return 0;
}

int32_t irq_getPrio(int32_t irqnr) {
	if (irqnr >= CONFIG_MACH_IRQ_COUNT) {
		return -1;
	}
	return PLIC->prio[irqnr];
}

int32_t irq_mask(int32_t irqnr) {
	//TODO
	return -1;
}

int32_t irq_unmask(int32_t irqnr) {
	//TODO
	return -1;
}
