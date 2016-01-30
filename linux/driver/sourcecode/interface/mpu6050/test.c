#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "mpu6050.h"

int main(int argc, const char *argv[])
{
	int fd;
	union mpu6050_data data; 
	
	fd = open("/dev/mpu6050", O_RDWR);
	if (fd < 0) {
		perror("open");
		exit(1);
	}

	while(1) {
		ioctl(fd, GET_ACCEL, &data);
		printf("acceleration data: x = %04x, y = %04x, z = %04x\n", 
				data.accel.x, data.accel.y, data.accel.z);

		ioctl(fd, GET_GYRO, &data);
		printf("gyroscope data: x = %04x, y = %04x, z = %04x\n", 
				data.accel.x, data.accel.y, data.accel.z);

		sleep(1);
	}

	close(fd);

	return 0;
}
