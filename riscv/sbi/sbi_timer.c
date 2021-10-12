#include <FreeRTOS.h>
#include <task.h>
#include <sbi.h>
#include <stdint.h>
#include <riscv.h>


extern size_t uxTimerIncrementsForOneTick;
static volatile uint64_t next;

/** read 64-bit time */
static inline uint64_t riscv_rdtime(void)
{
#ifdef __LP64__
        uint64_t n;
        __asm__ volatile ("rdtime %0" : "=r" (n));
        return n;
#else
        uint32_t u1;
        uint32_t lo;
        uint32_t u2;
        asm volatile (
                "1:\n"
                "rdtimeh        %0\n"
                "rdtime         %1\n"
                "rdtimeh        %2\n"
                "bne            %0, %2, 1b\n"
                : "=&r"(u1), "=&r"(lo), "=&r"(u2));
        return ((uint64_t)u1 << 32) | lo;
#endif
}

void vPortSetupTimerInterrupt() {
	uint64_t mtime = riscv_rdtime();
	next = mtime + uxTimerIncrementsForOneTick;
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
