#include <stdio.h>
#include <fcntl.h>   /* File Control Definitions           */
#include <termios.h> /* POSIX Terminal Control Definitions */
#include <unistd.h>  /* UNIX Standard Definitions 	   */
#include <errno.h>   /* ERROR Number Definitions           */

void main(void){
    int i=0;
    char chout[] = {9, 11, 12, 13, 14, 15, 16, 17, 00, 18};
    printf("So luong phan tu mang: %c  ",sizeof(char));
    printf("So luong data: %c  ",chout[0]);
	// nbytes = chout[0];
    for (i = 1; i <= chout[0]/*bytes_read*/ ; i++)							/*printing only the received characters*/
	{
		printf("%c  ",chout[i]);
		// printf("Got %d: %d\n", i, chout[i]);
		//printf("Got so ky tu %d \n", sizeof(chout));
	}
	printf("\n\n +----------------------------------+\n\n\n");
}
