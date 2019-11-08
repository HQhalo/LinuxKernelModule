#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include<linux/random.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "DeviceQT"
#define CLASS_NAME "ClassQT"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("QT");
MODULE_DESCRIPTION("Random number");

static dev_t first;
static struct cdev DeviceQT;
static struct class *classQT; 

ssize_t random_read (struct file *f , char __user *buf, size_t len, loff_t *off); 

ssize_t random_read (struct file *f , char __user *buf, size_t len, loff_t *off)
{
    char temp[4]={"0000"}; 
    get_random_bytes(temp,sizeof(char)*len);
    copy_to_user(buf,temp,4);
    len =4;
    return 4;
}

static struct file_operations pugs_fops =
{
    .owner = THIS_MODULE,
    .read = random_read,  
};

static int __init random_init(void) /* Constructor */
{

    if (alloc_chrdev_region(&first, 1, 1, DEVICE_NAME) < 0)
    {
        return -1;
    }
    if ((classQT = class_create(THIS_MODULE, CLASS_NAME)) == NULL)
    {
        unregister_chrdev_region(first, 1);
        return -1;
    }
    if (device_create(classQT, NULL, first, NULL,DEVICE_NAME) == NULL)
    {
        class_destroy(classQT);
        unregister_chrdev_region(first, 1);
        return -1;
    }

    cdev_init(&DeviceQT, &pugs_fops);
    if (cdev_add(&DeviceQT, first, 1) == -1)
    {
        device_destroy(classQT, first);
        class_destroy(classQT);
        unregister_chrdev_region(first, 1);
        return -1;
    }
   printk(KERN_INFO "QT Random: device class created correctly\n");
    return 0;
}

static void __exit random_exit(void) /* Destructor */
{
    cdev_del(&DeviceQT);
    device_destroy(classQT, first);
    class_destroy(classQT);
    unregister_chrdev_region(first, 1);
    printk(KERN_INFO "Exit QT random\n");
    
}

module_init(random_init);
module_exit(random_exit);

