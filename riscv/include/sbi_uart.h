/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2021
 */
#ifndef SBI_UART_H_
#define SBI_UART_H_
HAL_DEFINE_GLOBAL_ARRAY(uart);
#define SBI_UART_ID HAL_GET_ID(uart, sbi, sbi_data0)
#endif
