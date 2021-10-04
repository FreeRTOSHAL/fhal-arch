#ifndef SBI_H_
#define SBI_H_
	struct sbiret {
		long error;
		long value;
	};
#define SBI_SUCCESS 0
#define SBI_ERR_FAILED -1
#define SBI_ERR_NOT_SUPPORTED -2
#define SBI_ERR_INVALID_PARAM -3
#define SBI_ERR_DENIED -4
#define SBI_ERR_INVALID_ADDRESS -5
#define SBI_ERR_ALREADY_AVAILABLE -6
/**
 * Returns the current SBI specification version.  This function must always succeed. 
 * The minor number of the SBI specification is encoded in the low 24
 * bits, with the major number encoded in the next 7 bits.  Bit 32 must be 0 and
 * is reserved for future expansion.
 * \return SBI specification version
 */
struct sbiret sbi_get_spec_version(void);

/**
 * Returns the current SBI implementation ID, which is different for every SBI implementation. 
 * It is intended that this implementation ID allows software to probe for SBI implementation quirks.
 * \return current SBI implementation ID
 */
struct sbiret sbi_get_impl_id(void);

/**
 * Returns the current SBI implementation version.  The encoding of this version
 * number is specific to the SBI implementation.
 * \return current SBI implementation version
 */
struct sbiret sbi_get_impl_version(void);

/**
 * Returns 0 if the given extension ID is not available, or an extension-specific 
 * non-zero value if it is available.
 * \return 0 if the given extension ID is not available
 */
struct sbiret sbi_probe_extension(long extension_id);

/**
 * Return a value that is legal for the corresponding CSR.  0 is always a legal
 * value for any of these CSRs.
 * \return mvendorid
 */
struct sbiret sbi_get_mvendorid(void);

/**
 * Return a value that is legal for the corresponding CSR.  0 is always a legal
 * value for any of these CSRs.
 * \return marchid
 */
struct sbiret sbi_get_marchid(void);

/**
 * Return a value that is legal for the corresponding CSR.  0 is always a legal
 * value for any of these CSRs.
 * \return mimpid
 */
struct sbiret sbi_get_mimpid(void);

#ifdef CONFIG_SBI_V01

/**
 * Programs the clock for next event after *stime_value* time. This function also
 * clears the pending timer interrupt bit.
 *
 * If the supervisor wishes to clear the timer interrupt without scheduling the next
 * timer event, it can either request a timer interrupt infinitely far into the
 * future (i.e., (uint64_t)-1), or it can instead mask the timer interrupt by 
 * clearing sie.STIE.
 */
void sbi_set_timer(uint64_t stime_value);

/**
 * Send an inter-processor interrupt to all the harts defined in hart_mask.
 * Interprocessor interrupts manifest at the receiving harts as Supervisor Software Interrupts.
 *
 * hart_mask is a virtual address that points to a bit-vector of harts. The bit
 * vector is represented as a sequence of unsigned longs whose length equals the
 * number of harts in the system divided by the number of bits in an unsigned long,
 * rounded up to the next integer.
 */
void sbi_send_ipi(const unsigned long *hart_mask);

#else

/**
 * Programs the clock for next event after *stime_value* time. *stime_value* is in absolute
 * time. This function must clear the pending timer interrupt bit as well.
 *
 * If the supervisor wishes to clear the timer interrupt without scheduling the next
 * timer event, it can either request a timer interrupt infinitely far into the
 * future (i.e., (uint64_t)-1), or it can instead mask the timer interrupt by
 * clearing sie.STIE.
 */
struct sbiret sbi_set_timer(uint64_t stime_value);

/**
 * Send an inter-processor interrupt to all the harts defined in hart_mask.
 * Interprocessor interrupts manifest at the receiving harts as the supervisor software
 * interrupts.
 *
 * *Returns* following possible values via sbiret.
 * [cols="<,>",options="header,compact"]
 * |===
 * | Error code                | Description
 * | SBI_SUCCESS               | IPI was sent to all the targeted harts successfully.
 * |===
 */
struct sbiret sbi_send_ipi(unsigned long hart_mask, unsigned long hart_mask_base);
#endif

/**
 * Instruct the remote harts to execute one or more HFENCE.GVMA instructions,
 * covering the range of guest physical addresses between start and size only
 * for the given VMID. This function call is only valid for harts implementing
 * hypervisor extension.
 *
 * *Returns* following possible values via sbiret.
 * [cols="<,>",options="header,compact"]
 * |===
 * | Error code                | Description
 * | SBI_SUCCESS               | IPI was sent to all the targeted harts successfully.
 * | SBI_ERR_NOT_SUPPORTED	    | This function is not supported as it is not implemented or one of the target hart doesn't support hypervisor extension.
 * | SBI_ERR_INVALID_ADDRESS   | `start_addr` or `size` is not valid.
 * |===
 */
