#ifndef RPI_UART_H
#define RPI_UART_H

#include "rpi-base.h"

#define UART0_BASE (PERIPHERAL_BASE + 0x201000)
#define UART1_BASE (PERIPHERAL_BASE + 0x215000)

typedef struct 
{
	volatile uint32_t DR;									// 0x0 data register
	volatile uint32_t RSRECR;								// 0x4 
	volatile uint8_t reserved0[0x18 - 0x08];				// 0x08-0x14
	volatile uint32_t FR;									// 0x18 flag register
	volatile uint32_t reserved1;							// 0x1c 
	volatile uint32_t ILPR;									// 0x20 
	volatile uint32_t IBRD;									// 0x24 integer baud rate divisor
	volatile uint32_t FBRD;									// 0x28 fractional baud rate divisor
	volatile uint32_t LCRH;									// 0x2c line control register
	volatile uint32_t CR;									// 0x30 control register
	volatile uint32_t IFLS;									// 0x34 interrupt fifo level select register
	volatile uint32_t IMSC;									// 0x38 interrupt mask set clear register
	volatile uint32_t RIS;									// 0x3c raw interrrupt status register
	volatile uint32_t MIS;									// 0x40 masked interrupt status register
	volatile uint32_t ICR;									// 0x44 interrupt clear register
	volatile uint32_t DMACR;								// 0x48 DMA control register
	volatile uint8_t reserved2[0x80 - 0x4c];				// 0x4c-0x7c
	volatile uint32_t ITCR;									// 0x80 test control register
	volatile uint32_t ITIP;									// 0x84 integration test input register
	volatile uint32_t ITOP;									// 0x88 integration test output register
	volatile uint32_t TDR;									// 0x8c test data register
} uart_t;

extern uart_t * RPI_GetUART0(void);
extern uart_t * RPI_GetUART1(void);

#endif