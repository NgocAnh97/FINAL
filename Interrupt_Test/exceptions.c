#include "address_map_arm.h"
#include "defines.h"
#include "interrupt_ID.h"
/* This file:
 * 1. defines exception vectors for the A9 processor
 * 2. provides code that sets the IRQ mode stack, and that dis/enables
 * interrupts
 * 3. provides code that initializes the generic interrupt controller
*/
void config_interrupt(int, int);
void hw_write_bits(volatile int *, volatile int, volatile int);
void MPcore_private_timer_ISR(void);
void HPS_timer_ISR(void);
void interval_timer_ISR(void);
void pushbutton_ISR(void);
void IrDA_ISR(void);

// Define the remaining exception handlers
void __attribute__((interrupt)) __cs3_reset(void)
{
    while (1)
        ;
}
//Undefined mode � is entered if the processor attempts to execute an unimplemented instruction.
void __attribute__((interrupt)) __cs3_isr_undef(void)
{
    while (1)
        ;
}

void __attribute__((interrupt)) __cs3_isr_swi(void)
{
    while (1)
        ;
}

void __attribute__((interrupt)) __cs3_isr_pabort(void)
{
    while (1)
        ;
}

void __attribute__((interrupt)) __cs3_isr_dabort(void)
{
    while (1)
        ;
}
// Define the IRQ exception handler
void __attribute__((interrupt)) __cs3_isr_irq(void)
{
    // Read the ICCIAR (Interrupt Acknowledge Register) from the processor interface
    int address = MPCORE_GIC_CPUIF + ICCIAR;
    int int_ID = *((int *)address); //int_ID = *((int *) 0xFFFEC10C);

    if (int_ID == HPS_TIMER0_IRQ) // check if interrupt is from the HPS timer
    HPS_timer_ISR();
    // if (int_ID == MPCORE_PRIV_TIMER_IRQ) // check if interrupt is from the private timer
        // MPcore_private_timer_ISR();
    else if (int_ID == INTERVAL_TIMER_IRQ) // check if interrupt is from the Altera timer
        interval_timer_ISR();
    // else if (int_ID == IrDA_IRQ)
    //     IrDA_ISR();
    else if (int_ID == KEYS_IRQ) // check if interrupt is from the KEYs
        pushbutton_ISR();
    else
        while (1)
            ; // if unexpected, then stay here

    /* Clear this interrupt from the CPU Interface: write to the End of Interrupt Register (ICCEOIR).
    The interrupt handler software can then return control to the previously-interrupted main program*/
    address = MPCORE_GIC_CPUIF + ICCEOIR;
    *((int *)address) = int_ID; //*((int *) 0xFFFEC110) = int_ID;

    return;
}
//FIQ mode is entered in response to a fast interrupt request.
void __attribute__((interrupt)) __cs3_isr_fiq(void)
{
    while (1)
        ;
}

/*
 * Initialize the banked stack pointer register for IRQ mode
*/
void set_A9_IRQ_stack(void)
{
    int stack, mode;
    stack = A9_ONCHIP_END - 7; // top of A9 onchip memory, aligned to 8 bytes (0xFFFFFFFF - 7)
    /* change processor to IRQ mode with interrupts disabled 
    This mode is used to handle peripherals that issues interrupts*/
    // IRQ interrupts are disabled in the A9, by setting the IRQ disable bit in the CPSR to 1.
    mode = INT_DISABLE | IRQ_MODE; //mode = 0b11010010
    asm("msr cpsr, %[ps]"
        :
        : [ps] "r"(mode));
    /* set banked stack pointer */
    asm("mov sp, %[ps]"
        :
        : [ps] "r"(stack));

    /* go back to SVC - supervisor mode before executing subroutine return! It is also entered
on reset or power-up.
    This mode is used mainly by SWIs and the OS */
    mode = INT_DISABLE | SVC_MODE; //mode = 0b11010011
    asm("msr cpsr, %[ps]"
        :
        : [ps] "r"(mode));
}

