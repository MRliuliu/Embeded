#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>

#include <asm/io.h>
#include <asm/uaccess.h>

#include "fs4412_pwm.h"

MODULE_LICENSE("GPL");

#define TCFG0	0x00
#define TCFG1	0x04
#define TCON	0x08
#define TCNTB1	0x0C
#define TCMPB1	0x10

#define GPDCON 		0x114000A0
#define TIMER_BASE 	0x139D0000

static int pwm_major = 500;
static int pwm_minor = 0;
static int number_of_device = 1;

struct fs4412_pwm
{
	unsigned int *gpdcon;
	void __iomem *timer_base;
	struct cdev cdev;
};

static struct fs4412_pwm *pwm;
	
static int fs4412_pwm_open(struct inode *inode, struct file *file)
{
	writel((readl(pwm->gpdcon) & ~0xf) | 0x2, pwm->gpdcon);
	writel(readl(pwm->timer_base + TCFG0) | 0xff, pwm->timer_base + TCFG0);
	writel((readl(pwm->timer_base + TCFG1) & ~0xf) | 0x2, pwm->timer_base + TCFG1);
	writel(300, pwm->timer_base + TCNTB1);
	writel(150, pwm->timer_base + TCMPB1);
	writel((readl(pwm->timer_base + TCON) & ~0x1f) | 0x2, pwm->timer_base + TCON);
	//writel((readl(pwm->timer_base + TCON) & ~(0xf << 8)) | (0x9 << 8), pwm->timer_base + TCON);
	return 0;
}

static int fs4412_pwm_rlease(struct inode *inode, struct file *file)
{
	writel(readl(pwm->timer_base + TCON) & ~0xf, pwm->timer_base + TCON);
	return 0;
}

static long fs4412_pwm_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int data;

	if (_IOC_TYPE(cmd) != 'K')
		return -ENOTTY;

	if (_IOC_NR(cmd) > 3)
		return -ENOTTY;

	if (_IOC_DIR(cmd) == _IOC_WRITE)
		if (copy_from_user(&data, (void *)arg, sizeof(data)))
			return -EFAULT;

	switch(cmd)
	{
	case PWM_ON:
		writel((readl(pwm->timer_base + TCON) & ~0x1f) | 0x9, pwm->timer_base + TCON);
		break;
	case PWM_OFF:
		writel(readl(pwm->timer_base + TCON) & ~0x1f, pwm->timer_base + TCON);
		break;
	case SET_PRE:
		writel(readl(pwm->timer_base + TCON) & ~0x1f, pwm->timer_base + TCON);
		writel((readl(pwm->timer_base + TCFG0) & ~0xff) | (data & 0xff), pwm->timer_base + TCFG0);
		writel((readl(pwm->timer_base + TCON) & ~0x1f) | 0x9, pwm->timer_base + TCON);
		break;
	case SET_CNT:
		writel(data, pwm->timer_base + TCNTB1);
		writel(data >> 1, pwm->timer_base + TCMPB1);
		break;
	}

	return 0;
}
	
static struct file_operations fs4412_pwm_fops = {
	.owner = THIS_MODULE,
	.open = fs4412_pwm_open,
	.release = fs4412_pwm_rlease,
	.unlocked_ioctl = fs4412_pwm_ioctl,
};

static int __init fs4412_pwm_init(void)
{
	int ret;
	dev_t devno = MKDEV(pwm_major, pwm_minor);

	ret = register_chrdev_region(devno, number_of_device, "pwm");
	if (ret < 0) {
		printk("faipwm : register_chrdev_region\n");
		return ret;
	}

	pwm = kmalloc(sizeof(*pwm), GFP_KERNEL);
	if (pwm == NULL) {
		ret = -ENOMEM;
		printk("faipwm: kmalloc\n");
		goto err1;
	}
	memset(pwm, 0, sizeof(*pwm));

	cdev_init(&pwm->cdev, &fs4412_pwm_fops);
	pwm->cdev.owner = THIS_MODULE;
	ret = cdev_add(&pwm->cdev, devno, number_of_device);
	if (ret < 0) {
		printk("faipwm: cdev_add\n");
		goto err2;
	}

	pwm->gpdcon = ioremap(GPDCON, 4);
	if (pwm->gpdcon == NULL) {
		ret = -ENOMEM;
		printk("faipwm: ioremap gpdcon\n");
		goto err3;
	}

	pwm->timer_base = ioremap(TIMER_BASE, 0x20);
	if (pwm->timer_base == NULL) {
		ret = -ENOMEM;
		printk("failed: ioremap timer_base\n");
		goto err4;
	}


	return 0;
err4:
	iounmap(pwm->gpdcon);
err3:
	cdev_del(&pwm->cdev);
err2:
	kfree(pwm);
err1:
	unregister_chrdev_region(devno, number_of_device);
	return ret;
}

static void __exit fs4412_pwm_exit(void)
{
	dev_t devno = MKDEV(pwm_major, pwm_minor);
	iounmap(pwm->timer_base);
	iounmap(pwm->gpdcon);
	cdev_del(&pwm->cdev);
	kfree(pwm);
	unregister_chrdev_region(devno, number_of_device);
}

module_init(fs4412_pwm_init);
module_exit(fs4412_pwm_exit);
