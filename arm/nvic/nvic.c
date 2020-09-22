/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2016
 */
#include <stdint.h>
#include <irq.h>
#include <vector.h>
#if defined CONFIG_ARCH_ARM_CORTEX_M0
# include <core_cm0.h>
#elif defined CONFIG_ARCH_ARM_CORTEX_M3
# include <core_cm3.h>
#elif defined CONFIG_ARCH_ARM_CORTEX_M4F
# include <core_cm4.h>
#else
# error "Arch unknown"
#endif


int32_t irq_init() {
	return 0;
}
int32_t irq_enable(int32_t irqnr) {
	NVIC_EnableIRQ(irqnr);
	return 0;
}
int32_t irq_disable(int32_t irqnr) {
	NVIC_DisableIRQ(irqnr);
	return 0;
}
int32_t irq_notify(int32_t cpuid, int32_t irqnr) {
	return -1;
}
int32_t irq_clear(int32_t irqnr) {
	return -1;
}
int32_t irq_setPrio(int32_t irqnr, int32_t prio) {
	NVIC_SetPriority(irqnr, prio);
	return 0;
}
int32_t irq_getPrio(int32_t irqnr) {
	return  NVIC_GetPriority(irqnr);
}
int32_t irq_mask(int32_t irqnr) {
	return -1;
}
int32_t irq_unmask(int32_t irqnr) {
	return -1;
}
