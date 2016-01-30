#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/fs.h>

#define DEVICE_NAME "xmisc"

static int xopen(struct inode *inde, struct file *filp){
    printk("The xmisc is opened!\n");
    return 0;
}

static struct file_operations xmisc_fops = {
    .owner = THIS_MODULE,
    .open = xopen,
};

static struct miscdevice xmisc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &xmisc_fops,
};

static int __init xmisc_init(void){
    int ret = misc_register(&xmisc);
    printk("Init xmisc is OK.\n");
    return ret;
}

static void __exit xmisc_exit(void){
    misc_deregister(&xmisc);
    printk("Good bye misc.\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Richard.Wang");
MODULE_DESCRIPTION("The misc example");
module_init(xmisc_init);
module_exit(xmisc_exit);
