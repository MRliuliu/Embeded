#include <linux/module.h> 
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>

MODULE_LICENSE("Dual BSD/GPL");


const static struct of_device_id test_id_table[]={
	[0]={
			.compatible ="fs4412,test_device",
		},
	{},
};
static int driver_probe(struct platform_device *dev)
{
	struct resource * res =NULL;
	printk("platform: match ok!\n");
 	res=platform_get_resource(dev,IORESOURCE_MEM,0);
	if(res == NULL)
	{
		printk("platform_get_resource error\n");
		return -EINVAL;
	}
	printk("res->start:%d\n",res->start); //11
	printk("res->end:%d\n",res->end);     //22
	printk("res->flags:%#x\n",res->flags); //0x200


 	res=platform_get_resource(dev,IORESOURCE_MEM,1);
	if(res == NULL)
	{
		printk("platform_get_resource error\n");
		return -EINVAL;
	}
	printk("res->start:%d\n",res->start); //33
	printk("res->end:%d\n",res->end);     //44
	printk("res->flags:%#x\n",res->flags); //0x200

 	res=platform_get_resource(dev,IORESOURCE_IRQ,0);
	if(res == NULL)
	{
		printk("platform_get_resource error\n");
		return -EINVAL;
	}
	printk("res->start:%d\n",res->start); //99
	printk("res->end:%d\n",res->end);     //99
	printk("res->flags:%#x\n",res->flags); //0x400
	
	printk("platform probe end\n");
	return 0;
}

static int driver_remove(struct platform_device *dev)
{
	printk("platform: driver remove\n");
	return 0;
}


struct platform_driver test_driver = {
	.probe = driver_probe,
	.remove = driver_remove,
	.driver = {
		.name = "test_device1111",
		.of_match_table = test_id_table,
	},
};

static int __init s5pc100_platform_init(void)
{
	printk("dri init\n");
	platform_driver_register(&test_driver);
	return 0;
}

static void __exit s5pc100_platform_exit(void)
{
	printk("dri exit\n");
	platform_driver_unregister(&test_driver);
}

module_init(s5pc100_platform_init);
module_exit(s5pc100_platform_exit);
