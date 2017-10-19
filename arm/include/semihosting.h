#ifndef SEMIHOSTING_H_
#define SEMIHOSTING_H_
#include <stdint.h>
#include <stdbool.h>
bool semihosting_debugIsEnabled();
uint32_t semihosting_systemCall(uint32_t r1, uint32_t r2);
#define SEMIHOSTING_UART_ID HAL_GET_ID(uart, semihosting, semihosting_uart)
#endif
