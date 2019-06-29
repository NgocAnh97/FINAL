#include "address_map_arm.h"

void set_A9_IRQ_stack(void);
void config_GIC(void);
void config_HPS_timer(void);
void config_HPS_GPIO1(void);
void config_interval_timer(void);
void config_MPcore_private_timer(void);
void config_KEYs(void);
void config_IrDA(void);
void enable_A9_interrupts(void);
/* key_dir and pattern are written by interrupt service routines; we have to
 * declare these as volatile to avoid the compiler caching their values in
 * registers */
volatile int tick = 0; // set to 1 every time the HPS timer expires
volatile int key_dir = 0;
volatile int pattern = 0x0F0F0F0F; // pattern for LED lights
/* these globals are written by interrupt service routines; we declare them as
 * volatile to avoid the compiler caching their values, even in registers */
volatile char byte1, byte2, byte3; /* modified by interrupt service routine */
volatile int timeout;              // used to synchronize with the timer
/* function prototypes */
void HEX_IrDA(char, char, char);
/* ********************************************************************************
 * This program demonstrates use of interrupts with C code. It first starts
 * two timers: an HPS timer, and the FPGA interval timer. The program responds
 * to interrupts from these timers in addition to the pushbutton KEYs in the
 * FPGA.
 *
 * The interrupt service routine for the HPS timer causes the main program to
 * flash the green light connected to the HPS GPIO1 port.
 *
 * The interrupt service routine for the interval timer displays a pattern on
 * the LED lights, and shifts this pattern either left or right. The shifting
 * direction is reversed when KEY[1] is pressed
********************************************************************************/
int main(void)
{
    volatile int *HPS_GPIO1_ptr = (int *)HPS_GPIO1_BASE; // GPIO1 base address
    volatile int HPS_timer_LEDG = 0x01000000;            // value to turn on the HPS green light LEDG
    volatile int *LED_ptr = (int *)LED_BASE;

    set_A9_IRQ_stack();            // initialize the stack pointer for IRQ mode
    config_GIC();                  // configure the general interrupt controller
    config_HPS_timer();            // configure the HPS timer
    config_HPS_GPIO1();            // configure the HPS GPIO1 interface
    config_interval_timer();       // configure Altera interval timer to generate
                                   // interrupts
    // config_MPcore_private_timer(); // configure ARM A9 private timer
    config_KEYs();                 // configure pushbutton KEYs to generate interrupts
    config_IrDA();                 //// configure IrDA to generate interrupts
    enable_A9_interrupts();        // enable interrupts

    // while(1);
    // while (1) {
    //     while (!timeout)
    //         ; // wait to synchronize with timer

    //     /* display PS/2 data (from interrupt service routine) on HEX displays */
    //     HEX_IrDA(byte1, byte2, byte3);
    //     timeout = 0;
    // }
    while (1)
    {
        if (tick)
        {
            tick = 0;
            *HPS_GPIO1_ptr = HPS_timer_LEDG; // turn on/off the green light LEDG
            HPS_timer_LEDG ^= 0x01000000;    // toggle the bit that controls LEDG
        }
    }
}

/* setup HPS timer */
void config_HPS_timer()
{
    volatile int *HPS_timer_ptr = (int *)HPS_TIMER0_BASE; // timer base address

    *(HPS_timer_ptr + 0x2) = 0; // write to control register to stop timer
    /* set the timer period */
    int counter = 100000000;    // period = 1/(100 MHz) x (100 x 10^6) = 1 sec
    *(HPS_timer_ptr) = counter; // write to timer load register

    /* write to control register to start timer, with interrupts */
    *(HPS_timer_ptr + 2) = 0b011; // int mask (I) = 0 opposite in ARM private timer, mode (A) = 1, enable (E) = 1
}

