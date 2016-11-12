#include "rpi-base.h"
#include "rpi-gpio.h"
#include "rpi-uart.h"

static uart_t *uart0 = (uart_t *) UART0_BASE;
static uart_t *uart1 = (uart_t *) UART1_BASE;

uart_t * RPI_GetUART0(void)
{
	return uart0;
}

uart_t * RPI_GetUART1(void)
{
	return uart1;
}