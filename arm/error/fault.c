#include <stdio.h>
#include <stdbool.h>
#include <system.h>
#include <vector.h>
#include <core_cm4.h>
#include <FreeRTOS.h>
#include <task.h>
/* 
 * MemManage Fault Status Register
 */
#define IACCVIOL BIT(0)
#define DACCVIOL BIT(1)
#define MUNSTKERR BIT(3)
#define MSTKERR BIT(4)
#define MLSPERR BIT(5)
#define MMARVALID BIT(7)
/* 
 * BusFault Status Register
 */
#define IBUSERR BIT(0)
#define PRECISERR BIT(1)
#define IMPRECISERR BIT(2)
#define UNSTKERR BIT(3)
#define STKERR BIT(4)
#define LSPERR BIT(5)
#define BFARVALID BIT(7)
/*
 * UsageFault 
 */
#define UNDEFINSTR BIT(0)
#define INVSTATE BIT(1)
#define INVPC BIT(2)
#define NOCP BIT(3)
#define UNALIGNED BIT(8)
#define DIVBYZERO BIT(9)
/* 
 * HardFault Status Register
 */
#define VECTTBL BIT(1)
#define FORCED BIT(30)
#ifdef CONFIG_ARCH_ARM_PRINT_ERROR
#define PRINT_ERROR(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define PRINT_ERROR(fmt, ...) asm("dbg #0");for(;;);
#endif

