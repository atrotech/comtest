# include <stdio.h>
# include <stdlib.h>
# include <termio.h>
# include <unistd.h>
# include <fcntl.h>

# include <time.h>
# include <errno.h>
# include <string.h>
#define max(x,y) (((x)>=(y))?(x):(y))







static inline void WaitFdWriteable(int Fd)
{
    fd_set WriteSetFD;
    FD_ZERO(&WriteSetFD);
    FD_SET(Fd, &WriteSetFD);
    if (select(Fd + 1, NULL, &WriteSetFD, NULL, NULL) < 0) {
	  printf("%s",strerror(errno));
    }
}


int main(int argc, char **argv)
{
  int CommFd;
  struct termios port_settings;

  const char *DeviceName = "/dev/ttyAMA3";
  CommFd = open(DeviceName, O_RDWR, 0);
  if (fcntl(CommFd, F_SETFL, O_NONBLOCK) < 0)printf("Unable set to NONBLOCK mode");
  //baudrate 9600, 8N1
  cfsetispeed(&port_settings, B115200);
  cfsetospeed(&port_settings, B115200);

  port_settings.c_cflag &= ~PARENB;   //Set no parity
  port_settings.c_cflag &= ~CSTOPB;   //Set 1 stop bit
  port_settings.c_cflag &= ~CSIZE;    //Set 8 bit data using mask bit
  port_settings.c_cflag |= CS8;
  port_settings.c_cflag &= ~CRTSCTS;  //No hadware hanshaking

  tcsetattr(CommFd, TCSANOW, &port_settings); // apply the settings to the port
 

  for (;;)
  {
  	unsigned char Char = 0;
  	fd_set ReadSetFD;

  	FD_ZERO(&ReadSetFD);
  	FD_SET(CommFd, &ReadSetFD);
  	if (select(CommFd + 1, &ReadSetFD, NULL, NULL, NULL) < 0){printf("%s",strerror(errno));}
  	if (FD_ISSET(CommFd, &ReadSetFD))
    {
      while (read(CommFd, &Char, 1) == 1)
      {
        printf("%c\n",Char);
        WaitFdWriteable(CommFd);
        if (write(CommFd, &Char, 1) < 0){printf("%s",strerror(errno));}
      }
  	 }
   }

}
