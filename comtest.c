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
	  printf("%d",errno);
    }
}


int main(int argc, char **argv)
{
  int SerialFileStream = -1;

  const char *DeviceName = "/dev/ttyAMA3";
  SerialFileStream = open(DeviceName, O_RDWR, 0);

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
        printf("%c\n",Char);
        WaitFdWriteable(SerialFileStream);
        if (write(SerialFileStream, &Char, 1) < 0){}
      }
  	 }
   }

}
