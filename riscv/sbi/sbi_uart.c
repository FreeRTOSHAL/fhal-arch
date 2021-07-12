/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2021
 */
#include <FreeRTOS.h>
#include <uart.h>
#define UART_PRV
#include <uart_prv.h>
#include <sbi.h>


struct uart {
	struct uart_generic gen;
};

UART_INIT(sbi, port, baudrate) {
	int32_t ret;
	struct uart *uart = (struct uart *) UART_GET_DEV(port);
	if (uart == NULL) {
		return NULL;
	}
	ret = uart_generic_init(uart);
	if (ret < 0) {
		return NULL;
	}
	/*
	 * Already Init
	 */
	if (ret > 0) {
		return uart;
	}
	if (baudrate == 0) {
		return NULL;
	}

	return uart;
sbi_uart_init_error0:
	return NULL;
}


UART_DEINIT(sbi, uart) {
	uart->gen.init = false;
	return 0;
}


UART_GETC(sbi, uart, waittime) {
	char c;
	int lret;
	uart_lock(uart, waittime, 255);
	if (ulInIsr) {
		c = sbi_console_getchar();
	} else {
		register uintptr_t a0 asm ("a0") = 0;
		__asm volatile( "li a7, 4; ecall" : "+r" (a0) : : "a7");
		c = a0;
	}
	uart_unlock(uart, 255);
	return c;
}

UART_PUTC(sbi, uart, c, waittime) {
	int lret;
	uart_lock(uart, waittime, -1);
	if (ulInIsr) {
		sbi_console_putchar(c);
	} else {
		register uintptr_t a0 asm ("a0") = (uintptr_t)(c);
		__asm volatile( "li a7, 3; ecall" : : "r" (a0) : "a7");
	}
	uart_unlock(uart, -1);
	return 0;
}
UART_GETC_ISR(sbi, uart) {
	char c;
	if (ulInIsr) {
		c = sbi_console_getchar();
	} else {
		register uintptr_t a0 asm ("a0") = (uintptr_t)(c);
		__asm volatile( "li a7, 4; ecall" : "+r" (a0) : : "a7");
		c = a0;
	}
	return c;
}
UART_PUTC_ISR(sbi, uart, c) {
	if (ulInIsr) {
		sbi_console_putchar(c);
	} else {
		register uintptr_t a0 asm ("a0") = (uintptr_t)(c);
		__asm volatile( "li a7, 3; ecall" : : "r" (a0) : "a7");
	}
	return 0;
}

#ifndef CONFIG_UART_GENERIC_STRING
UART_PUTS(sbi, uart, s, waittime) {
	int32_t ret;
	uart_lock(uart, waittime, -1);
	while(*s != '\0') {
		if (*s == '\n') {
			ret = uart_putc(uart, '\r', waittime);
			if (ret < 0) {
				goto nxp_uart_puts_error0;
			}
		}
		ret = uart_putc(uart, *s, waittime);
		if (ret < 0) {
			goto nxp_uart_puts_error0;
		}
		s++;
	}
	uart_unlock(uart, -1);
	return 0;
nxp_uart_puts_error0:
	uart_unlock(uart, -1);
	return ret;
}
UART_PUTS_ISR(sbi, uart, s) {
	int32_t ret;
	while(*s != '\0') {
		if (*s == '\n') {
			ret = uart_putcISR(uart, '\r');
			if (ret < 0) {
				goto nxp_uart_putsISR_error0;
			}
		}
		ret = uart_putcISR(uart, *s);
		if (ret < 0) {
			goto nxp_uart_putsISR_error0;
		}
		s++;
	}
	return 0;
nxp_uart_putsISR_error0:
	return -1;
}
#endif
#ifndef CONFIG_UART_GENERIC_BYTE
UART_READ(sbi, uart, data, length, waittime) {
	int i;
	uart_lock(uart, waittime, -1);
	for (i = 0; i < length; i++) {
		data[i] = uart_getc(uart, waittime);
		if (data[i] == 255) {
			goto nxp_uart_read_error0;
		}
	}
	uart_unlock(uart, -1);
	return 0;
nxp_uart_read_error0:
	uart_unlock(uart, -1);
	return -1;
}

UART_WRITE(sbi, uart, data, length, waittime) {
	int i;
	int32_t ret;
	uart_lock(uart, waittime, -1);
	for (i = 0; i < length; i++) {
		ret = uart_putc(uart, data[i], waittime);
		if (ret < 0) {
			goto nxp_uart_read_error0;
		}
	}
	uart_unlock(uart, -1);
	return 0;
nxp_uart_read_error0:
	uart_unlock(uart, -1);
	return -1;
}

UART_READ_ISR(sbi, uart, data, length) {
	int i;
	for (i = 0; i < length; i++) {
		data[i] = uart_getcISR(uart);
		if (data[i] == 255) {
			goto nxp_uart_read_error0;
		}
	}
	return 0;
nxp_uart_read_error0:
	return -1;
}

UART_WRITE_ISR(sbi, uart, data, length) {
	int i;
	int32_t ret;
	for (i = 0; i < length; i++) {
		ret = uart_putcISR(uart, data[i]);
		if (ret < 0) {
			goto nxp_uart_read_error0;
		}
	}
	return 0;
nxp_uart_read_error0:
	return -1;
}
#endif
UART_OPS(sbi);

struct uart sbi_data0 = {
	UART_INIT_DEV(sbi)
	HAL_NAME("SBI Debug UART")
};

UART_ADDDEV(sbi, sbi_data0);
