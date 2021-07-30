#include <riscv.h>
#include <riscv/float.h>

#if __riscv_xlen == 64
	#define portWORD_SIZE 8
	#define store_x sd
	#define load_x ld
#elif __riscv_xlen == 32
	#define store_x sw
	#define load_x lw
	#define portWORD_SIZE 4
#else
	#error Assembler did not define __riscv_xlen
#endif

	.section .text
	.globl fpu_save
	.type fpu_save,@function
fpu_save:
	li t1, SR_FS
	addi sp, sp, -portFPU_CONTEX_SIZE
	csrs CSR_STATUS, t1
	frcsr t0
	FPU_STORE f0, 1 * portFPUWORD_SIZE(sp)
	FPU_STORE f1, 2 * portFPUWORD_SIZE(sp)
	FPU_STORE f2, 3 * portFPUWORD_SIZE(sp)
	FPU_STORE f3, 4 * portFPUWORD_SIZE(sp)
	FPU_STORE f4, 5 * portFPUWORD_SIZE(sp)
	FPU_STORE f5, 6 * portFPUWORD_SIZE(sp)
	FPU_STORE f6, 7 * portFPUWORD_SIZE(sp)
	FPU_STORE f7, 8 * portFPUWORD_SIZE(sp)
	FPU_STORE f8, 9 * portFPUWORD_SIZE(sp)
	FPU_STORE f9, 10 * portFPUWORD_SIZE(sp)
	FPU_STORE f10, 11 * portFPUWORD_SIZE(sp)
	FPU_STORE f11, 12 * portFPUWORD_SIZE(sp)
	FPU_STORE f12, 13 * portFPUWORD_SIZE(sp)
	FPU_STORE f13, 14 * portFPUWORD_SIZE(sp)
	FPU_STORE f14, 15 * portFPUWORD_SIZE(sp)
	FPU_STORE f15, 16 * portFPUWORD_SIZE(sp)
	FPU_STORE f16, 17 * portFPUWORD_SIZE(sp)
	FPU_STORE f17, 18 * portFPUWORD_SIZE(sp)
	FPU_STORE f18, 19 * portFPUWORD_SIZE(sp)
	FPU_STORE f19, 20 * portFPUWORD_SIZE(sp)
	FPU_STORE f20, 21 * portFPUWORD_SIZE(sp)
	FPU_STORE f21, 22 * portFPUWORD_SIZE(sp)
	FPU_STORE f22, 23 * portFPUWORD_SIZE(sp)
	FPU_STORE f23, 24 * portFPUWORD_SIZE(sp)
	FPU_STORE f24, 25 * portFPUWORD_SIZE(sp)
	FPU_STORE f25, 26 * portFPUWORD_SIZE(sp)
	FPU_STORE f26, 27 * portFPUWORD_SIZE(sp)
	FPU_STORE f27, 28 * portFPUWORD_SIZE(sp)
	FPU_STORE f28, 29 * portFPUWORD_SIZE(sp)
	FPU_STORE f29, 30 * portFPUWORD_SIZE(sp)
	FPU_STORE f30, 31 * portFPUWORD_SIZE(sp)
	FPU_STORE f31, 32 * portFPUWORD_SIZE(sp)
	sw t0, 33 * portFPUWORD_SIZE(sp)
	csrc CSR_STATUS, t1
	/* set FS to CLEAN */
	li t1, SR_FS_CLEAN
	csrs CSR_STATUS, t1
	ret

	.section .text
	.globl fpu_restore
	.type fpu_restore,@function
fpu_restore:
	li t1, SR_FS
	lw t0, 33 * portFPUWORD_SIZE(sp)
	csrs CSR_STATUS, t1
	FPU_LOAD f0, 1 * portFPUWORD_SIZE(sp)
	FPU_LOAD f1, 2 * portFPUWORD_SIZE(sp)
	FPU_LOAD f2, 3 * portFPUWORD_SIZE(sp)
	FPU_LOAD f3, 4 * portFPUWORD_SIZE(sp)
	FPU_LOAD f4, 5 * portFPUWORD_SIZE(sp)
	FPU_LOAD f5, 6 * portFPUWORD_SIZE(sp)
	FPU_LOAD f6, 7 * portFPUWORD_SIZE(sp)
	FPU_LOAD f7, 8 * portFPUWORD_SIZE(sp)
	FPU_LOAD f8, 9 * portFPUWORD_SIZE(sp)
	FPU_LOAD f9, 10 * portFPUWORD_SIZE(sp)
	FPU_LOAD f10, 11 * portFPUWORD_SIZE(sp)
	FPU_LOAD f11, 12 * portFPUWORD_SIZE(sp)
	FPU_LOAD f12, 13 * portFPUWORD_SIZE(sp)
	FPU_LOAD f13, 14 * portFPUWORD_SIZE(sp)
	FPU_LOAD f14, 15 * portFPUWORD_SIZE(sp)
	FPU_LOAD f15, 16 * portFPUWORD_SIZE(sp)
	FPU_LOAD f16, 17 * portFPUWORD_SIZE(sp)
	FPU_LOAD f17, 18 * portFPUWORD_SIZE(sp)
	FPU_LOAD f18, 19 * portFPUWORD_SIZE(sp)
	FPU_LOAD f19, 20 * portFPUWORD_SIZE(sp)
	FPU_LOAD f20, 21 * portFPUWORD_SIZE(sp)
	FPU_LOAD f21, 22 * portFPUWORD_SIZE(sp)
	FPU_LOAD f22, 23 * portFPUWORD_SIZE(sp)
	FPU_LOAD f23, 24 * portFPUWORD_SIZE(sp)
	FPU_LOAD f24, 25 * portFPUWORD_SIZE(sp)
	FPU_LOAD f25, 26 * portFPUWORD_SIZE(sp)
	FPU_LOAD f26, 27 * portFPUWORD_SIZE(sp)
	FPU_LOAD f27, 28 * portFPUWORD_SIZE(sp)
	FPU_LOAD f28, 29 * portFPUWORD_SIZE(sp)
	FPU_LOAD f29, 30 * portFPUWORD_SIZE(sp)
	FPU_LOAD f30, 31 * portFPUWORD_SIZE(sp)
	FPU_LOAD f31, 32 * portFPUWORD_SIZE(sp)
	addi sp, sp, portFPU_CONTEX_SIZE
	fscsr t0
	csrc CSR_STATUS, t1
	/* set FS to CLEAN */
	li t1, SR_FS_CLEAN
	csrs CSR_STATUS, t1
1:
	ret
