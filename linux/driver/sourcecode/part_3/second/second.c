/*======================================================================
    A "seond" device driver as an example of kernel timer
    
    The initial developer of the original code is Baohua Song
    <author@linuxdriver.cn>. All Rights Reserved.
======================================================================*/
#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <asm/io.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <linux/timer.h> /*包括timer.h头文件*/

#define SECOND_MAJOR 250    /*预设的second的主设备号*/

static int second_major = SECOND_MAJOR;

/*second设备结构体*/
struct second_dev
{
	struct cdev cdev; /*cdev结构体*/
	int counter;/* 一共经历了多少秒？*/
	struct timer_list s_timer; /*设备要使用的定时器*/
} second_dev;

/*定时器处理函数*/
static void second_timer_handle(unsigned long arg)
{
	mod_timer(&second_dev.s_timer,jiffies + HZ);
	second_dev.counter++;

	printk(KERN_NOTICE "current jiffies is %ld\n", jiffies);
}

/*文件打开函数*/
int second_open(struct inode *inode, struct file *filp)
{
	/*初始化定时器*/
	init_timer(&second_dev.s_timer);
	second_dev.s_timer.function = second_timer_handle;
	second_dev.s_timer.expires = jiffies + HZ;

	add_timer(&second_dev.s_timer); /*添加（注册）定时器*/
	second_dev.counter = 0; //计数清0

	return 0;
}

/*文件释放函数*/
int second_release(struct inode *inode, struct file *filp)
{
	del_timer(&second_dev.s_timer);

	return 0;
}

/*globalfifo读函数*/
static ssize_t second_read(struct file *filp, char __user *buf, size_t count,
		loff_t *ppos)
{  

	if(put_user(second_dev.counter, (int*)buf))
		return -EFAULT;
	else
		return sizeof(unsigned int);  
}

/*文件操作结构体*/
static const struct file_operations second_fops =
{
	.owner = THIS_MODULE, 
	.open = second_open, 
	.release = second_release,
	.read = second_read,
};

/*初始化并注册cdev*/
static void second_setup_cdev(struct second_dev *dev, int index)
{
	int err, devno = MKDEV(second_major, index);

	cdev_init(&dev->cdev, &second_fops);
	dev->cdev.owner = THIS_MODULE;
	err = cdev_add(&dev->cdev, devno, 1);
	if (err)
		printk(KERN_NOTICE "Error %d adding LED%d", err, index);
}

/*设备驱动模块加载函数*/
int second_init(void)
{
	int ret;
	dev_t devno = MKDEV(second_major, 0);

	/* 申请设备号*/
	ret = register_chrdev_region(devno, 1, "second");
	if (ret < 0)
		return ret;

	second_setup_cdev(&second_dev, 0);

	return 0;
}

/*模块卸载函数*/
void second_exit(void)
{
	cdev_del(&second_dev.cdev);   /*注销cdev*/
	unregister_chrdev_region(MKDEV(second_major, 0), 1); /*释放设备号*/
}

module_init(second_init);
module_exit(second_exit);
