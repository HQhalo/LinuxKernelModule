#include <asm/unistd.h>
#include <asm/cacheflush.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <asm/pgtable_types.h>
#include <linux/highmem.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/moduleparam.h>
#include <linux/unistd.h>
#include <asm/cacheflush.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("D0hnuts");

void **system_call_table_addr;


asmlinkage int (*custom_syscall) (const char __user *pathname);


asmlinkage int my_sys_open( const char __user *pathname) {
    if(strcmp(current->comm , "test")==0){
        char path[100];
        copy_from_user(path,pathname,strnlen_user(pathname,100));
        printk(KERN_INFO "HOOK!");
        printk("THREAD NAME = %s  FILE = %s \n\n", current->comm,pathname);

    }
    return  custom_syscall(pathname);

}

static int __init entry_point(void){
    write_cr0 (read_cr0 () & (~ 0x10000));
    printk(KERN_INFO "Captain Hook loaded successfully..\n");
    system_call_table_addr = (void*)0xffffffff82000280;
    custom_syscall = system_call_table_addr[__NR_openat];
    
    system_call_table_addr[__NR_openat] = my_sys_open;
    write_cr0 (read_cr0 () | 0x10000);
    return 0;
}
static void __exit exit_point(void){
 write_cr0 (read_cr0 () & (~ 0x10000));
 printk(KERN_INFO "Unloaded Captain Hook successfully\n");
 system_call_table_addr[__NR_openat] = custom_syscall;
 
 write_cr0 (read_cr0 () | 0x10000);

}
module_init(entry_point);
module_exit(exit_point);
