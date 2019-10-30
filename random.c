#include <linux/module.h> 
#define DRIVER_AUTHOR "8==D"
#define DRIVER_DESC "Random number"
static int __init init_hello(void)
{
    
}
static void __exit exit_hello(void)
{
    
}
module_init(init_hello);
module_exit(exit_hello);
MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_SUPPORTED_DEVICE("testdevice"); 