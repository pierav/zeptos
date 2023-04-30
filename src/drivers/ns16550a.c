#include "io.h"
#include <stdint.h>

#define UART_RBR_OFFSET 0  /* In:  Recieve Buffer Register */
#define UART_THR_OFFSET 0  /* Out: Transmitter Holding Register */
#define UART_DLL_OFFSET 0  /* Out: Divisor Latch Low */
#define UART_IER_OFFSET 1  /* I/O: Interrupt Enable Register */
#define UART_DLM_OFFSET 1  /* Out: Divisor Latch High */
#define UART_FCR_OFFSET 2  /* Out: FIFO Control Register */
#define UART_IIR_OFFSET 2  /* I/O: Interrupt Identification Register */
#define UART_LCR_OFFSET 3  /* Out: Line Control Register */
#define UART_MCR_OFFSET 4  /* Out: Modem Control Register */
#define UART_LSR_OFFSET 5  /* In:  Line Status Register */
#define UART_MSR_OFFSET 6  /* In:  Modem Status Register */
#define UART_SCR_OFFSET 7  /* I/O: Scratch Register */
#define UART_MDR1_OFFSET 8 /* I/O:  Mode Register */

#define UART_LSR_FIFOE 0x80          /* Fifo error */
#define UART_LSR_TEMT 0x40           /* Transmitter empty */
#define UART_LSR_THRE 0x20           /* Transmit-hold-register empty */
#define UART_LSR_BI 0x10             /* Break interrupt indicator */
#define UART_LSR_FE 0x08             /* Frame error indicator */
#define UART_LSR_PE 0x04             /* Parity error indicator */
#define UART_LSR_OE 0x02             /* Overrun error indicator */
#define UART_LSR_DR 0x01             /* Receiver data ready */
#define UART_LSR_BRK_ERROR_BITS 0x1E /* BI, FE, PE, OE bits */

static volatile void *uart8250_base;
static uint32_t uart8250_in_freq;
static uint32_t uart8250_baudrate;
static uint32_t uart8250_reg_width;
static uint32_t uart8250_reg_shift;

static uint32_t get_reg(uint32_t num) {
    uint32_t offset = num << uart8250_reg_shift;

    if (uart8250_reg_width == 1)
        return readb_relaxed(uart8250_base + offset);
    else if (uart8250_reg_width == 2)
        return readw_relaxed(uart8250_base + offset);
    else
        return readl_relaxed(uart8250_base + offset);
}

static void set_reg(uint32_t num, uint32_t val) {
    uint32_t offset = num << uart8250_reg_shift;

    if (uart8250_reg_width == 1)
        writeb_relaxed(val, uart8250_base + offset);
    else if (uart8250_reg_width == 2)
        writew_relaxed(val, uart8250_base + offset);
    else
        writel_relaxed(val, uart8250_base + offset);
}

int uart8250_putc(int ch) {
    while ((get_reg(UART_LSR_OFFSET) & UART_LSR_THRE) == 0)
        ;
    set_reg(UART_THR_OFFSET, (char)ch);
    return 0;
}

int uart8250_getc(void) {
    if (get_reg(UART_LSR_OFFSET) & UART_LSR_DR)
        return get_reg(UART_RBR_OFFSET);
    return -1;
}

int uart8250_init(void *base, uint32_t in_freq, uint32_t baudrate,
                  uint32_t reg_shift, uint32_t reg_width) {
    uint16_t bdiv;

    uart8250_base = (volatile void *)base;
    uart8250_reg_shift = reg_shift;
    uart8250_reg_width = reg_width;
    uart8250_in_freq = in_freq;
    uart8250_baudrate = baudrate;

    bdiv = uart8250_in_freq / (16 * uart8250_baudrate);

    /* Disable all interrupts */
    set_reg(UART_IER_OFFSET, 0x00);
    /* Enable DLAB */
    set_reg(UART_LCR_OFFSET, 0x80);

    if (bdiv) {
        /* Set divisor low byte */
        set_reg(UART_DLL_OFFSET, bdiv & 0xff);
        /* Set divisor high byte */
        set_reg(UART_DLM_OFFSET, (bdiv >> 8) & 0xff);
    }

    /* 8 bits, no parity, one stop bit */
    set_reg(UART_LCR_OFFSET, 0x03);
    /* Enable FIFO */
    set_reg(UART_FCR_OFFSET, 0x01);
    /* No modem control DTR RTS */
    set_reg(UART_MCR_OFFSET, 0x00);
    /* Clear line status */
    get_reg(UART_LSR_OFFSET);
    /* Read receive buffer */
    get_reg(UART_RBR_OFFSET);
    /* Set scratchpad */
    set_reg(UART_SCR_OFFSET, 0x00);

    return 0;
}
