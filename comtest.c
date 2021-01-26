# include <stdio.h>
# include <stdlib.h>
# include <termio.h>
# include <unistd.h>
# include <fcntl.h>
# include <getopt.h>
# include <time.h>
# include <errno.h>
# include <string.h>
#define max(x,y) (((x)>=(y))?(x):(y))




static int SerialSpeed(const char *SpeedString)
{
    int SpeedNumber = atoi(SpeedString);
#   define TestSpeed(Speed) if (SpeedNumber == Speed) return B##Speed
    TestSpeed(1200);
    TestSpeed(2400);
    TestSpeed(4800);
    TestSpeed(9600);
    TestSpeed(19200);
    TestSpeed(38400);
    TestSpeed(57600);
    TestSpeed(115200);
    TestSpeed(230400);
    return -1;
}



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
  int DeviceSpeed = B38400;
  const char *DeviceName = "/dev/ttyAMA3";
  CommFd = open(DeviceName, O_RDWR, 0);
  if (fcntl(CommFd, F_SETFL, O_NONBLOCK) < 0)printf("Unable set to NONBLOCK mode");

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
