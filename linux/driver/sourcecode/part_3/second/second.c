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
#include <linux/timer.h> /*����timer.hͷ�ļ�*/

#define SECOND_MAJOR 250    /*Ԥ���second�����豸��*/

static int second_major = SECOND_MAJOR;

/*second�豸�ṹ��*/
struct second_dev
{
	struct cdev cdev; /*cdev�ṹ��*/
	int counter;/* һ�������˶����룿*/
	struct timer_list s_timer; /*�豸Ҫʹ�õĶ�ʱ��*/
} second_dev;

/*��ʱ��������*/
static void second_timer_handle(unsigned long arg)
{
	mod_timer(&second_dev.s_timer,jiffies + HZ);
	second_dev.counter++;

	printk(KERN_NOTICE "current jiffies is %ld\n", jiffies);
}

/*�ļ��򿪺���*/
int second_open(struct inode *inode, struct file *filp)
{
	/*��ʼ����ʱ��*/
	init_timer(&second_dev.s_timer);
	second_dev.s_timer.function = second_timer_handle;
	second_dev.s_timer.expires = jiffies + HZ;

	add_timer(&second_dev.s_timer); /*��ӣ�ע�ᣩ��ʱ��*/
	second_dev.counter = 0; //������0

	return 0;
}

/*�ļ��ͷź���*/
int second_release(struct inode *inode, struct file *filp)
{
	del_timer(&second_dev.s_timer);

	return 0;
}

/*globalfifo������*/
static ssize_t second_read(struct file *filp, char __user *buf, size_t count,
		loff_t *ppos)
{  

	if(put_user(second_dev.counter, (int*)buf))
		return -EFAULT;
	else
		return sizeof(unsigned int);  
}

/*�ļ������ṹ��*/
static const struct file_operations second_fops =
{
	.owner = THIS_MODULE, 
	.open = second_open, 
	.release = second_release,
	.read = second_read,
};

/*��ʼ����ע��cdev*/
static void second_setup_cdev(struct second_dev *dev, int index)
{
	int err, devno = MKDEV(second_major, index);

	cdev_init(&dev->cdev, &second_fops);
	dev->cdev.owner = THIS_MODULE;
	err = cdev_add(&dev->cdev, devno, 1);
	if (err)
		printk(KERN_NOTICE "Error %d adding LED%d", err, index);
}

/*�豸����ģ����غ���*/
int second_init(void)
{
	int ret;
	dev_t devno = MKDEV(second_major, 0);

	/* �����豸��*/
	ret = register_chrdev_region(devno, 1, "second");
	if (ret < 0)
		return ret;

	second_setup_cdev(&second_dev, 0);

	return 0;
}

/*ģ��ж�غ���*/
void second_exit(void)
{
	cdev_del(&second_dev.cdev);   /*ע��cdev*/
	unregister_chrdev_region(MKDEV(second_major, 0), 1); /*�ͷ��豸��*/
}

module_init(second_init);
module_exit(second_exit);
