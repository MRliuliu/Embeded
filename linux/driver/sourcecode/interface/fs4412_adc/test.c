#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, const char *argv[])
{
	int fd;
	int data;

	fd = open("/dev/adc", O_RDWR);
	if (fd < 0) {
		perror("open");
		exit(1);
	}

	while(1) {
		read(fd, &data, sizeof(data));
		printf("Vol: %0.2fV\n", 1.8 * data / 4096);
		usleep(100000);
	}


	return 0;
}
