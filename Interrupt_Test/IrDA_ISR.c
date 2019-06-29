////IrDA_ISR:
#include <stdio.h>
#include "address_map_arm.h"
// #include <fcntl.h>	// include the fcntl.h and sys/mman.h header files
// #include <sys/mman.h>	//which are needed to use the /dev/mem device file and the mmap and munmap kernel functions.
// #include "hwlib.h"
// #include "socal/socal.h" // alt_write_hword alt_read_hword
// #include "socal/hps.h"
// #include "socal/alt_gpio.h"
extern volatile char byte1, byte2, byte3;

/*******************************************************************************
 * This program demonstrates use of UART of the IrDA port
 *
 * It performs the following:
 *	1. sends a text string to the UART
 * 	2. reads character data from the UART
 * 	3. echos the character data back to the UART
 ******************************************************************************/
/*******************************************************************************
 * Subroutine to send a character to the IrDA
 ******************************************************************************/
void put_irda(volatile int *IrDA_ptr, char c)
{
    int control;
    control = *(IrDA_ptr + 1); // read the IrDA control register
    if (control & 0x00FF0000)  // if WSPACE =1 (bit 16-23) space, echo character, else ignore
        *(IrDA_ptr) = c;
}

/*******************************************************************************
 * Subroutine to read a character from the IrDA
 * Returns \0 if no character, otherwise returns the character
 ******************************************************************************/
char get_irda(volatile int *IrDA_ptr)
{
    int data;
    data = *(IrDA_ptr);    // read the IrDA data register
    if (data & 0x00008000) // check RVALID to see if there is new data
        return ((char)data & 0xFF);
    else
        return ('\0');
}

void IrDA_ISR(void)
{
    /* Declare volatile pointers to I/O registers (volatile means that IO load
       and store instructions will be used to access these pointer locations,
       instead of regular memory loads and stores) */
    printf("\nIrDA IRQ");
    volatile int *IrDA_ptr = (int *)IrDA_BASE; // IrDA address
    volatile int *LED_ptr = (int *)LED_BASE;
    int IrDA_data, RAVAIL, WSPACE, RVALID;
    char c;

    IrDA_data = *(IrDA_ptr);               // read the Data register
    RVALID   = IrDA_data & 0x8000;
    RAVAIL = (IrDA_data & 0x00FF0000) >> 16; // 7 extract the RAVAIL field
    WSPACE = (*(IrDA_ptr + 1) & 0x00FF0000) >> 16; ///7
    *LED_ptr ^= 0b101010101;
    printf("Set LED.\n");
    printf("Wspace: %d\n",WSPACE);
    printf("RAVAIL: %d\n",RAVAIL);
    printf("RVALID: %d\n",RVALID);
    // usleep(30000);           //undefined
    // if (RVALID > 0)
	// {
	// 	/* always save the last three bytes received */
	// 	byte1 = byte2;
	// 	byte2 = byte3;
	// 	byte3 = IrDA_data & 0xFF;
	// 	if ( (byte2 == (char) 0xAA) && (byte3 == (char) 0x00) )
	// 		// mouse inserted; initialize sending of data
	// 		*(IrDA_ptr) = 0xF4;
	// }
    /* 
    if (WSPACE > 0)
    {
        put_irda(IrDA_ptr, 5);
        printf("IrDA put characters.\n");
        *(IrDA_ptr) = 0xFF;
        // c = get_irda(IrDA_ptr);
        //printf("IrDA get characters.\n");
        // if (c != '\0')
        //     put_irda(IrDA_ptr, c);
        //     printf("IrDA read and echo characters.\n");
    }
    */

    return;
}
