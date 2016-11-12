
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "rpi-armtimer.h"
#include "rpi-base.h"
#include "rpi-gpio.h"
#include "rpi-interrupts.h"

#include "rpi-aux.h"

/** @brief The BCM2835/6 Interupt controller peripheral at it's base address */
static rpi_irq_controller_t* rpiIRQController =
        (rpi_irq_controller_t*)RPI_INTERRUPT_CONTROLLER_BASE;

static INTERRUPT_VECTOR InterruptVectorTable[64+8]; // hardcoded 21 basic pending registers

/**
    @brief Return the IRQ Controller register set
*/
rpi_irq_controller_t* RPI_GetIrqController( void )
{
    return rpiIRQController;
}

void IRQRegister(const uint32_t irq, FN_INTERRUPT_HANDLER fHandler, void *args)
{
    IRQBlock();
    InterruptVectorTable[irq].fHandler = fHandler;
    InterruptVectorTable[irq].args = args;
    IRQUnBlock();
}

void IRQBlock(void)
{

}

void IRQUnBlock(void)
{

}

void RPI_EnableIrq(const uint32_t irq)
{
    uint32_t mask = 1 << (irq % 32);

    IRQBlock();
    if (irq <= 31)
    {
        rpiIRQController->Enable_IRQs_1 = mask;
    }
    else if (irq <= 63)
    {
        rpiIRQController->Enable_IRQs_2 = mask;
    }
    else
    {
        rpiIRQController->Enable_Basic_IRQs = mask;
    }
    IRQUnBlock();
}

void RPI_DisableIrq(const uint32_t irq)
{
    uint32_t mask = 1 << (irq % 32);

    IRQBlock();
    if (irq <= 31)
    {
        rpiIRQController->Disable_IRQs_1 = mask;
    }
    else if (irq <= 63)
    {
        rpiIRQController->Disable_IRQs_1 = mask;
    }
    else
    {
        rpiIRQController->Disable_Basic_IRQs = mask;
    }
    IRQUnBlock();
}

// initialize the interrupt controller
void RPI_IrqControllerInit(void)
{
    /* Enable the timer interrupt IRQ */
    RPI_EnableIrq(RPI_IRQ_ARM_TIMER);
}


/**
    @brief The Reset vector interrupt handler

    This can never be called, since an ARM core reset would also reset the
    GPU and therefore cause the GPU to start running code again until
    the ARM is handed control at the end of boot loading
*/
void __attribute__((interrupt("ABORT"))) reset_vector(void)
{
    while( 1 )
    {
        LED_ON();
    }
}

/**
    @brief The undefined instruction interrupt handler

    If an undefined intstruction is encountered, the CPU will start
    executing this function. Just trap here as a debug solution.
*/
void __attribute__((interrupt("UNDEF"))) undefined_instruction_vector(void)
{
    while( 1 )
    {
        /* Do Nothing! */
        LED_ON();
    }
}


/**
    @brief The supervisor call interrupt handler

    The CPU will start executing this function. Just trap here as a debug
    solution.
*/
void __attribute__((interrupt("SWI"))) software_interrupt_vector(void)
{
    while( 1 )
    {
        /* Do Nothing! */
        LED_ON();
    }
}


/**
    @brief The prefetch abort interrupt handler

    The CPU will start executing this function. Just trap here as a debug
    solution.
*/
void __attribute__((interrupt("ABORT"))) prefetch_abort_vector(void)
{
    while( 1 )
    {
        LED_ON();
    }
}


/**
    @brief The Data Abort interrupt handler

    The CPU will start executing this function. Just trap here as a debug
    solution.
*/
void __attribute__((interrupt("ABORT"))) data_abort_vector(void)
{
    while( 1 )
    {
        LED_ON();
    }
}

extern void handleInterruptRange(uint32_t pending, const uint32_t base)
{
    while (pending)
    {
        // get index of first set bit
        uint32_t bit = 31 - __builtin_clz(pending);

        // map to irq
        uint32_t irq = base + bit;

        // call interrupt handler
        if (InterruptVectorTable[irq].fHandler)
        {
            InterruptVectorTable[irq].fHandler(irq, InterruptVectorTable[irq].args);
        }

        // clear bit field
        pending &= ~(1 << bit);
    }
}

/**
    @brief The IRQ Interrupt handler

    This handler is run every time an interrupt source is triggered. It's
    up to the handler to determine the source of the interrupt and most
    importantly clear the interrupt flag so that the interrupt won't
    immediately put us back into the start of the handler again.
*/
void __attribute__((interrupt("IRQ"))) interrupt_vector(void)
{
    register uint32_t basic_pending;

    // read pending registers
    basic_pending = rpiIRQController->IRQ_basic_pending;

    // pending 1 (0-31)
    if (basic_pending & (1 << 8))
    {
        handleInterruptRange(rpiIRQController->IRQ_pending_1, 0);
    }

    // pending 2 (32-61)
    if (basic_pending & (1 << 9))
    {
        handleInterruptRange(rpiIRQController->IRQ_pending_2, 32);
    }

    // basic pending
    if (basic_pending & 0xff)
    {
        handleInterruptRange(basic_pending & 0xff, 64);
    }
}


/**
    @brief The FIQ Interrupt Handler

    The FIQ handler can only be allocated to one interrupt source. The FIQ has
    a full CPU shadow register set. Upon entry to this function the CPU
    switches to the shadow register set so that there is no need to save
    registers before using them in the interrupt.

    In C you can't see the difference between the IRQ and the FIQ interrupt
    handlers except for the FIQ knowing it's source of interrupt as there can
    only be one source, but the prologue and epilogue code is quite different.
    It's much faster on the FIQ interrupt handler.

    The prologue is the code that the compiler inserts at the start of the
    function, if you like, think of the opening curly brace of the function as
    being the prologue code. For the FIQ interrupt handler this is nearly
    empty because the CPU has switched to a fresh set of registers, there's
    nothing we need to save.

    The epilogue is the code that the compiler inserts at the end of the
    function, if you like, think of the closing curly brace of the function as
    being the epilogue code. For the FIQ interrupt handler this is nearly
    empty because the CPU has switched to a fresh set of registers and so has
    not altered the main set of registers.
*/
void __attribute__((interrupt("FIQ"))) fast_interrupt_vector(void)
{

}
