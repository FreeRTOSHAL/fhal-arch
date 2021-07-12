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

#define PLIC_SCHIFT 1

#ifdef CONFIG_ARCH_RISCV_MMODE
# define CONTEX_ID(hartID) ((hartID << PLIC_SCHIFT))
#endif
#ifdef CONFIG_ARCH_RISCV_SMODE
# define CONTEX_ID(hartID) ((hartID << PLIC_SCHIFT) + 1)
#endif

static inline uint32_t PLIC_ClamIRQ() {
	uint32_t hartID = irq_getCPUID();
	uint32_t contexID = CONTEX_ID(hartID); 
	return PLIC->target[contexID].claim_complete;
}

static inline void PLIC_CompleteIRQ(uint32_t source) {
	uint32_t hartID = irq_getCPUID();
	uint32_t contexID = CONTEX_ID(hartID); 
	PLIC->target[contexID].claim_complete = source;
}

void irq_handler(uintptr_t cause, uintptr_t mepc) {
	if ((cause & CAUSE_INT) && ((cause & MCAUSE_CAUSE) == IRQ_EXT)) {
		uint32_t irqNr = PLIC_ClamIRQ();
		/* no pending IRQ? stop. why? */
		CONFIG_ASSERT(irqNr != 0);
		if (vector[irqNr] != NULL) {
			vector[irqNr]();
		}
		PLIC_CompleteIRQ(irqNr);
	} else if ((cause & CAUSE_INT) && ((cause & MCAUSE_CAUSE) == IRQ_TIMER)) {
#ifdef CONFIG_ARCH_SBI_TIMER
		extern void sbiTimerCallback();
		sbiTimerCallback();
#else
		CONFIG_ASSERT(0);
#endif
	} else {
		CONFIG_ASSERT(0);
	}
}

int32_t irq_init() {
	static bool init = false;
	int i;
	uint32_t hartID = irq_getCPUID();
	uint32_t contexID = CONTEX_ID(hartID); 
	if (init) {
		return 0;
	}
	init = true;
	/* Disable all interrupts for the current hart. */
	/* irq == 0 dosn't exists */
	for(i = 1; i < ((CONFIG_MACH_IRQ_COUNT + 32u) / 32u); i++) {
		PLIC->targetEnable[contexID].enables[i] = 0;
	}

	/* irq == 0 dosn't exists */
	for (i = 1; i < CONFIG_MACH_IRQ_COUNT; i++) {
		PLIC->prio[i] = 0;
	}
	CONFIG_ASSERT(offsetof(struct plic, targetEnable) == 0x2000);
	CONFIG_ASSERT(offsetof(struct plic, target) == 0x200000);

	PLIC->target[contexID].threshold = 0;
#ifdef CONFIG_ARCH_RISCV_MMODE
	set_csr(mie, BIT(IRQ_M_EXT));
#endif
#ifdef CONFIG_ARCH_RISCV_SMODE
	set_csr(sie, BIT(IRQ_S_EXT));
#endif
	return 0;
}

int32_t irq_enable(int32_t irqnr) {
	uint32_t hartID = irq_getCPUID();
	uint32_t current;
	uint32_t contexID = CONTEX_ID(hartID); 
	if (irqnr >= CONFIG_MACH_IRQ_COUNT) {
		return -1;
	}
	current = PLIC->targetEnable[contexID].enables[irqnr / 32];

	current |= BIT(irqnr % 32);

	PLIC->targetEnable[contexID].enables[irqnr / 32] = current;
	return 0;
}

int32_t irq_disable(int32_t irqnr) {
	uint32_t hartID = irq_getCPUID();
	uint32_t current;
	uint32_t contexID = CONTEX_ID(hartID); 
	if (irqnr >= CONFIG_MACH_IRQ_COUNT) {
		return -1;
	}
	current = PLIC->targetEnable[contexID].enables[irqnr / 32];

	current &= ~BIT(irqnr % 32);

	PLIC->targetEnable[contexID].enables[irqnr / 32] = current;
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
#ifdef CONFIG_ARCH_RISCV_MMODE
	return read_csr(mhartid);
#else
	return ulHartId;
#endif
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