void fault_handlerC(uint32_t *hardfault_args);
__attribute__((naked)) void fault_handler(void) {
        /*
         * Get the appropriate stack pointer, depending on our mode,
         * and use it as the parameter to the C handler. This function
         * will never return
         */

        __asm(  
                        "MOVS   R0, #4  \n"
                        "MOV    R1, LR  \n"
                        "TST    R0, R1  \n"
                        "BEQ    1f    \n"
                        "MRS    R0, PSP \n"
                        "B      fault_handlerC      \n"
			"BX     LR \n"
                "1:  \n"
                        "MRS    R0, MSP \n"
                        "B      fault_handlerC      \n"
			"BX     LR \n"
	) ;
}
void fault_handlerC(uint32_t *hardfault_args) {
	volatile uint32_t stacked_r0 ;
	volatile uint32_t stacked_r1 ;
	volatile uint32_t stacked_r2 ;
	volatile uint32_t stacked_r3 ;
	volatile uint32_t stacked_r12 ;
	volatile uint32_t stacked_lr ;
	volatile uint32_t stacked_pc ;
	volatile uint32_t stacked_psr ;
	SCB_Type *scb = SCB;
	volatile uint32_t MMFSR;
	volatile uint32_t BFSR;
	volatile uint32_t UFSR;
	volatile uint32_t HFSR;
	bool stackValid = false;

	stacked_r0 = ((uint32_t)hardfault_args[0]) ;
	stacked_r1 = ((uint32_t)hardfault_args[1]) ;
	stacked_r2 = ((uint32_t)hardfault_args[2]) ;
	stacked_r3 = ((uint32_t)hardfault_args[3]) ;
	stacked_r12 = ((uint32_t)hardfault_args[4]) ;
	stacked_lr = ((uint32_t)hardfault_args[5]) ;
	stacked_pc = ((uint32_t)hardfault_args[6]) ;
	stacked_psr = ((uint32_t)hardfault_args[7]) ;
	MMFSR = (scb->CFSR & SCB_CFSR_MEMFAULTSR_Msk) >> SCB_CFSR_MEMFAULTSR_Pos;
	BFSR = (scb->CFSR & SCB_CFSR_BUSFAULTSR_Msk) >> SCB_CFSR_BUSFAULTSR_Pos;
	UFSR = (scb->CFSR & SCB_CFSR_USGFAULTSR_Msk) >> SCB_CFSR_USGFAULTSR_Pos;
	HFSR = scb->HFSR;
	if (HFSR & FORCED) {
#ifdef CONFIG_ARCH_ARM_PRINT_ERROR
		PRINT_ERROR("HardFault\n");
#endif
	}
	if (HFSR & VECTTBL) {
#ifdef CONFIG_ARCH_ARM_PRINT_ERROR
		PRINT_ERROR("HardFault: BusFault on a vector table read during exception processing");
#endif
	}
	/*
	 * Check is MemManage Fault
	 */
	if (MMFSR & MMARVALID) {
		/* 
		 * This fault occurs on any access to an XN region, even when the MPU is disabled or not present.
		 * When this bit is 1, the PC value stacked for the exception return points to the faulting
		 * instruction. The processor has not written a fault address to the MMAR.
		 */
		if (MMFSR & IACCVIOL) {
			PRINT_ERROR("MemManage Fault: Illeagal Instruction\n");
			stackValid = true;
		}
		/*
		 * When this bit is 1, the PC value stacked for the exception return points to the faulting
		 * instruction. The processor has loaded the MMAR with the address of the attempted access.
		 */
		if (MMFSR & DACCVIOL) {
			PRINT_ERROR("MemManage Fault: Illeagal Memmory Access at address: 0x%lx\n", scb->MMFAR);
			stackValid = true;
		}
		/* 
		 * This fault is chained to the handler. This means that when this bit is 1, the original return stack
		 * is still present. The processor has not adjusted the SP from the failing return, and has not
		 * performed a new save. The processor has not written a fault address to the MMAR.
		 */
		if (MMFSR & MUNSTKERR) {
			PRINT_ERROR("MemManage Fault: On unstacking for a return from exception\n");
			stackValid = true;
		}
		/* 
		 * When this bit is 1, the SP is still adjusted but the values in the context area on the stack might
		 * be incorrect. The processor has not written a fault address to the MMAR.
		 */
		if (MMFSR & MSTKERR) {
			PRINT_ERROR("MemManage Fault: On stacking for exception entry\n");
		}
		/*
		 * MemManage fault occurred during floating-point lazy state preservation.
		 */
		if (MMFSR & MLSPERR) {
			PRINT_ERROR("MemManage Fault: occurred during floating-point lazy state preservation");
			stackValid = true;
		}
	}
	/*
	 * Check is BusFault
	 */
	if (BFSR & BFARVALID) {
		/*
		 * This fault is chained to the handler. This means that when the processor sets this bit to 1, the original return
		 * stack is still present. The processor does not adjust the SP from the failing return, does not performed a
		 * new save, and does not write a fault address to the BFAR.
		 */
		if (BFSR & IBUSERR) {
			PRINT_ERROR("BusFault\n");
			stackValid = true;
		}
		/* 
		 * a data bus error has occurred, and the PC value stacked for the exception return points to the instruction
		 * that caused the fault.
		 */
		if (BFSR & PRECISERR) {
			PRINT_ERROR("BusFault: Illegal Address: 0x%lx\n", scb->BFAR);
			stackValid = true;
		}
		/*
		 * When the processor sets this bit to 1, it does not write a fault address to the BFAR.
		 * This is an asynchronous fault. Therefore, if it is detected when the priority of the current process is higher
		 * than the BusFault priority, the BusFault becomes pending and becomes active only when the processor
		 * returns from all higher priority processes. If a precise fault occurs before the processor enters the handler
		 * for the imprecise BusFault, the handler detects both IMPRECISERR set to 1 and one of the precise fault
		 * status bits set to 1.
		 */
		if (BFSR & IMPRECISERR) {
			PRINT_ERROR("BusFault: in Exeption Handler\n");
		}
		/* 
		 * This fault is chained to the handler. This means that when the processor sets this bit to 1, the original return
		 * stack is still present. The processor does not adjust the SP from the failing return, does not performed a
		 * new save, and does not write a fault address to the BFAR.
		 */
		if (BFSR & UNSTKERR) {
			PRINT_ERROR("BusFault\n");
			stackValid = true;
		}
		/* 
		 * When the processor sets this bit to 1, the SP is still adjusted but the values in the context area on the stack
		 * might be incorrect. The processor does not write a fault address to the BFAR.
		 */
		if (BFSR & STKERR) {
			PRINT_ERROR("BusFault\n");
		}
		if (BFSR & LSPERR) {
			PRINT_ERROR("BusFault: during floating-point lazy state preservation\n");
			stackValid = true;
		}
	}
	/* 
	 * When this bit is set to 1, the PC value stacked for the exception return points to the undefined instruction.
	 * An undefined instruction is an instruction that the processor cannot decode.
	 */
	if (UFSR & UNDEFINSTR) {
		PRINT_ERROR("UsageFault: Can't decode instruction\n");
	}
	/*
	 * When this bit is set to 1, the PC value stacked for the exception return points to the instruction that
	 * attempted the illegal use of the EPSR.
	 * This bit is not set to 1 if an undefined instruction uses the EPSR.
	 */
	if (UFSR & INVSTATE) {
		PRINT_ERROR("UsageFault: illegal use of EPSR\n");
	}
	/* 
	 * the processor has attempted an illegal load of EXC_RETURN to the PC, as a result of an invalid
	 * context, or an invalid EXC_RETURN value.
	 * When this bit is set to 1, the PC value stacked for the exception return points to the instruction that tried
	 * to perform the illegal load of the PC.
	 */
	if (UFSR & INVPC) {
		PRINT_ERROR("UsageFault: Illegal PC on return form exception\n");
	}
	if (UFSR & NOCP) {
		PRINT_ERROR("UsageFault: The processor does not support coprocessor instruction\n");
	}
	if (UFSR & UNALIGNED) {
		PRINT_ERROR("UsageFault: Unaligned access\n");
	}
	if (UFSR & DIVBYZERO) {
		PRINT_ERROR("UsageFault: Divide by zero :P\n");
	}

	if (stackValid) {
		goto outPrintStack;
	} else {
		goto out;
	}
outPrintStack:
	PRINT_ERROR("R0: 0x%lx R1: 0x%lx R2: 0x%lx R3: 0x%lx R12: 0x%lx LR: 0x%lx PC: 0x%lx PSR: 0x%lx\n", stacked_r0, stacked_r1, stacked_r2, stacked_r3, stacked_r12, stacked_lr, stacked_pc, stacked_psr);
out:
#ifdef CONFIG_INCLUDE_pcTaskGetTaskName
	PRINT_ERROR("Taskname: %s\n", pcTaskGetTaskName(NULL));
#endif
	PRINT_ERROR("Kernel Panic\n");
	PRINT_ERROR("Halt System\n");
	asm("dbg #0");
	for (;;);
}