/* setup HPS GPIO1. The GPIO1 port has one green light (LEDG) and one pushbutton
 * KEY connected for the DE1-SoC Computer. The KEY is connected to GPIO1[25],
 * and is not used here. The green LED is connected to GPIO1[24]. */
void config_HPS_GPIO1()
{
    volatile int *HPS_GPIO1_ptr = (int *)HPS_GPIO1_BASE; // GPIO1 base address

    *(HPS_GPIO1_ptr + 0x1) = 0x01000000; // write to the data direction register to set
                                         // bit 24 (LEDG) to be an output
    // Other possible actions include setting up GPIO1 to use the KEY, including
    // setting the debounce option and causing the KEY to generate an interrupt.
    // We do not use the KEY in this example.
}

/* setup the interval timer interrupts in the FPGA */
void config_interval_timer()
{
    volatile int *interval_timer_ptr = (int *)TIMER_BASE; // interal timer base address

    /* set the interval timer period for scrolling the HEX displays */
    int counter = 5000000; // 1/(100 MHz) x 5x10^6 = 50 msec; default setting provided gives timer period of 125ms
    *(interval_timer_ptr + 0x2) = (counter & 0xFFFF);
    *(interval_timer_ptr + 0x3) = (counter >> 16) & 0xFFFF;

    /* start interval timer, enable its interrupts */
    *(interval_timer_ptr + 1) = 0x7; // STOP = 0, START = 1, CONT = 1, ITO = 1
}

/* setup private timer in the ARM A9 */
void config_MPcore_private_timer()
{
    volatile int *MPcore_private_timer_ptr = (int *)MPCORE_PRIV_TIMER; // timer base address

    /* set the timer period */
    int counter = 20000000;                // period = 1/(200 MHz) x 40x10^6 = 0.2 sec
    *(MPcore_private_timer_ptr) = counter; // write to timer load register

    /* write to control register to start timer, with interrupts */
    *(MPcore_private_timer_ptr + 2) = 0x7; // int mask = 1, mode = 1, enable = 1
}

/* setup the KEY interrupts in the FPGA */
void config_KEYs()
{
    volatile int *KEY_ptr = (int *)KEY_BASE; // pushbutton KEY address

    *(KEY_ptr + 2) = 0xF; // enable interrupts for all KEYs
}

////* setup the IrDA interrupts in the FPGA */
void config_IrDA()
{
    volatile int *IrDA_ptr = (int *)IrDA_BASE; // pushbutton KEY base address

    *(IrDA_ptr) = 0xFF;    //reset
    *(IrDA_ptr + 1) = 0xF; // enable interrupts for IrDA
}
/****************************************************************************************
 * Subroutine to show a string of HEX data on the HEX displays
****************************************************************************************/
void HEX_IrDA(char b1, char b2, char b3)
{
    volatile int *HEX3_HEX0_ptr = (int *)HEX3_HEX0_BASE;
    volatile int *HEX5_HEX4_ptr = (int *)HEX5_HEX4_BASE;

    /* SEVEN_SEGMENT_DECODE_TABLE gives the on/off settings for all segments in
     * a single 7-seg display in the DE2 Media Computer, for the hex digits 0 -
     * F */
    unsigned char seven_seg_decode_table[] = {
        0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,
        0x7F, 0x67, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};
    unsigned char hex_segs[] = {0, 0, 0, 0, 0, 0, 0, 0};
    unsigned int shift_buffer, nibble;
    unsigned char code;
    int i;

    shift_buffer = (b1 << 16) | (b2 << 8) | b3;
    for (i = 0; i < 6; ++i)
    {
        nibble = shift_buffer & 0x0000000F; // character is in rightmost nibble
        code = seven_seg_decode_table[nibble];
        hex_segs[i] = code;
        shift_buffer = shift_buffer >> 4;
    }
    /* drive the hex displays */
    *(HEX3_HEX0_ptr) = *(int *)(hex_segs);
    *(HEX5_HEX4_ptr) = *(int *)(hex_segs + 4);
}
