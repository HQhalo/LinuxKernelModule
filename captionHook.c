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
asmlinkage int (*real_open)(const char* __user, int, int);
 
asmlinkage int custom_open(const char* __user file_name, int flags, int mode)
{
    //if(strcmp(current->comm , "test")==0){
    
    printk("hooked: Thread: %s ,open(\"%s\", %X, %X)\n", current->comm,file_name,
                                                    flags,
                                                    mode);
    //}
  return real_open(file_name, flags, mode);
}  

asmlinkage int (*real_write) (int dirfd, const char *pathname, int flags, mode_t mode);

asmlinkage int my_sys_open(int dirfd, const char *pathname, int flags, mode_t mode) {
   

    if(strcmp(current->comm , "test")==0){
       // struct filename  *tmp = getname_flags(pathname,0,NULL);
       
        printk(KERN_INFO "HOOK!\n");
       // printk(KERN_INFO "THREAD NAME = %s  FILE = %s \n\n", current->comm,tmp->name);

    }
    
    return  (*custom_syscall)(dirfd,pathname,flags,mode);
}
asmlinkage int (*custom_syscall) (int dirfd, const char *pathname, int flags, mode_t mode);

asmlinkage int my_sys_open(int dirfd, const char *pathname, int flags, mode_t mode) {
   

    if(strcmp(current->comm , "test")==0){
       // struct filename  *tmp = getname_flags(pathname,0,NULL);
       
        printk(KERN_INFO "HOOK!\n");
       // printk(KERN_INFO "THREAD NAME = %s  FILE = %s \n\n", current->comm,tmp->name);

    }
    
    return  (*custom_syscall)(dirfd,pathname,flags,mode);
}

static int __init entry_point(void){
    write_cr0 (read_cr0 () & (~ 0x10000));
    printk(KERN_INFO "Captain Hook loaded successfully..\n");
    system_call_table_addr = (void*)0xffffffff82000280;
    real_open = system_call_table_addr[__NR_openat];
    
    system_call_table_addr[__NR_openat] = custom_open;
    write_cr0 (read_cr0 () | 0x10000);
    return 0;
}
static void __exit exit_point(void){
 write_cr0 (read_cr0 () & (~ 0x10000));
 printk(KERN_INFO "Unloaded Captain Hook successfully\n");
 system_call_table_addr[__NR_openat] = real_open;
 
 write_cr0 (read_cr0 () | 0x10000);

}
module_init(entry_point);
module_exit(exit_point);
