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
//#include <linux/errno.h>
#include <asm/uaccess.h>
#include <linux/sched.h>
#include <linux/poll.h>
#include <linux/semaphore.h>

MODULE_LICENSE ("GPL");

int hello_major = 250;
int hello_minor = 0;
int number_of_devices = 1;

struct hello_device
{
	char data[128];
	int len;
	wait_queue_head_t rq, wq;
	struct semaphore sem;
	struct cdev cdev;
	struct fasync_struct *async_queue;
} hello_device;

static int hello_open (struct inode *inode, struct file *filp)
{
	filp->private_data = container_of(inode->i_cdev, struct hello_device, cdev);
	printk (KERN_INFO "Hey! device opened\n");

	return 0;
}

static int hello_release (struct inode *inode, struct file *filp)
{
	struct hello_device *dev = filp->private_data;

	if (dev->async_queue) fasync_helper(-1, filp, 0, &dev->async_queue);
	printk (KERN_INFO "Hmmm... device closed\n");

	return 0;
}

ssize_t hello_read (struct file *filp, char *buff, size_t count, loff_t *offp)
{
	ssize_t result = 0;
	struct hello_device *dev = filp->private_data;

	down(&dev->sem);
	while (hello_device.len == 0)
	{
		up(&dev->sem);
		if (filp->f_flags & O_NONBLOCK)
			return -EAGAIN;
		if (wait_event_interruptible(dev->rq, (dev->len != 0))) return -ERESTARTSYS;
		down(&dev->sem);
	}

	if (count > dev->len) count = dev->len;
	if (copy_to_user (buff, dev->data, count)) 
	{
		result = -EFAULT;
	}
	else
	{
		printk (KERN_INFO "wrote %d bytes\n", count);
		dev->len -= count;
		result = count;
		memcpy(dev->data, dev->data+count, dev->len);
	}
	up(&dev->sem);
	wake_up(&dev->wq);

	return result;
}

ssize_t hello_write (struct file *filp, const char  *buf, size_t count, loff_t *f_pos)
{
	ssize_t ret = 0;
	struct hello_device *dev = filp->private_data;

	if (count > 128) return -ENOMEM;
	
	down(&dev->sem);
	while (dev->len == 128)
	{
		up(&dev->sem);
		if (filp->f_flags & O_NONBLOCK)
			return -EAGAIN;
		if (wait_event_interruptible(dev->wq, (dev->len != 128))) return -ERESTARTSYS;
		down(&dev->sem);
	}

	if (count > (128 - dev->len)) count = 128 - dev->len;
	if (copy_from_user (dev->data+dev->len, buf, count)) {
		ret = -EFAULT;
	}
	else {
		dev->len += count;
		ret = count;
	}
	up(&dev->sem);
	wake_up(&dev->rq);

	/* 产生异步读信号 */
    if (dev->async_queue)
       kill_fasync(&dev->async_queue, SIGIO, POLL_IN);

	return ret;
}

static unsigned int hello_poll(struct file *filp, poll_table *wait)
{
	unsigned int mask = 0;
	struct hello_device *dev = filp->private_data;


	poll_wait(filp, &dev->rq, wait);
	poll_wait(filp, &dev->wq, wait);  

	down(&dev->sem);
	if (dev->len > 0)
	{
		mask |= POLLIN | POLLRDNORM; /*标示数据可获得*/
	}
	if (dev->len != 128)
	{
		mask |= POLLOUT | POLLWRNORM; /*标示数据可写入*/
	}
	up(&dev->sem);

	return mask;
}

static int hello_fasync(int fd, struct file *filp, int mode)
{
	struct hello_device *dev = filp->private_data; 
	return fasync_helper(fd, filp, mode, &dev->async_queue);
}

struct file_operations hello_fops = {
	.owner = THIS_MODULE,
	.open  = hello_open,
	.release = hello_release,
	.read  = hello_read,
	.write = hello_write,
	.poll = hello_poll,
	.fasync = hello_fasync
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
	init_waitqueue_head(&hello_device.rq);
	init_waitqueue_head(&hello_device.wq);
	sema_init(&hello_device.sem, 1);
	memset(hello_device.data, 0, 128);
	hello_device.len = 0;

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
