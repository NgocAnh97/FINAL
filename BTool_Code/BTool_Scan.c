/*====================================================================================================*/
/* Serial Port Programming in C (Serial Port Write)                                                   */
/* Non Cannonical mode                                                                                */
/*----------------------------------------------------------------------------------------------------*/
/* Program writes a character to the serial port at 9600 bps 8N1 format                               */
/* Baudrate - 9600                                                                                    */
/* Stop bits -1                                                                                       */
/* No Parity                                                                                          */

/*-------------------------------------------------------------*/
/* termios structure -  /usr/include/asm-generic/termbits.h    */
/* use "man termios" to get more info about  termios structure */
/*-------------------------------------------------------------*/

#include <stdio.h>
#include <fcntl.h>   /* File Control Definitions           */
#include <termios.h> /* POSIX Terminal Control Definitions */
#include <unistd.h>  /* UNIX Standard Definitions 	   */
#include <errno.h>   /* ERROR Number Definitions           */

void main(void)
{
    int fd; /*File Descriptor*/
    int i = 0;

    printf("\n +----------------------------------+");
    printf("\n |        Serial Port Write         |");
    printf("\n +----------------------------------+");

    /*------------------------------- Opening the Serial Port -------------------------------*/

    /* Change /dev/ttyUSB0 to the one corresponding to your system */

    fd = open("/dev/ttyACM0", O_RDWR | O_NOCTTY | O_NDELAY); /* ttyACM0 is the Converter   */
                                                             /* O_RDWR Read/Write access to serial port           */
                                                             /* O_NOCTTY - No terminal will control the process   */
                                                             /* O_NDELAY -Non Blocking Mode,Does not care about-  */
                                                             /* -the status of DCD line,Open() returns immediatly */

    if (fd == -1) /* Error Checking */
        printf("\n  Error! in Opening ttyACM0  ");
    else
        printf("\n  ttyACM0 Opened Successfully ");

    /*---------- Setting the Attributes of the serial port using termios structure --------- */

    struct termios SerialPortSettings; /* Create the structure                          */

    tcgetattr(fd, &SerialPortSettings); /* Get the current attributes of the Serial port */

    cfsetispeed(&SerialPortSettings, B115200); /* Set Read  Speed as 115200                       */
    cfsetospeed(&SerialPortSettings, B115200); /* Set Write Speed as 115200                       */

    SerialPortSettings.c_cflag &= ~PARENB; /* Disables the Parity Enable bit(PARENB),So No Parity   */
    SerialPortSettings.c_cflag &= ~CSTOPB; /* CSTOPB = 2 Stop bits,here it is cleared so 1 Stop bit */
    SerialPortSettings.c_cflag &= ~CSIZE;  /* Clears the mask for setting the data size             */
    SerialPortSettings.c_cflag |= CS8;     /* Set the data bits = 8                                 */

    SerialPortSettings.c_cflag &= ~CRTSCTS;       /* No Hardware flow Control                         */
    SerialPortSettings.c_cflag |= CREAD | CLOCAL; /* Enable receiver,Ignore Modem Control lines       */

    SerialPortSettings.c_iflag &= ~(IXON | IXOFF | IXANY);         /* Disable XON/XOFF flow control both i/p and o/p */
    SerialPortSettings.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG); /* Non Cannonical mode                            */

    SerialPortSettings.c_oflag &= ~OPOST; /*No Output Processing*/

    if ((tcsetattr(fd, TCSANOW, &SerialPortSettings)) != 0) /* Set the attributes to the termios structure*/
        printf("\n  ERROR ! in Setting attributes");
    else
        printf("\n  BaudRate = 115200 \n  StopBits = 1 \n  Parity   = none");

    /*------------------------------- Write data to serial port -----------------------------*/
    printf("\n  Board DE10 gui: \n");
    /*-----------------------Signal to initial launchPad--------------------------*/
    char write_buffer[42] = {0x01, 0x00, 0xFE, 0x26, 0x08, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  
                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00 };   

    /*-----------------------Signal to scan other BLE devices--------------------------*/
    char write_buffer[] = {0x01, 0x04, 0xFE, 0x03, 0x03, 0x01, 0x00};

    int bytes_written = 0; /* Value for storing the number of bytes written to the port */

    bytes_written = write(fd, write_buffer, sizeof(write_buffer)); /* use write() to send data to port                                            */
                                                                   /* "fd"                   - file descriptor pointing to the opened serial port */
                                                                   /*	"write_buffer"         - address of the buffer containing data	            */
                                                                   /* "sizeof(write_buffer)" - amount of memory is allocated  */             
    //printf("\n  %c written to ttyACM0", write_buffer);
    printf("\n  %d Bytes written to ttyACM0", bytes_written);
    for (i = 0; i < sizeof(write_buffer); i++)                     /*printing only the received characters*/
    {
        printf("\n Character written at %d: %x", i, write_buffer[i]);
    }

    printf("\n +----------------------------------+\n\n");
    usleep (2000);                  //suspend execution for microsecond

    /*------------------------------- Read data from serial port -----------------------------*/
    
    printf("\n +----------------------------------+");
    printf("\n |        Serial Port Read          |");
    printf("\n +----------------------------------+");


    /* Setting Time outs */
	SerialPortSettings.c_cc[VMIN] = 100; 		/* Read at least VMIN characters */
	SerialPortSettings.c_cc[VTIME] = 0; 		/* Wait indefinetly   */

    tcflush(fd, TCIFLUSH); /* Discards old data in the rx buffer            */

    char chout[];        /* Buffer to store the data received              */
    size_t nbytes;
    ssize_t bytes_read = 0;    /* Number of bytes read by the read() system call, int is alright*/
    //nbytes = sizeof(chout);

    bytes_read = read(fd, &chout, sizeof(chout));                                   /* Read the data                   */
    printf("\n\n  The number of bytes read - Bytes Rxed: %d", bytes_read); /* Print the number of bytes read */
    printf("\n\n  ");
    for (i = 0; i < /*sizeof(chout)*/ bytes_read; i++)                     /*printing only the received characters*/
    {
        printf("\n Character read at %d: %x\n", i, chout[i]);
        //printf("Got so ky tu %d \n",sizeof(chout));
    }
    printf("\n\n +----------------------------------+\n\n\n");

    close(fd); /* Close the Serial port */
}
