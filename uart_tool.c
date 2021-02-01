#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <asm/termios.h>

int main(int argc, char* argv[]) {

	int nread=0;
	char buff[4096];
	int retval, fd, speed;
	struct termios2 ntio;

	if (argc != 3) {
		printf("Usage: %s /dev/ttyXXX any_baudrate\n", argv[0], argv[0]);
		return -1;
	}

	speed = atoi(argv[2]);
	fd = open(argv[1], O_RDONLY);
	if (fd < 0) {
		perror("Open");
		return -1;
	}

	ioctl(fd, TCGETS2, &ntio);
	ntio.c_cflag &= ~CBAUD;
	ntio.c_cflag |= BOTHER;
	ntio.c_ispeed = speed;
	ntio.c_ospeed = speed;
	retval = ioctl(fd, TCSETS2, &ntio);

	if (retval == 0){
		printf("New baud configured");
		while(1){	
			nread=read(fd,buff,4096);
			buff[nread]=0;  //这个必须有
			if(nread>0)printf("%s",buff);//
			usleep(1);
		}
	}else{
		printf("error config band");
	}
	close(fd);
}
