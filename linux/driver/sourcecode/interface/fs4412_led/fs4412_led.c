#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>

#include <asm/io.h>
#include <asm/uaccess.h>

#include "fs4412_led.h"

MODULE_LICENSE("Dual BSD/GPL");

#define LED_MA 500
#define LED_MI 0
#define LED_NUM 1

#define FS4412_GPF3CON	0x114001E0
#define FS4412_GPF3DAT	0x114001E4

#define FS4412_GPX1CON	0x11000C20
#define FS4412_GPX1DAT	0x11000C24

#define FS4412_GPX2CON	0x11000C40
#define FS4412_GPX2DAT	0x11000C44




static unsigned int *gpf3con;
static unsigned int *gpf3dat;

static unsigned int *gpx1con;
static unsigned int *gpx1dat;

static unsigned int *gpx2con;
static unsigned int *gpx2dat;


struct cdev cdev;

void fs4412_led_on(int nr)
{
	switch(nr) {
		case 1: 
			writel(readl(gpx2dat) | 1 << 7, gpx2dat);
			break;
		case 2: 
			writel(readl(gpx1dat) | 1 << 0, gpx1dat);
			break;
		case 3: 
			writel(readl(gpf3dat) | 1 << 4, gpf3dat);
			break;
		case 4: 
			writel(readl(gpf3dat) | 1 << 5, gpf3dat);
			break;
	}
}

void fs4412_led_off(int nr)
{
	switch(nr) {
		case 1: 
			writel(readl(gpx2dat) & ~(1 << 7), gpx2dat);
			break;
		case 2: 
			writel(readl(gpx1dat) & ~(1 << 0), gpx1dat);
			break;
		case 3: 
			writel(readl(gpf3dat) & ~(1 << 4), gpf3dat);
			break;
		case 4: 
			writel(readl(gpf3dat) & ~(1 << 5), gpf3dat);
			break;
	}
}

static int s5pv210_led_open(struct inode *inode, struct file *file)
{
	return 0;
}
	
static int s5pv210_led_release(struct inode *inode, struct file *file)
{
	return 0;
}
	
static long s5pv210_led_unlocked_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int nr;

	if(copy_from_user((void *)&nr, (void *)arg, sizeof(nr)))
		return -EFAULT;

	if (nr < 1 || nr > 4)
		return -EINVAL;

	switch (cmd) {
		case LED_ON:
			fs4412_led_on(nr);
			break;
		case LED_OFF:
			fs4412_led_off(nr);
			break;
		default:
			printk("Invalid argument");
			return -EINVAL;
	}

	return 0;
}

int fs4412_led_ioremap(void)
{
	int ret;

	gpf3con = ioremap(FS4412_GPF3CON, 4);
	if (gpf3con == NULL) {
		printk("ioremap gpf3con\n");
		ret = -ENOMEM;
		return ret;
	}

	gpf3dat = ioremap(FS4412_GPF3DAT, 4);
	if (gpf3dat == NULL) {
		printk("ioremap gpx2dat\n");
		ret = -ENOMEM;
		return ret;
	}


	gpx1con = ioremap(FS4412_GPX1CON, 4);
	if (gpx1con == NULL) {
		printk("ioremap gpx2con\n");
		ret = -ENOMEM;
		return ret;
	}

	gpx1dat = ioremap(FS4412_GPX1DAT, 4);
	if (gpx1dat == NULL) {
		printk("ioremap gpx2dat\n");
		ret = -ENOMEM;
		return ret;
	}
	gpx2con = ioremap(FS4412_GPX2CON, 4);
	if (gpx2con == NULL) {
		printk("ioremap gpx2con\n");
		ret = -ENOMEM;
		return ret;
	}

	gpx2dat = ioremap(FS4412_GPX2DAT, 4);
	if (gpx2dat == NULL) {
		printk("ioremap gpx2dat\n");
		ret = -ENOMEM;
		return ret;
	}

	return 0;
}

void fs4412_led_iounmap(void)
{
	iounmap(gpf3con);
	iounmap(gpf3dat);
	iounmap(gpx1con);
	iounmap(gpx1dat);
	iounmap(gpx2con);
	iounmap(gpx2dat);
}

void fs4412_led_io_init(void)
{

	writel((readl(gpf3con) & ~(0xff << 16)) | (0x11 << 16), gpf3con);
	writel(readl(gpx2dat) & ~(0x3<<4), gpf3dat);

	writel((readl(gpx1con) & ~(0xf << 0)) | (0x1 << 0), gpx1con);
	writel(readl(gpx1dat) & ~(0x1<<0), gpx1dat);

	writel((readl(gpx2con) & ~(0xf << 28)) | (0x1 << 28), gpx2con);
	writel(readl(gpx2dat) & ~(0x1<<7), gpx2dat);
}
	
struct file_operations s5pv210_led_fops = {
	.owner = THIS_MODULE,
	.open = s5pv210_led_open,
	.release = s5pv210_led_release,
	.unlocked_ioctl = s5pv210_led_unlocked_ioctl,
};

static int s5pv210_led_init(void)
{
	dev_t devno = MKDEV(LED_MA, LED_MI); 
	int ret;

	ret = register_chrdev_region(devno, LED_NUM, "newled");
	if (ret < 0) {
		printk("register_chrdev_region\n");
		return ret;
	}

	cdev_init(&cdev, &s5pv210_led_fops);
	cdev.owner = THIS_MODULE;
	ret = cdev_add(&cdev, devno, LED_NUM);
	if (ret < 0) {
		printk("cdev_add\n");
		goto err1;
	}

	ret = fs4412_led_ioremap();
	if (ret < 0)
		goto err2;


	fs4412_led_io_init();

	printk("Led init\n");

	return 0;
err2:
	cdev_del(&cdev);
err1:
	unregister_chrdev_region(devno, LED_NUM);
	return ret;
}

static void s5pv210_led_exit(void)
{
	dev_t devno = MKDEV(LED_MA, LED_MI);

	fs4412_led_iounmap();
	cdev_del(&cdev);
	unregister_chrdev_region(devno, LED_NUM);
	printk("Led exit\n");
}

module_init(s5pv210_led_init);
module_exit(s5pv210_led_exit);
