#include <uart.h>
#define UART_PRV
#include <uart_prv.h>
#include <semihosting.h>

#define SYS_OPEN     0x01
#define SYS_CLOSE    0x02
#define SYS_WRITEC   0x03
#define SYS_WRITE0   0x04
#define SYS_WRITE    0x05
#define SYS_READ     0x06
#define SYS_READC    0x07
#define SYS_ISERROR  0x08
#define SYS_ISTTY    0x09
#define SYS_SEEK     0x0A
#define SYS_FLEN     0x0C
#define SYS_TMPNAM   0x0D
#define SYS_REMOVE   0x0E
#define SYS_RENAME   0x0E
#define SYS_CLOCK    0x10
#define SYS_TIME     0x11
#define SYS_ERRNO    0x13
#define SYS_GET_CMD  0x15
#define SYS_HEAPINFO 0x16
#define SYS_ELAPSED  0x30
#define SYS_TICKFREQ 0x31

struct uart {
	struct uart_generic gen;
};


UART_INIT(semihosting, port, bautrate) {
	struct uart *uart = (struct uart *) UART_GET_DEV(port);
	int32_t ret;
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
	/* ignored */
	(void) bautrate;
	return uart;
}
UART_DEINIT(semihosting, uart) {
	return 0;
}
UART_GETC(semihosting, uart, waittime) {
	return '\0';
}
UART_PUTC(semihosting, uart, c, waittime) {
	int32_t ret;
	uart_lock(uart, waittime, -1);
	ret = uart_putcISR(uart, c);
	uart_unlock(uart, -1);
	return ret;
}
UART_PUTS(semihosting, uart, data, waittime) {
	int32_t ret;
	uart_lock(uart, waittime, -1);
	ret = uart_putsISR(uart, data);
	uart_unlock(uart, -1);
	return ret;
}
UART_GETC_ISR(semihosting, uart) {
	return '\0';
}
UART_PUTC_ISR(semihosting, uart, c) {
	if (!semihosting_debugIsEnabled()) {
		/* igonre Debug is Enabled */
		return 0;
	}
	semihosting_systemCall(SYS_WRITEC, (uint32_t) &c);
	return 0;
}
UART_PUTS_ISR(semihosting, uart, data) {
	if (!semihosting_debugIsEnabled()) {
		/* igonre Debug is Enabled */
		return 0;
	}
	semihosting_systemCall(SYS_WRITE0, (uint32_t) data);
	return 0;
}
UART_OPS(semihosting);
struct uart semihosting_uart = {
	UART_INIT_DEV(semihosting)
	HAL_NAME("Semihost UART")
};
UART_ADDDEV(semihosting, semihosting_uart);
