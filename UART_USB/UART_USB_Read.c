#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */

void main(void)
{
	int fd; /*File Descriptor*/
	printf("\n +----------------------------------+");
	printf("\n |        Serial Port Read          |");
	printf("\n +----------------------------------+");

	/*------------------------------- Opening the Serial Port -------------------------------*/

	/* Change /dev/ttyUSB0 to the one corresponding to your system */

	fd = open("/dev/ttyACM0", O_RDWR | O_NOCTTY | O_SYNC | O_NDELAY); /* ttyUSB0 is the based USB2SERIAL Converter   */
																	  /* O_RDWR   - Read/Write access to serial port       */
																	  /* O_NOCTTY - No terminal will control the process   */
																	  /* Open in blocking mode (not O_NDELAY),read will wait              */
	printf("\n fd:  %d \n", fd);									  ///

	if (fd == -1) /* Error Checking */
		printf("\n  Error! in Opening ttyUSB0: %s  ", strerror(errno));
	else
		printf("\n  ttyUSB0 Opened Successfully ");
	//fcntl(fd, F_SETFL, FNDELAY);

	///Configure the port
	struct termios options; /* Create the structure                          */

	//memset (&options, 0, sizeof options);					//////

	tcgetattr(fd, &options); /* Get the current attributes of the Serial port */

	cfsetispeed(&options, B115200); /* Set Read Speed as 115200                       */
	cfsetospeed(&options, B115200); /* Set Write Speed as 115200                    */

	/* 8N1 Mode */
	options.c_cflag &= ~PARENB; /* Disables the Parity Enable bit(PARENB),So No Parity   */
	options.c_cflag &= ~CSTOPB; /* CSTOPB = 2 Stop bits,here it is cleared so 1 Stop bit */
	options.c_cflag &= ~CSIZE;  /* Clears the mask for setting the data size             */
	options.c_cflag |= CS8;		/* Set the data bits = 8                                 */

	options.c_cflag &= ~CRTSCTS;	   /* No Hardware flow Control                         */
	options.c_cflag |= CREAD | CLOCAL; /* Enable receiver,Ignore Modem Control lines       */

	// options.c_iflag &= ~(IXON | IXOFF | IXANY);		   /* Disable XON/XOFF flow control both i/p and o/p */
	// options.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG); /* Non Cannonical mode                            */

	options.c_oflag &= ~OPOST; //*No Output Processing*/

	options.c_lflag &= ~ICANON;
	/* Setting Time outs */
	options.c_cc[VMIN] = 0; /* Read at least VMIN characters */
	// options.c_cc[VTIME] = 1; 		/* Wait indefinetly *10 s  */

	if ((tcsetattr(fd, TCSANOW, &options)) != 0) /* Set the attributes to the termios structure*/
		printf("\n  ERROR ! in Setting attributes");
	else
		printf("\n  BaudRate = 115200 \n  StopBits = 1 \n  Parity   = none\n\n");

	/*------------------------------- Read data from serial port -----------------------------*/
	//sleep(2); //required to make flush work, for some reason
	//tcflush(fd, TCIFLUSH);  						 /* Discards old data in the rx buffer, only used in code read            */

	int i = 0, j = 0;
	unsigned char chout[50]; /* Buffer to store the data received              */
	//memset(chout, 0, sizeof(chout)); // delete buffer

	int bytes_read; /* Number of bytes read by the read() system call */

	unsigned char cmd1[] = {0x01, 0x00, 0xFE, 0x26, 0x08, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
	unsigned char cmd1c[] = {0x04, 0xFF, 0x06, 0x7F, 0x06, 0x1};
	//unsigned char cmd2[] = {0x01, 0x04, 0xFE, 0x03, 0x03, 0x01, 0x00};

	// while (1){
	write(fd, cmd1, sizeof(cmd1));
	usleep(100000);
	bytes_read = read(fd, chout, /*size*/ sizeof(chout) - 1); /* Read the data return the number of bytes read  */
	if (bytes_read > 0)
	{
		printf("\n\n  The number of bytes read - Bytes Rxed: %d\n", bytes_read); /* Print the number of bytes read */
		for (i = 0; i < 255 /* sizeof(chout)*/; i++)							 /*printing only the received characters*/
		{
			printf("%x  ", chout[i]);
		}
		for (i = 0; i < 40 /* sizeof(chout)*/; i++) /*printing only the received characters*/
		{
			if (chout[i] == cmd1c[j])
			{
				j++;i++;
			}
			else printf("Not\n");
		}
		printf("\nOK\n");
		memset(chout, 0, sizeof(chout)); // delete buffer
	}
	// }

	// while (1)
	// { //Read from arduino
	// 	bytes_read = read(fd, chout, /*size*/ sizeof(chout) - 1); /* Read the data return the number of bytes read  */

	// 	if (bytes_read > 0)
	// 	{
	// 		printf("\n\n  The number of bytes read - Bytes Rxed: %d\n", bytes_read); /* Print the number of bytes read */
	// 		for (i = 0; i < bytes_read /* sizeof(chout)*/; i++) /*printing only the received characters*/
	// 		{
	// 			//printf("Hello ");
	// 			printf("%c ", chout[i]);
	// 		}
	// 		memset(chout, 0, sizeof(chout)); // delete buffer
	// 	}
	// }

	printf("\n\n +----------------------------------+\n\n\n");
	close(fd); /* Close the serial port */
}
