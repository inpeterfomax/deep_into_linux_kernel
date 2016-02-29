#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#define BUF_SIZE 64
#define LOG_FILE "/test.log"
#define STOP_CHAR '#'
int serial_configure(int fd, int nSpeed, int flowctl);
int indexof(unsigned char * buf, char ch);
int main(int argc, char **argv)
{
	int fd,fdlog;
  	int i,j;
	int nread = 0;
	int stop = 0;
	int first =1;
  	if( argc!=2 ) { 
  	printf("USAGE: uartread /dev/ttySx\n");
		return -1;	 
	}	
	if ((fd = open(argv[1], O_RDWR | O_NOCTTY )) < 0) {
		printf("Can't Open Serial Port");
		return -1;
	}
	if ( serial_configure(fd, 115200, 1) < 0) {
		printf("set_opt error");
		return -1;
	}
	if ((fdlog = open(LOG_FILE, O_CREAT | O_WRONLY | O_NOCTTY | O_TRUNC  )) < 0) {
		printf("Can't Open log file");
		return -1;
	}  
	do {
		unsigned char buf[BUF_SIZE+1] = {'\0'};
		int cnt = read(fd, buf, BUF_SIZE);
		if(cnt < 0) {
			perror("read error");
			return -1;
		} else {
			int index = indexof(buf,STOP_CHAR);
			if ( index >=0 ) {
				cnt = index + 1;
				stop= 1;
			}	 
			cnt = write(fdlog, buf, cnt);
			nread += cnt;
		}		
	} while( !stop );
	
	printf("Total bytes received: %d\n", nread);
	close(fd);
	close(fdlog);
	return 0;
}


int indexof(unsigned char * buf, char ch)
{
	int index = 0;
	while(*buf != ch && *buf != '\0' ) {
		buf++;
		index++;
	}
	if(*buf == '\0')
		return -1;
	else 
		return index;
}
int serial_configure(int fd, int nSpeed, int flowctl)
{
	struct termios tio;
	bzero(&tio, sizeof(tio));
	switch (nSpeed) {
	case 9600:
		cfsetispeed(&tio, B9600);
		cfsetospeed(&tio, B9600);
		break;
	case 38400:
		cfsetispeed(&tio, B38400);
		cfsetospeed(&tio, B38400);
		break;  			  
	case 115200:
		cfsetispeed(&tio, B115200);
		cfsetospeed(&tio, B115200);
		break;
	case 460800:
		cfsetispeed(&tio, B460800);
		cfsetospeed(&tio, B460800);
		break;
	case 921600:
		cfsetispeed(&tio, B921600);
		cfsetospeed(&tio, B921600);
		break;
	default:
		cfsetispeed(&tio, B9600);
		cfsetospeed(&tio, B9600);
		break;
	}
	tio.c_cflag |= CLOCAL | CREAD;
	if(flowctl)
		tio.c_cflag |= CRTSCTS;
	else 
		tio.c_cflag &= ~CRTSCTS;

	tcflush(fd, TCIFLUSH);

	if ((tcsetattr(fd, TCSANOW, &tio)) != 0) {
		printf("com set error");
		return -1;
	}
	return 0;
}