/*
 * Turn on interrupts in the ARM processor (by setting the IRQ disable bit in the CPSR to 0)
*/
void enable_A9_interrupts(void)
{
    int status = SVC_MODE | INT_ENABLE; //int status = 0b01010011;
    asm("msr cpsr, %[ps]"
        :
        : [ps] "r"(status));
}

/* 
Turn off interrupts in the ARM processor
*/
void disable_A9_interrupts(void)
{
    int status = 0b11010011;
    asm("msr cpsr, %[ps]"
        :
        : [ps] "r"(status));
}

/*
 * Configure the Generic Interrupt Controller (GIC)
*/
void config_GIC(void)
{
    int address; // used to calculate register addresses

    /* enable some examples of interrupts */
    // config_interrupt(MPCORE_PRIV_TIMER_IRQ, CPU0);
    // config_interrupt(IrDA_IRQ, CPU0);

    /* configure the HPS timer interrupt */
    *((int *)0xFFFED8C4) = 0x01000000;
    *((int *)0xFFFED118) = 0x00000080;

    /* configure the FPGA interval timer and KEYs interrupts */
    // config_interrupt (KEYS_IRQ, CPU0);
    // config_interrupt (INTERVAL_TIMER_IRQ, CPU0);
    *((int *)0xFFFED848) = 0x00000101;
    *((int *)0xFFFED108) = 0x00000300;

    // Set Interrupt Priority Mask Register (ICCPMR). Enable interrupts of all priorities
    address = MPCORE_GIC_CPUIF + ICCPMR; //*((int *) 0xFFFEC104) = 0xFFFF;
    *((int *)address) = 0xFFFF;

    // Set CPU Interface Control Register (ICCICR). Enable signaling of interrupts
    address = MPCORE_GIC_CPUIF + ICCICR; //*((int *) 0xFFFEC100) = 1;
    *((int *)address) = ENABLE;

    // Configure the Distributor Control Register (ICDDCR) to enable the Distributor, send pending
    // interrupts to CPUs
    address = MPCORE_GIC_DIST + ICDDCR;
    *((int *)address) = ENABLE; //*((int *) 0xFFFED000) = 1;
}
/* 
 * Configure registers in the GIC for individual interrupt IDs.
*/
void config_interrupt(int int_ID, int CPU_target)
{
    int n, addr_offset, value, address;
    /* Set Interrupt Processor Targets Register (ICDIPTRn) to cpu0. 
	 * n = integer_div(int_ID / 4) * 4
	 * addr_offet = #ICDIPTR + n
	 * value = CPU_target << ((int_ID & 0x3) * 8)
	 */
    n = (int_ID >> 2) << 2;
    addr_offset = ICDIPTR + n;
    value = CPU_target << ((int_ID & 0x3) << 3);

    /* Now that we know the register address and value, we need to set the correct bits in 
	 * the GIC register, without changing the other bits */
    address = MPCORE_GIC_DIST + addr_offset;
    hw_write_bits((int *)address, 0xff << ((int_ID & 0x3) << 3), value);

    /* Set Interrupt Set-Enable Registers (ICDISERn). 
	 * n = (integer_div(in_ID / 32) * 4
	 * addr_offset = 0x100 + n
	 * value = enable << (int_ID & 0x1F) */
    n = (int_ID >> 5) << 2;
    addr_offset = ICDISER + n;
    value = 0x1 << (int_ID & 0x1f);
    /* Now that we know the register address and value, we need to set the correct bits in 
	 * the GIC register, without changing the other bits */
    address = MPCORE_GIC_DIST + addr_offset;
    hw_write_bits((int *)address, 0x1 << (int_ID & 0x1f), value);
}

void hw_write_bits(volatile int *addr, volatile int unmask, volatile int value)
{
    *addr = ((~unmask) & *addr) | value;
}
