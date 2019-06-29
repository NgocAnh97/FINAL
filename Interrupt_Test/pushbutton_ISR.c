#include "address_map_arm.h"

extern volatile int key_dir;
extern volatile int pattern;
/***************************************************************************************
 * Pushbutton - Interrupt Service Routine
 *
 * This routine toggles the key_dir variable from 0 <-> 1
****************************************************************************************/
void pushbutton_ISR(void)
{
	printf("\nButton IRQ");
    volatile int * KEY_ptr = (int *)KEY_BASE;				//(int *)0xFF200050
	volatile int * LED_ptr = (int *) LED_BASE;
    volatile int *HEX3_HEX0_ptr = (int *) HEX3_HEX0_BASE;
	int press, LED_bits, HEX_bits;


    press          = *(KEY_ptr + 3); // read the pushbutton interrupt register
    *(KEY_ptr + 3) = press;          // Clear the interrupt

    key_dir ^= 1; // Toggle key_dir value
    // if (press & 0x1)							// KEY0
	// 	LED_bits = 0b1;							//HEX_bits = 0b00111111;
	// else if (press & 0x2)					// KEY1
	// 	LED_bits = 0b10;						//HEX_bits = 0b00000110;
	// else if (press & 0x4)
	// 	LED_bits = 0b100;						//HEX_bits = 0b01011011;
	// else  //if (press & 0x8)
	// 	LED_bits = 0b1000;						//HEX_bits = 0b01001111;
	// *LED_ptr = LED_bits;	

	if (press & 0x1)							// KEY0
		HEX_bits = 0b00111111;							//
	else if (press & 0x2)					// KEY1
		HEX_bits = 0b00000110;						//
	else if (press & 0x4)
		HEX_bits = 0b01011011;					//
	else  //if (press & 0x8)
		HEX_bits = 0b01001111;					//

	*HEX3_HEX0_ptr = HEX_bits;
	return;

}
