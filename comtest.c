# include <stdio.h>
# include <stdlib.h>
# include <termio.h>
# include <unistd.h>
# include <fcntl.h>
# include <getopt.h>
# include <time.h>
# include <errno.h>
# include <string.h>


//-------------------------------------------------------------------------

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

//-------------------------------------------------------------------------

static inline void WaitFdWriteable(int Fd)
{
    fd_set WriteSetFD;
    FD_ZERO(&WriteSetFD);
    FD_SET(Fd, &WriteSetFD);
    if (select(Fd + 1, NULL, &WriteSetFD, NULL, NULL) < 0) {
	  printf("%s",strerror(errno));
    }
}

//-------------------------------------------------------------------------

int main(int argc, char **argv)
{
    int CommFd, TtyFd;

    struct termios TtyAttr;
    struct termios BackupTtyAttr;

    int DeviceSpeed = B38400;
    int TtySpeed = B38400;
    int ByteBits = CS8;
    const char *DeviceName = "/dev/ttyAMA3";
    const char *TtyName = "/dev/tty";
    int OutputHex = 0;
    int OutputToStdout = 0;
    int UseColor = 0;

    CommFd = open(DeviceName, O_RDWR, 0);

    if (fcntl(CommFd, F_SETFL, O_NONBLOCK) < 0)
     	printf("Unable set to NONBLOCK mode");


    memset(&TtyAttr, 0, sizeof(struct termios));
    TtyAttr.c_iflag = IGNPAR;
    TtyAttr.c_cflag = DeviceSpeed | HUPCL | ByteBits | CREAD | CLOCAL;
    TtyAttr.c_cc[VMIN] = 1;

    if (tcsetattr(CommFd, TCSANOW, &TtyAttr) < 0)
        printf("Unable to set comm port");

    TtyFd = open(TtyName, O_RDWR | O_NDELAY, 0);

    TtyAttr.c_cflag = TtySpeed | HUPCL | ByteBits | CREAD | CLOCAL;
    if (tcgetattr(TtyFd, &BackupTtyAttr) < 0)
	printf("Unable to get tty");

    if (tcsetattr(TtyFd, TCSANOW, &TtyAttr) < 0)
	printf("Unable to set tty");


//-------------------------


    for (;;) {
	unsigned char Char = 0;
	fd_set ReadSetFD;

	void OutputStdChar(FILE *File) {
	    char Buffer[10];
	    int Len = sprintf(Buffer, OutputHex ? "%.2X  " : "%c", Char);
	    fwrite(Buffer, 1, Len, File);
	}

	FD_ZERO(&ReadSetFD);

	FD_SET(CommFd, &ReadSetFD);
	FD_SET( TtyFd, &ReadSetFD);
#	define max(x,y) ( ((x) >= (y)) ? (x) : (y) )
	if (select(max(CommFd, TtyFd) + 1, &ReadSetFD, NULL, NULL, NULL) < 0) {
	    printf("%s",strerror(errno));
	}
#	undef max

	if (FD_ISSET(CommFd, &ReadSetFD)) {
	    while (read(CommFd, &Char, 1) == 1) {

		WaitFdWriteable(TtyFd);
		if (write(TtyFd, &Char, 1) < 0) {
	  	    printf("%s",strerror(errno));
		}
		if (OutputToStdout) {
		    if (UseColor)
			fwrite("\x1b[01;34m", 1, 8, stdout);
		    OutputStdChar(stdout);
		    if (UseColor)
			fwrite("\x1b[00m", 1, 8, stdout);
		    fflush(stdout);
		}
	    }
	}


	if (FD_ISSET(TtyFd, &ReadSetFD)) {
	    while (read(TtyFd, &Char, 1) == 1) {
       		static int EscKeyCount = 0;
		WaitFdWriteable(CommFd);
       		if (write(CommFd, &Char, 1) < 0) {
	  	    printf("%s",strerror(errno));
		}
		if (OutputToStdout) {
		    if (UseColor)
			fwrite("\x1b[01;31m", 1, 8, stderr);
		    OutputStdChar(stderr);
		    if (UseColor)
			fwrite("\x1b[00m", 1, 8, stderr);
		    fflush(stderr);
	        }


      		if (Char == '\x1b') {
                    EscKeyCount ++;
                    if (EscKeyCount >= 3)
                        goto ExitLabel;
                } else
                    EscKeyCount = 0;
	    }
        }

        string text[80];
        sprintf(text,"%s%c",text,Char);

        if (Char == 'M')printf("%s", text);
    }

ExitLabel:
    if (tcsetattr(TtyFd, TCSANOW, &BackupTtyAttr) < 0)
	printf("Unable to set tty");

    return 0;
}
