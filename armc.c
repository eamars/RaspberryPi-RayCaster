/*

	Part of the Raspberry-Pi Bare Metal Tutorials
	Copyright (c) 2015, Brian Sidebotham
	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:

	1. Redistributions of source code must retain the above copyright notice,
		this list of conditions and the following disclaimer.

	2. Redistributions in binary form must reproduce the above copyright notice,
		this list of conditions and the following disclaimer in the
		documentation and/or other materials provided with the distribution.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
	AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
	IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
	ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
	LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
	CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
	SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
	CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
	ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
	POSSIBILITY OF SUCH DAMAGE.

*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "rpi-aux.h"
#include "rpi-armtimer.h"
#include "rpi-gpio.h"
#include "rpi-interrupts.h"
#include "rpi-mailbox-interface.h"
#include "rpi-systimer.h"
#include "rpi-uart.h"

#include "sip.h"

int dummy(uint8_t *payload, uint8_t payload_length)
{
	printf("DID SOMETHING\r\n");

	return 0;
}

void timerHandler(uint32_t irq, void *args)
{
	static int lit = 0;

    /* Clear the ARM Timer interrupt - it's the only interrupt we have
       enabled, so we want don't have to work out which interrupt source
       caused us to interrupt */
    RPI_GetArmTimer()->IRQClear = 1;

    /* Flip the LED */
    if( lit )
    {
        LED_ON();
        lit = 0;
    }
    else
    {
        LED_OFF();
        lit = 1;
    }
}

void uartRxHandler(uint32_t irq, void *args)
{
	char ch;
	RPI_AuxMiniUartNonBlockRead(&ch);

	printf("RX:%c\r\n", ch);
}

/** Main function - we'll never return from here */
void kernel_main( unsigned int r0, unsigned int r1, unsigned int atags )
{
	/* Write 1 to the LED init nibble in the Function Select GPIO
	   peripheral register to enable LED pin as an output */
	RPI_GetGpio()->LED_GPFSEL |= LED_GPFBIT;

	// setup the interript controller
	RPI_IrqControllerInit();

	/* Setup the system timer interrupt */
	RPI_ArmTimerInit();

	/* Initialise the UART */
	RPI_AuxMiniUartInit( 115200, 8, false );

	/* Print to the UART using the standard libc functions */
	printf( "Raspberry Pi Program Loader\r\n" );

	RPI_PropertyInit();
	RPI_PropertyAddTag( TAG_GET_BOARD_MODEL );
	RPI_PropertyAddTag( TAG_GET_BOARD_REVISION );
	RPI_PropertyAddTag( TAG_GET_FIRMWARE_VERSION );
	RPI_PropertyAddTag( TAG_GET_BOARD_MAC_ADDRESS );
	RPI_PropertyAddTag( TAG_GET_BOARD_SERIAL );
	RPI_PropertyAddTag( TAG_GET_MAX_CLOCK_RATE, TAG_CLOCK_ARM );
	RPI_PropertyProcess();

	rpi_mailbox_property_t* mp;
	mp = RPI_PropertyGet( TAG_GET_BOARD_MODEL );

	if( mp )
		printf( "Board Model: 0x%x\r\n", mp->data.value_32 );
	else
		printf( "Board Model: NULL\r\n" );

	mp = RPI_PropertyGet( TAG_GET_BOARD_REVISION );

	if( mp )
		printf( "Board Revision: 0x%x\r\n", mp->data.value_32 );
	else
		printf( "Board Revision: NULL\r\n" );

	mp = RPI_PropertyGet( TAG_GET_FIRMWARE_VERSION );

	if( mp )
		printf( "Firmware Version: 0x%x\r\n", mp->data.value_32 );
	else
		printf( "Firmware Version: NULL\r\n" );

	mp = RPI_PropertyGet( TAG_GET_BOARD_MAC_ADDRESS );

	if( mp )
		printf( "MAC Address: %2.2X:%2.2X:%2.2X:%2.2X:%2.2X:%2.2X\r\n",
			   mp->data.buffer_8[0], mp->data.buffer_8[1], mp->data.buffer_8[2],
			   mp->data.buffer_8[3], mp->data.buffer_8[4], mp->data.buffer_8[5] );
	else
		printf( "MAC Address: NULL\r\n" );

	mp = RPI_PropertyGet( TAG_GET_BOARD_SERIAL );

	if( mp )
		printf( "Serial Number: %8.8X%8.8X\r\n",
				mp->data.buffer_32[0], mp->data.buffer_32[1] );
	else
		printf( "Serial Number: NULL\r\n" );

	mp = RPI_PropertyGet( TAG_GET_MAX_CLOCK_RATE );

	if( mp )
		printf( "Maximum ARM Clock Rate: %d MHz\r\n", mp->data.buffer_32[1] / (1000000));
	else
		printf( "Maximum ARM Clock Rate: NULL\r\n" );

	/* Ensure the ARM is running at it's maximum rate */
	RPI_PropertyInit();
	RPI_PropertyAddTag( TAG_SET_CLOCK_RATE, TAG_CLOCK_ARM, mp->data.buffer_32[1] );
	RPI_PropertyProcess();

	RPI_PropertyInit();
	RPI_PropertyAddTag( TAG_GET_CLOCK_RATE, TAG_CLOCK_ARM );
	RPI_PropertyProcess();

	mp = RPI_PropertyGet( TAG_GET_CLOCK_RATE );

	if( mp )
		printf( "Set ARM Clock Rate: %d MHz\r\n", mp->data.buffer_32[1] / (1000000));
	else
		printf( "Set ARM Clock Rate: NULL\r\n" );

	SIP_t *sip = (SIP_t *)malloc(sizeof(SIP_t));
	memset(sip, 0x0, sizeof(SIP_t));

	SIPRegisterCommand(sip, 0x00, dummy);
	IRQRegister(RPI_IRQ_ARM_TIMER, timerHandler, 0);
	IRQRegister(RPI_IRQ_AUX_INT, uartRxHandler, 0);

	/* Enable interrupts! */
	_enable_interrupts();

	while( 1 )
	{
		// fetch data from mini uart
		char ch;
		RPI_AuxMiniUartBlockRead(&ch);

		printf("Poll:%c\r\n", ch);
		
		//SIPFeedInput(sip, ch);
	}
}