struct sbiret sbi_remote_hfence_gvma_vmid(unsigned long hart_mask, unsigned long hart_mask_base, unsigned long start_addr, unsigned long size, unsigned long vmid);

/**
 * Instruct the remote harts to execute one or more HFENCE.GVMA instructions,
 * covering the range of guest physical addresses between start and size for all
 * the guests. This function call is only valid for harts implementing hypervisor
 * extension.
 *
 * *Returns* following possible values via sbiret.
 * [cols="<,>",options="header,compact"]
 * |===
 * | Error code                | Description
 * | SBI_SUCCESS               | IPI was sent to all the targeted harts successfully.
 * | SBI_ERR_NOT_SUPPORTED	    | This function is not supported as it is not implemented or one of the target hart doesn't support hypervisor extension.
 * | SBI_ERR_INVALID_ADDRESS   | `start_addr` or `size` is not valid.
 * |===
 */
struct sbiret sbi_remote_hfence_gvma(unsigned long hart_mask, unsigned long hart_mask_base, unsigned long start_addr, unsigned long size);

/**
 * Instruct the remote harts to execute one or more HFENCE.VVMA instructions,
 * covering the range of guest virtual addresses between start and size for the
 * given guest ASID. This function call is only valid for harts implementing
 * hypervisor extension.
 *
 * *Returns* following possible values via sbiret.
 * [cols="<,>",options="header,compact"]
 * |===
 * | Error code                | Description
 * | SBI_SUCCESS               | IPI was sent to all the targeted harts successfully.
 * | SBI_ERR_NOT_SUPPORTED	    | This function is not supported as it is not implemented or one of the target hart doesn't support hypervisor extension.
 * | SBI_ERR_INVALID_ADDRESS   | `start_addr` or `size` is not valid.
 * |===
 */
struct sbiret sbi_remote_hfence_vvma_asid(unsigned long hart_mask, unsigned long hart_mask_base, unsigned long start_addr, unsigned long size, unsigned long asid);

/**
 * Instruct the remote harts to execute one or more HFENCE.VVMA instructions,
 * covering the range of guest virtual addresses between start and size for any
 * process belonging to the current guest. This function call is only valid for
 * harts implementing hypervisor extension.
 *
 * *Returns* following possible values via sbiret.
 * [cols="<,>",options="header,compact"]
 * |===
 * | Error code                | Description
 * | SBI_SUCCESS               | IPI was sent to all the targeted harts successfully.
 * | SBI_ERR_NOT_SUPPORTED	    | This function is not supported as it is not implemented or one of the target hart doesn't support hypervisor extension.
 * | SBI_ERR_INVALID_ADDRESS   | `start_addr` or `size` is not valid.
 * |===
 */
struct sbiret sbi_remote_hfence_vvma(unsigned long hart_mask, unsigned long hart_mask_base, unsigned long start_addr, unsigned long size);


/**
 * Clears the pending IPIs if any. The IPI is cleared only in the hart for which
 * this SBI call is invoked.
 */
void sbi_clear_ipi(void);

#ifndef CONFIG_SBI_RFENCE_EXTENSION
/**
 * Instructs remote harts to execute FENCE.I instruction.
 * N.B. hart_mask is as described in sbi_send_ipi.
 */
void sbi_remote_fence_i(const unsigned long *hart_mask);

/**
 * Instructs the remote harts to execute one or more SFENCE.VMA instructions,
 * covering the range of virtual addresses between start and size.
 */
void sbi_remote_sfence_vma(const unsigned long *hart_mask, unsigned long start, unsigned long size);

/**
 * Instruct the remote harts to execute one or more SFENCE.VMA instructions,
 * covering the range of virtual addresses between start and size.  This covers
 * only the given ASID.
 */
void sbi_remote_sfence_vma_asid(const unsigned long *hart_mask, unsigned long start, unsigned long size, unsigned long asid);
#else
/**
 * Instructs remote harts to execute FENCE.I instruction.
 *
 * *Returns* following possible values via sbiret.
 * [cols="<,>",options="header,compact"]
 * |===
 * | Error code                | Description
 * | SBI_SUCCESS               | IPI was sent to all the targeted harts successfully.
 * |===
 */
struct sbiret sbi_remote_fence_i(unsigned long hart_mask, unsigned long hart_mask_base);

/**
 * Instructs the remote harts to execute one or more SFENCE.VMA instructions,
 * covering the range of virtual addresses between start and size.
 *
 * *Returns* following possible values via sbiret.
 * [cols="<,>",options="header,compact"]
 * |===
 * | Error code                | Description
 * | SBI_SUCCESS               | IPI was sent to all the targeted harts successfully.
 * | SBI_ERR_INVALID_ADDRESS   | `start_addr` or `size` is not valid.
 * |===
 */
