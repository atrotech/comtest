# include <stdio.h>
# include <stdlib.h>
# include <termio.h>
# include <unistd.h>
# include <fcntl.h>



static inline void WaitFdWriteable(int Fd)
{
    fd_set WriteSetFD;
    FD_ZERO(&WriteSetFD);
    FD_SET(Fd, &WriteSetFD);
    if (select(Fd + 1, NULL, &WriteSetFD, NULL, NULL) < 0) {
	  printf("%d",12);
    }
}


int main(int argc, char **argv)
{
  int SerialFileStream = -1;

  const char *DeviceName = "/dev/ttyAMA3";
  SerialFileStream = open(DeviceName, O_RDWR | O_NOCTTY | O_NDELAY);

  if (fcntl(SerialFileStream, F_SETFL, O_NONBLOCK) < 0)printf("Unable set to NONBLOCK mode");
  //baudrate 9600, 8N1
  

  struct termios options;
	tcgetattr(SerialFileStream, &options);
	options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;		//<Set baud rate
	options.c_iflag = IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;
	tcflush(SerialFileStream, TCIFLUSH);
	tcsetattr(SerialFileStream, TCSANOW, &options);

  	//----- TX BYTES -----
	unsigned char tx_buffer[20];
	unsigned char *p_tx_buffer;
	
	p_tx_buffer = &tx_buffer[0];
	*p_tx_buffer++ = 'H';
	*p_tx_buffer++ = 'e';
	*p_tx_buffer++ = 'l';
	*p_tx_buffer++ = 'l';
	*p_tx_buffer++ = 'o';
	
	if (SerialFileStream != -1)
	{
		int count = write(SerialFileStream, &tx_buffer[0], (p_tx_buffer - &tx_buffer[0]));		//Filestream, bytes to write, number of bytes to write
		if (count < 0)
		{
			printf("UART TX error\n");
		}
	}
 

  for (;;)
  {
  	unsigned char Char = 0;
  	fd_set ReadSetFD;

  	FD_ZERO(&ReadSetFD);
  	FD_SET(SerialFileStream, &ReadSetFD);
  	if (select(SerialFileStream + 1, &ReadSetFD, NULL, NULL, NULL) < 0){}
  	if (FD_ISSET(SerialFileStream, &ReadSetFD))
    {
      while (read(SerialFileStream, &Char, 1) == 1)
      {
        printf("%c\n",'x');
        WaitFdWriteable(SerialFileStream);
        if (write(SerialFileStream, &Char, 1) < 0){}
      }
  	 }
   }

}
