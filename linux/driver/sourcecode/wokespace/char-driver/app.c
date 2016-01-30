#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "common.h"
#define SIZE 127

void myread(int fd){
    char data[SIZE + 1];
    read(fd, data, SIZE);
    printf("read = %s\n", data);
}

void mywrite(int fd){
    char data[SIZE + 1] = "abcdefg\n";
    write(fd, data, strlen(data));
}


int main(int argv, char * argc[]){
    int fd;
	char ch;
    fd = open ("/dev/cool", O_RDWR);
    if(fd < 0){
        printf("Open /deve/hello error!\n");
        return fd;
    }
    myread(fd);
    mywrite(fd);
    myread(fd);


    ioctl(fd, ONE);
    ioctl(fd, TWO);
    ioctl(fd, TRE);
	printf("Please enter any key exit ...\n");
	ch = getchar();
    close(fd);

    return 0;
}