struct sbiret sbi_remote_sfence_vma(unsigned long hart_mask, unsigned long hart_mask_base, unsigned long start_addr, unsigned long size);

/**
 * Instruct the remote harts to execute one or more SFENCE.VMA instructions,
 * covering the range of virtual addresses between start and size. This covers
 * only the given ASID.
 *
 * *Returns* following possible values via sbiret.
 * [cols="<,>",options="header,compact"]
 * |===
 * | Error code                | Description
 * | SBI_SUCCESS               | IPI was sent to all the targeted harts successfully.
 * | SBI_ERR_INVALID_ADDRESS   | `start_addr` or `size` is not valid.
 * |===
 */
struct sbiret sbi_remote_sfence_vma_asid(unsigned long hart_mask, unsigned long hart_mask_base, unsigned long start_addr, unsigned long size, unsigned long asid);
#endif

/**
 * Read a byte from debug console; returns the byte on success, or -1 for failure.
 * Note. This is the only SBI call that has a non-void return type.
 */
int sbi_console_getchar(void);

/**
 * Write data present in *ch* to debug console.
 *
 * Unlike `sbi_console_getchar`, this SBI call **will block** if there
 * remain any pending characters to be transmitted or if the receiving terminal
 * is not yet ready to receive the byte. However, if the console doesn't exist
 * at all, then the character is thrown away.
 */
void sbi_console_putchar(int ch);

/**
 * Puts all the harts to shut down state from supervisor point of view. This SBI
 * call doesn't return.
 */
void sbi_shutdown(void);

/**
 * Informs the SBI implementation that the supervisor would like the given hart to
 * begin execution.  This call is asynchronous -- more specifically,
 * `sbi_hart_start()` may return before execution has actually begin as long as
 * the SBI implementation is capable of ensuring the return code is accurate.
 *
 * *start_addr* points to a runtime-specified physical address, where a hart can
 * resume execution after its initialization/resume sequence.  Before jumping to
 * *start_addr*, the hart MUST configure PMP if present and switch to Supervisor
 * mode.
 *
 * *priv* is an XLEN-bit value.  Upon execution from `start_addr`, `a1` will
 * contain this exact value.
 *
 * *Returns* one of the following possible SBI error codes through sbiret.error.
 *
 * [cols="<,>",options="header,compact"]
 * |===
 * | Error code                | Description
 * | SBI_SUCCESS               | Hart was previously in stopped state. It will start executing from `start_addr`.
 * | SBI_ERR_INVALID_ADDRESS   | `start_addr` is not valid possibly due to following reasons. + it is not a valid physical address. + The address is prohibited by PMP to run in supervisor mode +
 * | SBI_ERR_INVALID_PARAM     | `hartid` is not a valid hartid as corresponding hart cannot started in supervisor mode.
 * | SBI_ERR_ALREADY_AVAILABLE | The given hartid is already started.
 * | SBI_ERR_FAILED            | The start request failed for unknown reasons.
 * |===
 *
 * The target hart jumps to higher privilege mode(S or VS mode) by executing at
 * `start_addr` with following values in specific registers.
 *
 * [cols="<,>",options="header,compact"]
 * |===
 * |Register Name		|Value  |
 * |satp			|  0    |
 * |sstatus.sie		|  0    |
 * |a0			|hartid |
 * |a1			|priv   |
 * |===
 *
 * All other registers remain in an undefined state.
 */
struct sbiret sbi_hart_start(unsigned long hartid, unsigned long start_addr, unsigned long priv);

/**
 * Returns ownership of the calling hart back to the SBI implementation.
 * This call is not expected to return under normal conditions.  `sbi_hart_stop()` must
 * be called with supervisor and user interrupts disabled.
 *
 * *Returns* following SBI error code through sbiret.error only if it fails.
 *
 * SBI_ERR_FAILED
 */
struct sbiret sbi_hart_stop(void);

/**
 * *Returns* the current status of *hartid* in sbiret.value, or an error through
 * sbiret.error. The possible status values are shown on the table below.
 *
 * [cols="<,,>",options="header,compact"]
 * |===
 * | Name                 		| Value | Description
 * | STARTED                     	|   0   | Already started
 * | STOPPED         		|   1   | Stopped
 * | START_REQUEST_PENDING       	|   2   | A start request pending
 * | STOP_REQUEST_PENDING       	|   3   | A stop request pending
 * |===
 *
 * Possible error code:
 *
 * SBI_ERR_INVALID_PARAM
 *
 * Since harts may transition state at any time due to any concurrent `sbi_hart_start` or
 * `sbi_hart_stop` calls, the return value from this function may not represent the actual
 * state of the hart at the time of return value verification.
 */
struct sbiret sbi_hart_status(unsigned long hartid);
#endif
