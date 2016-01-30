/*
 * test.c
 *
 * Test application code for char-read.c
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
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

int main (void) 
{
	int fd1, fd2;
	char buf[64];

	if ((fd1 = open ("/dev/hello0",O_RDONLY)) < 0)
	{
		perror("fail to open");
		return -1;
	}
	if ((fd2 = open ("/dev/hello1",O_RDONLY)) < 0)
	{
		perror("fail to open");
		return -1;
	}
	
	read(fd1, buf, 64);
	printf("read from device0 : %s\n", buf);
	read(fd2, buf, 64);
	printf("read from device1 : %s\n", buf);

	return 0;
}
