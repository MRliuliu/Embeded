#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/select.h>

#define  N  64

int main()
{
	int fd;
	char buf[N];
	fd_set rdfs;

	if ((fd = open("/dev/hello", O_RDONLY)) < 0)
	{
		perror("fail to open");
		exit(-1);
	}

	while ( 1 )
	{
		FD_ZERO(&rdfs);
		FD_SET(fd, &rdfs);
		FD_SET(STDIN_FILENO, &rdfs);
		select(fd+1, &rdfs, NULL, NULL, NULL);
		if (FD_ISSET(STDIN_FILENO, &rdfs))
		{
			fgets(buf, N, stdin);
			printf("%s\n", buf);
		}
		if (FD_ISSET(fd, &rdfs))
		{
			bzero(buf, N);
			read(fd, buf, N);
			printf("%s\n", buf);
		}
	}

	return 0;
}
