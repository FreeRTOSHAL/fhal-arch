#include <FreeRTOS.h>
#include <task.h>
#include <sbi.h>
#include <stdint.h>
#include <riscv.h>


extern size_t uxTimerIncrementsForOneTick;
static volatile uint64_t *mtime = (uint64_t *) CONFIG_MACH_MTIME_BASE_ADDRESS;
static volatile uint64_t next;

void vPortSetupTimerInterrupt() {
	next = *mtime + uxTimerIncrementsForOneTick;
	set_csr(sie, 0x20); /* enable timer interrupt */
	sbi_set_timer(next);
}

void sbiTimerCallback() {
	BaseType_t ret;
	next = next + uxTimerIncrementsForOneTick;
	sbi_set_timer(next);
	ret = xTaskIncrementTick();
	if (ret != 0) {
		vTaskSwitchContext();
	}
}
