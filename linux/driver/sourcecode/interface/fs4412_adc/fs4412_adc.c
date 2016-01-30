#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/of.h>
#include <linux/sched.h>
#include <linux/interrupt.h>

#include <asm/io.h>
#include <asm/uaccess.h>

#define FS4412_ADCCON		0x00
#define FS4412_ADCDAT		0x0C
#define FS4412_ADCCLRINT	0x18
#define FS4412_ADCMUX		0x1C

MODULE_LICENSE("GPL");

struct resource *mem_res;
struct resource *irq_res;
void __iomem *adc_base;
unsigned int adc_major = 500;
unsigned int adc_minor = 0;
struct cdev cdev;
int flags = 0;
wait_queue_head_t readq;

static int fs4412_adc_open(struct inode *inode, struct file *file)
{
	return 0;
}
	
static int fs4412_adc_release(struct inode *inode, struct file *file)
{
	return 0;
}

static ssize_t fs4412_adc_read(struct file *file, char *buf, size_t count, loff_t *loff)
{
	int data = 0;

	if (count != 4)
		return -EINVAL;

	writel(3, adc_base + FS4412_ADCMUX);
	writel(1<<0 | 1<<14 | 0xff<<6 | 0x1<<16, adc_base + FS4412_ADCCON);
	//printk("adccon = %x\n", readl(adc_base + FS4412_ADCCON));
	wait_event_interruptible(readq, flags == 1);
	
	data = readl(adc_base + FS4412_ADCDAT) & 0xfff;

	//printk("data = %x\n", data);

	if (copy_to_user(buf, &data, sizeof(data)))
		return -EFAULT;

	flags = 0;

	return count;
}


irqreturn_t adc_interrupt(int irqno, void *devid)
{
	flags = 1;
	writel(0, adc_base + FS4412_ADCCLRINT);
	wake_up_interruptible(&readq);

	return IRQ_HANDLED; 
}

struct file_operations fs4412_dt_adc_fops = {
	.owner = THIS_MODULE,
	.open = fs4412_adc_open,
	.release = fs4412_adc_release,
	.read = fs4412_adc_read,

};

int fs4412_dt_probe(struct platform_device *pdev) 
{
	int ret; 
	dev_t devno = MKDEV(adc_major, adc_minor);

	printk("match OK\n");

	init_waitqueue_head(&readq);

	mem_res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	irq_res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (mem_res == NULL || irq_res == NULL) {
		printk("No resource !\n");
		return -ENODEV;
	}

	printk("mem = %x: irq = %d\n", mem_res->start, irq_res->start);


	adc_base = ioremap(mem_res->start, mem_res->end - mem_res->start);
	if (adc_base == NULL) {
		printk("failed to ioremap address reg\n");
		return -EINVAL;
	};

	ret = request_irq(irq_res->start, adc_interrupt, IRQF_DISABLED, "adc", NULL);
	if (ret < 0) {
		printk("failed request irq: irqno = %d\n", irq_res->start);
		goto err1;
	}

	printk("major = %d, minor = %d, devno = %x\n", adc_major, adc_minor, devno);
	ret = register_chrdev_region(devno, 1, "fs4412-adc");
	if (ret < 0) {
		printk("failed register char device region\n");
		goto err2;
	}

	cdev_init(&cdev, &fs4412_dt_adc_fops);
	cdev.owner = THIS_MODULE;
	ret = cdev_add(&cdev, devno, 1);
	if (ret < 0) {
		printk("failed add device\n");
		goto err3;
	}

	return 0;

err3:
	unregister_chrdev_region(devno, 1);
err2:
	free_irq(irq_res->start, NULL);
err1:
	iounmap(adc_base);
	return ret;
}

int fs4412_dt_remove(struct platform_device *pdev)
{
	dev_t devno = MKDEV(adc_major, adc_minor);
	printk("remove OK\n");
	cdev_del(&cdev);
	unregister_chrdev_region(devno, 1);
	free_irq(irq_res->start, NULL);
	iounmap(adc_base);
	return 0;
}

static const struct of_device_id fs4412_dt_of_matches[] = {
	{ .compatible = "fs4412,adc"},
	{ /* nothing to be done! */},
};

MODULE_DEVICE_TABLE(of, fs4412_dt_of_matches);

struct platform_driver fs4412_dt_driver = {
	.driver = {
		.name = "fs4412-dt",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(fs4412_dt_of_matches),
	},
	.probe = fs4412_dt_probe,
	.remove = fs4412_dt_remove,
};

module_platform_driver(fs4412_dt_driver);
