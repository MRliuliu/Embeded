#include <linux/cdev.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <asm/uaccess.h>
#include <linux/ioctl.h>
#include "common.h"
#include <linux/fs.h>
#include <linux/device.h>

#define MAX 127

static int mymajor = 245;
static int myminor = 0;
static int number_of_devices = 1;

static struct hdevice{
	char data[MAX + 1];
	int lock_num;
	spinlock_t lock;
	struct cdev cdev;
	struct class * hello_class;
	struct device * hello_device;
}hdev;

static int hello_open(struct inode *inode, struct file *filp){
	spin_lock(&hdev.lock);
	if(hdev.lock_num){
		spin_unlock(&hdev.lock);
		return -EBUSY;
	}
	hdev.lock_num++;
	spin_unlock(&hdev.lock);
	printk("Hello, the device opened\n");
	return 0;
}

static int hello_release(struct inode *inode, struct file * filp){
	spin_lock(&hdev.lock);
	hdev.lock_num--;
	spin_unlock(&hdev.lock);

	printk("Hello, the device closed\n");
	return 0;
}

static ssize_t hello_read(struct file *filp, char __user *buff, size_t count, loff_t *offp){
	ssize_t ret = 0;
	if(*offp == 0){
		if(count > MAX)
			count = MAX;
		copy_to_user(buff, hdev.data, count);
		ret = count;
	}
	*offp = ret;
	return ret;
}

static ssize_t hello_write(struct file *filp, const char __user * buff, size_t count, loff_t *offp){
	ssize_t ret  = 0;
	if(count > MAX){
		ret = -EFAULT;
	}else{
		memset(hdev.data, 0, MAX);
		copy_from_user(hdev.data, buff, count);
		hdev.data[count] = '\0';
		ret = count;
	}
	return count;
}

long hello_ioctl(struct file *filp, unsigned int cmd, unsigned long arg){
	long  ret = 0;
	switch(cmd){
		case ONE:
			printk("The command is one\n");
			break;
		case TWO:
			printk("The command is two\n");
			break;

		default:
			ret = -EINVAL;
	}

	return ret;

}

static struct file_operations fops={
	.owner = THIS_MODULE,
	.open  = hello_open,
	.release = hello_release,
	.read  = hello_read,
	.write = hello_write,
	.unlocked_ioctl = hello_ioctl,
};

static void char_reg_cdev(void){
	printk("Hello init function!\n");
	spin_lock_init(&hdev.lock);
	strcpy(hdev.data, "Welcome to your\n");
	hdev.lock_num = 0;
}

static int hello_init(void){
	int ret;
	dev_t  devnu = 0;

	devnu = MKDEV(mymajor, myminor);
	ret = register_chrdev_region(devnu, number_of_devices, "hello");
	if(ret < 0){
		printk("Hello: can't get major number %d\n", mymajor);
		return ret;
	}

	cdev_init(&hdev.cdev, &fops);
	hdev.cdev.owner = THIS_MODULE;

	ret = cdev_add(&hdev.cdev, devnu, 1);
	if(ret){
		printk("Error %d adding cdev add!l", ret);
		goto err1;
	}

	hdev.hello_class = class_create(THIS_MODULE, "hello");
	if(IS_ERR(hdev.hello_class)){
		printk("Err: failed in createing class!\n");
		goto err2;
	}

	hdev.hello_device = device_create(hdev.hello_class, NULL, devnu, NULL, "cool");
	if(IS_ERR(hdev.hello_device)){
		printk("Err: failed in createing device!");
		goto err3;
	}

	char_reg_cdev();

	printk("Registered character driver \'hello\'!\n");
	return 0;

err3:
	class_destroy(hdev.hello_class);
err2:
	cdev_del(&hdev.cdev);
err1:
	unregister_chrdev_region(devnu, number_of_devices);
	return ret;

}

static void hello_exit(void){
	dev_t devnu = 0;
	devnu = MKDEV(mymajor, myminor);
	device_destroy(hdev.hello_class, devnu);
	class_destroy(hdev.hello_class);
	cdev_del(&hdev.cdev);
	unregister_chrdev_region(devnu, number_of_devices);
	printk("Bye driver \'hello\'!\n");

}

MODULE_AUTHOR("Richard.Wang");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("The hello example");
module_init(hello_init);
module_exit(hello_exit);
