#include <stdint.h>
#include <semihosting.h>
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


uint32_t semihosting_systemCall(uint32_t r1, uint32_t r2) {
	uint32_t ret;
	asm volatile (
		"mov r0, %1\n"
		"mov r1, %2\n"
		"bkpt #0xAB\n"
		"mov %0, r0"
	:"=r" (ret) : "r" (r1), "r" (r2): "r0", "r1");
	return ret;
}

bool semihosting_debugIsEnabled() {
	return (CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk) != 0;
}
