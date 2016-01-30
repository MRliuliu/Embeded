/*
 * test.c
 *
 * Test application code for testing the ioctl driver
 *
 * Copyright (C) 2005 Farsight
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#include "hello.h"

int main (void) 
{
	int fd;

	fd = open ("/dev/hello",O_RDWR);
	if (fd < 0) {
	  perror ("fd open failed");
	  exit(-1);
	}
	
	if (ioctl (fd, HELLO_ONE) < 0)
	{
		perror("fail to ioctl");
	}

	if (ioctl (fd, HELLO_TWO) < 0)
	{
		perror("fail to ioctl");
	}

	if (ioctl(fd, 1010) < 0)
	{
		perror("fail to ioctl");
	}
	close (fd);
	
	return 0;
}
