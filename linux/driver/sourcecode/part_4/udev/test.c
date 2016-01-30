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
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

int main (void) 
{
	int fd;
	char buf[50] = {0};
	fd = open ("/dev/farsight_dev",O_RDWR);
	if (fd < 0) {
	  printf ("fd open failed\n");
	  exit(0);
	}
	printf ("\n/dev/farsight_dev opened, fd=%d\n", fd);
	printf ("Read returns %d\n", read (fd, buf, sizeof(buf)));
	buf[50]='\0';
	printf ("buf = %s\n", buf);
	close (fd);
	printf ("/dev/farsiht_dev closed :)\n");
	
	return 0;
}
