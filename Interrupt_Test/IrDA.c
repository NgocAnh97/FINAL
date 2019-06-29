// /* function prototypes */
// // void put_irda(volatile int *, char);
// // char get_irda(volatile int *);

// /*******************************************************************************
//  * Subroutine to send a character to the IrDA
//  ******************************************************************************/
// void put_irda(volatile int * IrDA_ptr, char c) {
//     int control;
//     control = *(IrDA_ptr + 1); // read the IrDA control register
//     if (control & 0xFFFF0000)       // if WSPACE =1 space, echo character, else ignore
//         *(IrDA_ptr) = c;
// }

// /*******************************************************************************
//  * Subroutine to read a character from the IrDA
//  * Returns \0 if no character, otherwise returns the character
//  ******************************************************************************/
// char get_irda(volatile int * IrDA_ptr) {
//     int data;
//     data = *(IrDA_ptr); // read the IrDA data register
//     if (data & 0x00008000)   // check RVALID to see if there is new data
//         return ((char)data & 0xFF);
//     else
//         return ('\0');
// }


    
