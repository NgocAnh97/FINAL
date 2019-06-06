#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
void main(void)
{
	int fd;
	fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY /*| O_NDELAY*/);	/* ttyUSB0 is the FT232 based USB2SERIAL Converter   */
																	/* O_RDWR   - Read/Write access to serial port       */
																	/* O_NOCTTY - No terminal will control the process   */
																	/* Open in blocking mode,read will wait              */
	printf("fd:  %d \n", fd);
	// if (fd == -1) // if open is unsucessful
	// {
	// 	perror("open_port: Unable to open /dev/ttyUSB0");
	// }
	// else
	// {
	// 	fcntl(fd, F_SETFL, 0);
	// }
	if(fd == -1)						/* Error Checking */
        printf("\n  Error! in Opening ttyUSB0  ");
    else
        printf("\n  ttyUSB0 Opened Successfully ");
	//fcntl(fd, F_SETFL, FNDELAY);

	///Configure the port
	struct termios options;			/* Create the structure                          */
	tcgetattr(fd, &options);		/* Get the current attributes of the Serial port */
	cfsetispeed(&options, B9600); /* Set Read Speed as 9600                       */
	cfsetospeed(&options, B9600);	/* Set Write Speed as 9600                       */

	/* 8N1 Mode */
	options.c_cflag |= (CLOCAL | CREAD);	/* Enable receiver,Ignore Modem Control lines       */
	options.c_cflag &= ~PARENB; 		 /* Disables the Parity Enable bit(PARENB),So No Parity   */
	options.c_cflag &= ~CSTOPB;			/* CSTOPB = 2 Stop bits,here it is cleared so 1 Stop bit */
	options.c_cflag &= ~CSIZE;			/* Clears the mask for setting the data size             */
	options.c_cflag |= CS8; 			/* Set the data bits = 8                                 */
	options.c_cflag &= ~CRTSCTS;		/* No Hardware flow Control                         */

	//options.c_iflag &= ~(IXON | IXOFF | IXANY);          //* Disable XON/XOFF flow control both i/p and o/p */
	options.c_lflag &= ~(ICANON | ECHO | ISIG);	/* Non Cannonical mode                            */

	options.c_oflag &= ~OPOST;			//*No Output Processing*/

	/* Setting Time outs */
	options.c_cc[VMIN] = 100; 		/* Read at least 10 characters */
	options.c_cc[VTIME] = 0; 		/* Wait indefinetly   */

	//tcsetattr(fd, TCSANOW, &options);		// apply the settings (options) to the port	
	if((tcsetattr(fd,TCSANOW, &options)) != 0) /* Set the attributes to the termios structure*/
		    printf("\n  ERROR ! in Setting attributes");
	else
            printf("\n  BaudRate = 9600 \n  StopBits = 1 \n  Parity   = none");



	/*------------------------------- Write data to serial port -----------------------------*/
	//printf("de10 gui: \n");

	//char buff[]={0x11};
	//char buff[]={11,12,13,23,00};
	//write(fd, buff, sizeof(buff));				//write 1 buff to file fd


	/*------------------------------- Read data from serial port -----------------------------*/

	tcflush(fd, TCIFLUSH);  						 /* Discards old data in the rx buffer            */
	char chout[32];									/* Buffer to store the data received              */
	size_t nbytes;
	int bytes_read = 0;								/* Number of bytes read by the read() system call */
	int i = 0;
	nbytes = sizeof(chout);
	
	bytes_read = read(fd, &chout, 32);				/* Read the data                   */
	printf("\n\n  The number of bytes read - Bytes Rxed: -%d", bytes_read); 		/* Print the number of bytes read */
	printf("\n\n  ");
	for (i = 0; i < nbytes/*bytes_read*/ ; i++)							/*printing only the received characters*/
	{
		printf("%c",chout[i]);
		// printf("byte:  %d \n", bytes_read);
		// printf("Got %d: %d\n", i, chout[i]);
		//printf("Got so ky tu %d \n",sizeof(chout));
	}
	printf("\n\n +----------------------------------+\n\n\n");
	// printf("%s \n", buff);
	close(fd);										/* Close the serial port */
}
