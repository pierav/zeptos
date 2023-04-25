#pragma once

#include <stdint.h>

int uart8250_putc(int ch);
int uart8250_getc(void);
int uart8250_init(void *base, uint32_t in_freq, uint32_t baudrate,
                  uint32_t reg_shift, uint32_t reg_width);
