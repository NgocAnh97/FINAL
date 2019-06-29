#include "address_map_arm.h"

extern volatile int tick;
volatile int *HPS_GPIO1_ptr = (int *)HPS_GPIO1_BASE; // GPIO1 base address
volatile int HPS_timer_LEDG = 0x01000000;            // value to turn on the HPS green light LEDG

/******************************************************************************
 * HPS timer0 interrupt service routine
 *
 * This code increments the tick variable, and clears the interrupt
 *****************************************************************************/
void HPS_timer_ISR()
{
    printf("\nHPS Timer");
    volatile int * HPS_timer_ptr = (int *)HPS_TIMER0_BASE; // HPS timer address

    ++tick; // used by main program

    *(HPS_timer_ptr + 3); // Read timer end of interrupt register to
                          // clear the interrupt
    while (1)
    {
        if (tick)
        {
            tick = 0;
            *HPS_GPIO1_ptr = HPS_timer_LEDG; // turn on/off the green light LEDG
            HPS_timer_LEDG ^= 0x01000000;    // toggle the bit that controls LEDG
        }
    }
    return;
}

