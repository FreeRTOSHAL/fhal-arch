#ifndef RISCV_FLOAT_H_
#define RISCV_FLOAT_H_
#if __riscv_flen == 32
	#define portFPUWORD_SIZE 4
	#define FPU_STORE fsw
	#define FPU_LOAD flw
#elif __riscv_flen == 64
	#define portFPUWORD_SIZE 8
	#define FPU_STORE fsd
	#define FPU_LOAD fld
#elif __riscv_flen == 128
	#define portFPUWORD_SIZE 16
	#define FPU_STORE fsq
	#define FPU_LOAD flq
#else
	#error Assembler did not define __riscv_flen
#endif

#ifdef CONFIG_ARCH_RISCV_MMODE
# define CSR_STATUS mstatus
#endif
#ifdef CONFIG_ARCH_RISCV_SMODE
# define CSR_STATUS sstatus
#endif
#define portFPU_CONTEX_SIZE (33 * portFPUWORD_SIZE)
#ifdef CONFIG_ARCH_64BIT
# define portFPU_ADDITIONAL_CONTEX_SIZE (portFPU_CONTEX_SIZE / 8)
#else
# define portFPU_ADDITIONAL_CONTEX_SIZE (portFPU_CONTEX_SIZE / 4)
#endif

#endif
