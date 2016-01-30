#include <linux/module.h> 
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>

MODULE_LICENSE("Dual BSD/GPL");


static int driver_probe(struct platform_device *dev)
{
	printk("platform: match ok!\n");
	return 0;
}

static int driver_remove(struct platform_device *dev)
{
	printk("platform: driver remove\n");
	return 0;
}

/*
static void device_release(struct device *dev)
{
	printk("platform: device remove\n");
}

struct platform_device test_device = {
	.id = -1,
	.name = "test_device",
	.dev.release = device_release, 
};
*/

struct platform_driver test_driver = {
	.probe = driver_probe,
	.remove = __devexit_p(driver_remove),
	.driver = {
		.name = "test_device",
	},
};

static int __init platform_driver_init(void)
{
	//platform_device_register(&test_device);
	platform_driver_register(&test_driver);
	return 0;
}

static void __exit platform_driver_exit(void)
{
	platform_driver_unregister(&test_driver);
	//platform_device_unregister(&test_device);
}

module_init(platform_driver_init);
module_exit(platform_driver_exit);
