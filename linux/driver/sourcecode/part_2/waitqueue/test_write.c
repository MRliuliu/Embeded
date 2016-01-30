#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define N 90

int main()
{
	int i, fd;
	char buf[N];

	for (i=0; i<90; i++) 
	{
		buf[i] = i + 33;
	}

	if ((fd = open("/dev/hello", O_WRONLY)) < 0)
	{
		perror("fail to open");
	}
	printf("wrote %d bytes\n", write(fd, buf, N));
	close(fd);

	return 0;
}

