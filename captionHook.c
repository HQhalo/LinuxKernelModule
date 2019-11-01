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
/*MY sys_call_table address*/
//ffffffff81601680
void **system_call_table_addr;
/*my custom syscall that takes process name*/
asmlinkage int (*custom_syscall) (char* name);
/*hook*/
asmlinkage int captain_hook(char* play_here) {
 /*do whatever here (print "HAHAHA", reverse their string, etc)
 But for now we will just print to the dmesg log*/
 printk(KERN_INFO "Pname Syscall:HOOK! HOOK! HOOK! HOOK!...ROOOFFIIOO!");
 return custom_syscall(play_here);
}
/*Make page writeable*/
int make_rw(unsigned long address){
 unsigned int level;
 pte_t *pte = lookup_address(address, &level);
 if(pte->pte &~_PAGE_RW){
 pte->pte |=_PAGE_RW;
 }
 return 0;
}
/* Make the page write protected */
int make_ro(unsigned long address){
 unsigned int level;
 pte_t *pte = lookup_address(address, &level);
 pte->pte = pte->pte &~_PAGE_RW;
 return 0;
}
static int __init entry_point(void){
 printk(KERN_INFO "Captain Hook loaded successfully..\n");
 /*MY sys_call_table address*/
 system_call_table_addr = (void*)0xffffffff81601680;
 /* Replace custom syscall with the correct system call name (write,open,etc) to hook*/
 custom_syscall = system_call_table_addr[__NR_pname];
 /*Disable page protection*/
 make_rw((unsigned long)system_call_table_addr);
 /*Change syscall to our syscall function*/
 system_call_table_addr[__NR_pname] = captain_hook;
 return 0;
}
static int __exit exit_point(void){
 printk(KERN_INFO "Unloaded Captain Hook successfully\n");
 /*Restore original system call */
 system_call_table_addr[__NR_pname] = custom_syscall;
 /*Renable page protection*/
 make_ro((unsigned long)system_call_table_addr);
 return 0;
}
module_init(entry_point);
module_exit(exit_point);