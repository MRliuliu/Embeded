/*
 * char-read.c
 *
 * Character driver with read operation
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
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/semaphore.h>

MODULE_LICENSE ("GPL");

int hello_major = 250;
int hello_minor = 0;
int number_of_devices = 1;

struct hello_device
{
	char data[128];
	struct semaphore sem;
	struct cdev cdev;
} hello_device;

static int hello_open (struct inode *inode, struct file *file)
{
	printk (KERN_INFO "Hey! device opened\n");

	return 0;
}

static int hello_release (struct inode *inode, struct file *file)
{
	printk (KERN_INFO "Hmmm... device closed\n");

	return 0;
}

ssize_t hello_read (struct file *filp, char *buff, size_t count, loff_t *offp)
{
	ssize_t result = 0;

	if (count > 127) count = 127;

	if (down_interruptible(&hello_device.sem)) return -EINTR;

	if (copy_to_user (buff, hello_device.data, count)) 
	{
		result = -EFAULT;
	}
	else
	{
		printk (KERN_INFO "wrote %d bytes\n", count);
		result = count;
	}
	up(&hello_device.sem);

	return result;
}

ssize_t hello_write (struct file *filp, const char  *buf, size_t count, loff_t *f_pos)
{
	ssize_t ret = 0;

	if (count > 127) return -ENOMEM;
	
	if (down_interruptible(&hello_device.sem)) return -EINTR;

	if (copy_from_user (hello_device.data, buf, count)) {
		ret = -EFAULT;
	}
	else {
		hello_device.data[count] = '\0';
		printk (KERN_INFO"Received: %s\n", hello_device.data);
		ret = count;
	}
	up(&hello_device.sem);

	return ret;
}


struct file_operations hello_fops = {
	.owner = THIS_MODULE,
	.open  = hello_open,
	.release = hello_release,
	.read  = hello_read,
	.write = hello_write
};

static void char_reg_setup_cdev (void)
{
	int error;
	dev_t devno;

	devno = MKDEV (hello_major, hello_minor);
	cdev_init (&hello_device.cdev, &hello_fops);
	hello_device.cdev.owner = THIS_MODULE;
	error = cdev_add (&hello_device.cdev, devno , 1);
	if (error)
		printk (KERN_NOTICE "Error %d adding char_reg_setup_cdev", error);
}

static int __init hello_2_init (void)
{
	int result;
	dev_t devno;

	devno = MKDEV (hello_major, hello_minor);
	result = register_chrdev_region (devno, number_of_devices, "hello");

	if (result < 0) {
		printk (KERN_WARNING "hello: can't get major number %d\n", hello_major);
		return result;
	}

	char_reg_setup_cdev ();
	//init_MUTEX(&hello_device.sem);  for version not more than 2.6.35
	sema_init(&hello_device.sem, 1);
	printk (KERN_INFO "char device registered\n");

	return 0;
}

static void __exit hello_2_exit (void)
{
	dev_t devno = MKDEV (hello_major, hello_minor);

	cdev_del (&hello_device.cdev);

	unregister_chrdev_region (devno, number_of_devices);
}

module_init (hello_2_init);
module_exit (hello_2_exit);
