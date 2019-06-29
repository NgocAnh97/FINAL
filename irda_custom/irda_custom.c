#include "address_custom.h"
#include <stdio.h>
#include <unistd.h>
// #include "hwlib.h"

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
/*******************************************************************************
 * This program demonstrates use of the IrDA port
 *
 * It performs the following:
 *	1. sends a text string to the IrDA
 * 	2. reads character data from the IrDA
 * 	3. echos the character data back to the IrDA
 ******************************************************************************/
int main(void)
{
  /* Declare volatile pointers to I/O registers (volatile means that IO load
       and store instructions will be used to access these pointer locations,
       instead of regular memory loads and stores) */
  volatile int *IrDA_ptr = (int *)RS232_0_BASE; // IrDA address
  // volatile int *RS232_ptr = (int *)RS232_0_BASE;
  volatile int *led_ptr = (int *)LEDR_BASE; 

  // char  text_string[] = "\nIrDA example code\n> \0";
  char *str;
  char a = 1;
  char b = 5;
  char c = 15;

  while (1)
  {
    put_irda(IrDA_ptr, a); //transmit
    *(led_ptr) = a;
    put_irda(IrDA_ptr, b);
    *(led_ptr) = b;
    put_irda(IrDA_ptr, c);
    *(led_ptr) = c; //display receive data to led
  }

  /* print a text string */
  // for (str = text_string; *str != 0; ++str)
  //     put_irda(IrDA_ptr, *str);

  // /* read and echo characters */
  // while (1) {
  //     c = get_irda(IrDA_ptr);
  //     if (c != '\0')
  //         put_irda(IrDA_ptr, c);
  // }
}
