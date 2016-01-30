/*
 * hello.c
 *
 * Debugging usage example
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


#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE ("GPL");

#undef PDEBUG
#ifdef HELLO_DEBUG
#define PDEBUG(fmt, args...) printk (KERN_DEBUG fmt, ##args)
#else
#define PDEBUG(fmt, args...)
#endif

int init_module (void)
{
	PDEBUG("Hello world\n");

	return 0;
}

void cleanup_module (void)
{
	PDEBUG("Goodbye world\n");
}
