#include<linux/cdev.h>
#include<linux/module.h>
#include<linux/moduleparam.h>
#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/fs.h>
#include<asm/uaccess.h>
#define MAX 127 

static char data[MAX-1]="The device name is hello.\n";
static int mymajor = 245;   //主设备号
static int myminor = 0;       //次设备号
static int number_of_devices = 1;  //设备数量
static struct cdev cdev;     //

static int hello_open(struct inode * inode, struct file * file){
		printk("Hello, the device opened \n");
		return 0;
}

static ssize_t hello_read(struct file *flip, char __user *buff, size_t count, loff_t *offp){
	ssize_t ret = 0;
	printk("Hello, the device readed \n");
	if(*offp == 0){
		if(count > MAX)
			count = MAX;
		copy_to_user(buff, data, count);
		ret = count;	
	}	

	*offp = ret;
	return ret;
}

static ssize_t hello_write(struct file * filp, const char __user * buff, size_t count, loff_t * offp){
	ssize_t ret = 0;
	if(count > MAX){
		ret = -EFAULT;
	}else{
		memset(data, 0, MAX);
		copy_from_user(data, buff, count);
		data[count] = '\0';
		ret = count;
	}
	return ret;
}

static struct file_operations fops={
	.owner = THIS_MODULE,   //owner固定用法
	.open = hello_open,        //函数指针
	.read = hello_read,
	.write = hello_write,
};



static void char_reg_cdev(void){
	printk("Hello init function! \n");	
}


static int hello_init(void){
	int ret;          //注册设备号状态
	dev_t devnu = 0;    //设备号
	devnu = MKDEV(mymajor, myminor);
	ret = register_chrdev_region(devnu, number_of_devices, "hello");
	if(ret < 0){
	printk("Hello: can't get major number %d \n", mymajor);
	return ret;
}

	cdev_init(&cdev, &fops);           //把fops赋值给cdev
	cdev.owner = THIS_MODULE;
	ret = cdev_add(&cdev, devnu, 1);     //添加设备
if(ret){
	printk("Error %d adding cdev add!", ret);

	goto err1;

}

	char_reg_cdev();
	printk("Registered character driver \'hello\'!\n");
	return 0; 

err1:
	unregister_chrdev_region(devnu, number_of_devices);
	return ret;
}

static void hello_exit(void){
	dev_t devnu = 0;
	devnu = MKDEV(mymajor, myminor);
	cdev_del(&cdev);
	unregister_chrdev_region(devnu, number_of_devices);
	printk("Bye driver \'hello\' !\n");
}


	MODULE_AUTHOR("name");
	MODULE_LICENSE("name");
	MODULE_DESCRIPTION("The hello example");
	module_init(hello_init);          //入口
	module_exit(hello_exit);       //退出




















