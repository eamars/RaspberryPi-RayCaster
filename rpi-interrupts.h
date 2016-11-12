/*

    Part of the Raspberry-Pi Bare Metal Tutorials
    Copyright (c) 2013, Brian Sidebotham
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

#ifndef RPI_INTERRUPTS_H
#define RPI_INTERRUPTS_H

#include <stdint.h>

#include "rpi-base.h"

/** @brief See Section 7.5 of the BCM2836 ARM Peripherals documentation, the base
    address of the controller is actually xxxxB000, but there is a 0x200 offset
    to the first addressable register for the interrupt controller, so offset the
    base to the first register */
#define RPI_INTERRUPT_CONTROLLER_BASE   ( PERIPHERAL_BASE + 0xB200 )

/** @brief Bits in the Enable_Basic_IRQs register to enable various interrupts.
    See the BCM2835 ARM Peripherals manual, section 7.5 */
#define RPI_BASIC_ARM_TIMER_IRQ         (1 << 0)
#define RPI_BASIC_ARM_MAILBOX_IRQ       (1 << 1)
#define RPI_BASIC_ARM_DOORBELL_0_IRQ    (1 << 2)
#define RPI_BASIC_ARM_DOORBELL_1_IRQ    (1 << 3)
#define RPI_BASIC_GPU_0_HALTED_IRQ      (1 << 4)
#define RPI_BASIC_GPU_1_HALTED_IRQ      (1 << 5)
#define RPI_BASIC_ACCESS_ERROR_1_IRQ    (1 << 6)
#define RPI_BASIC_ACCESS_ERROR_0_IRQ    (1 << 7)

typedef enum 
{
    RPI_IRQ_0   =   0   ,
    RPI_IRQ_1   =   1   ,
    RPI_IRQ_2   =   2   ,
    RPI_IRQ_3   =   3   ,
    RPI_IRQ_4   =   4   ,
    RPI_IRQ_5   =   5   ,
    RPI_IRQ_6   =   6   ,
    RPI_IRQ_7   =   7   ,
    RPI_IRQ_8   =   8   ,
    RPI_IRQ_9   =   9   ,
    RPI_IRQ_10  =   10  ,
    RPI_IRQ_11  =   11  ,
    RPI_IRQ_12  =   12  ,
    RPI_IRQ_13  =   13  ,
    RPI_IRQ_14  =   14  ,
    RPI_IRQ_15  =   15  ,
    RPI_IRQ_16  =   16  ,
    RPI_IRQ_17  =   17  ,
    RPI_IRQ_18  =   18  ,
    RPI_IRQ_19  =   19  ,
    RPI_IRQ_20  =   20  ,
    RPI_IRQ_21  =   21  ,
    RPI_IRQ_22  =   22  ,
    RPI_IRQ_23  =   23  ,
    RPI_IRQ_24  =   24  ,
    RPI_IRQ_25  =   25  ,
    RPI_IRQ_26  =   26  ,
    RPI_IRQ_27  =   27  ,
    RPI_IRQ_28  =   28  ,
    RPI_IRQ_AUX_INT  =   29  ,
    RPI_IRQ_30  =   30  ,
    RPI_IRQ_31  =   31  ,
    RPI_IRQ_32  =   32  ,
    RPI_IRQ_33  =   33  ,
    RPI_IRQ_34  =   34  ,
    RPI_IRQ_35  =   35  ,
    RPI_IRQ_36  =   36  ,
    RPI_IRQ_37  =   37  ,
    RPI_IRQ_38  =   38  ,
    RPI_IRQ_39  =   39  ,
    RPI_IRQ_40  =   40  ,
    RPI_IRQ_41  =   41  ,
    RPI_IRQ_42  =   42  ,
    RPI_IRQ_I2C_SPI_SLV_INT  =   43  ,
    RPI_IRQ_44  =   44  ,
    RPI_IRQ_PWA0  =   45  ,
    RPI_IRQ_PWA1  =   46  ,
    RPI_IRQ_47  =   47  ,
    RPI_IRQ_SMI  =   48  ,
    RPI_IRQ_GPIO_INT_0  =   49  ,
    RPI_IRQ_GPIO_INT_1  =   50  ,
    RPI_IRQ_GPIO_INT_2  =   51  ,
    RPI_IRQ_GPIO_INT_3  =   52  ,
    RPI_IRQ_I2C_INT  =   53  ,
    RPI_IRQ_SPI_INT  =   54  ,
    RPI_IRQ_PCM_INT  =   55  ,
    RPI_IRQ_56  =   56  ,
    RPI_IRQ_UART_INT  =   57  ,
    RPI_IRQ_58  =   58  ,
    RPI_IRQ_59  =   59  ,
    RPI_IRQ_60  =   60  ,
    RPI_IRQ_61  =   61  ,
    RPI_IRQ_62  =   62  ,
    RPI_IRQ_63  =   63  ,
    RPI_IRQ_ARM_TIMER  =   64  ,
    RPI_IRQ_ARM_MAILBOX  =   65  ,
    RPI_IRQ_DOORBELL_0  =   66  ,
    RPI_IRQ_DOORBELL_1  =   67  ,
    RPI_IRQ_GPU0_HALTED  =   68  ,
    RPI_IRQ_GPU1_HALTED  =   69  ,
    RPI_IRQ_ILLEGAL_ACCESS_TYPE_1  =   70  ,
    RPI_IRQ_ILLEGAL_ACCESS_TYPE_2  =   71  
} RPIIrq_t;


/** @brief The interrupt controller memory mapped register set */
typedef struct {
    volatile uint32_t IRQ_basic_pending;
    volatile uint32_t IRQ_pending_1;
    volatile uint32_t IRQ_pending_2;
    volatile uint32_t FIQ_control;
    volatile uint32_t Enable_IRQs_1;
    volatile uint32_t Enable_IRQs_2;
    volatile uint32_t Enable_Basic_IRQs;
    volatile uint32_t Disable_IRQs_1;
    volatile uint32_t Disable_IRQs_2;
    volatile uint32_t Disable_Basic_IRQs;
} rpi_irq_controller_t;

typedef void (*FN_INTERRUPT_HANDLER) (uint32_t irq, void *args);

typedef struct
{
    FN_INTERRUPT_HANDLER fHandler;
    void *args;
} INTERRUPT_VECTOR;

extern void IRQRegister(const uint32_t irq, FN_INTERRUPT_HANDLER fHandler, void *args);
extern void IRQBlock(void);
extern void IRQUnBlock(void);
extern void handleInterruptRange(uint32_t pending, const uint32_t base);


/* Found in the *start.S file, implemented in assembler */
extern void _enable_interrupts( void );


extern rpi_irq_controller_t* RPI_GetIrqController( void );
extern void RPI_IrqControllerInit(void);
extern void RPI_EnableIrq(const uint32_t irq);
extern void RPI_DisableIrq(const uint32_t irq);

#endif
