#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/interrupt.h>

MODULE_LICENSE("GPL");

struct resource *key1_res;
struct resource *key2_res;

irqreturn_t key_interrupt(int irqno, void *devid)
{
	printk("irqno = %d\n", irqno);

	return IRQ_HANDLED; 
}

int fs4412_dt_probe(struct platform_device *pdev) 
{
	int ret; 
	int irqflags;

	printk("match OK\n");

	key1_res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	key2_res = platform_get_resource(pdev, IORESOURCE_IRQ, 1);
	if (key2_res == NULL || key1_res == NULL) {
		printk("No resource !\n");
		return -ENODEV;
	}

	irqflags = IRQF_DISABLED | (key1_res->flags & IRQF_TRIGGER_MASK);

	ret = request_irq(key1_res->start, key_interrupt, irqflags, "key", NULL);
	if (ret < 0) {
		printk("failed request irq: irqno = irq_res->start");
		return ret; 
	}

	irqflags = IRQF_DISABLED | (key2_res->flags & IRQF_TRIGGER_MASK);

	ret = request_irq(key2_res->start, key_interrupt, irqflags, "key", NULL);
	if (ret < 0) {
		printk("failed request irq: irqno = irq_res->start");
		return ret; 
	}

	return 0;
}

int fs4412_dt_remove(struct platform_device *pdev)
{
	free_irq(key1_res->start, NULL);
	free_irq(key2_res->start, NULL);
	printk("remove OK\n");
	return 0;
}

static const struct of_device_id fs4412_dt_of_matches[] = {
	{ .compatible = "fs4412,key"},
	{ /* nothing to done! */},
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
