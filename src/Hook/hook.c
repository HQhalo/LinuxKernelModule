#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <asm/uaccess.h>
#include <asm/cacheflush.h>
#include <linux/syscalls.h>
#include <linux/delay.h>
#include <linux/fdtable.h>
#define CR0_WP 0x00010000  


MODULE_LICENSE("GPL");

void **syscall_table;
unsigned long **find_sys_call_table(void);

long (*real_open)(const char __user *filename, int flags, int mode);
long (*real_write)(unsigned int fd, const char __user * buf, size_t count);
unsigned long **find_sys_call_table() {
    
    unsigned long ptr;
    unsigned long *p;

    for (ptr = (unsigned long)sys_close;
         ptr < (unsigned long)&loops_per_jiffy;
         ptr += sizeof(void *)) {
             
        p = (unsigned long *)ptr;

        if (p[__NR_close] == (unsigned long)sys_close) {
            return (unsigned long **)p;
        }
    }
    
    return NULL;
}

long my_sys_open(const char __user *filename, int flags, int mode) {
    long ret;

    ret = real_open(filename, flags, mode);
	//if(strcmp(current->comm,"test")==0){
        printk(KERN_INFO "HOOK! : open system call\n");
    	printk(KERN_DEBUG "THREAD: %s,FILE:%s\n", current->comm,filename);
    //}
    return ret;
}
long my_sys_write(unsigned int fd, const char __user * buf, size_t count){
	struct files_struct *files = current->files;
	int f = real_write(fd,buf,count);
	
	//if(strcmp(current->comm,"rs:main")!=0){
	//if(strcmp(current->comm,"test")==0){
		char *tmp;
		char *pathname;
		struct file *file;
		struct path *path;

		spin_lock(&files->file_lock);
		file = fcheck_files(files, fd);
		if (!file) {
		    spin_unlock(&files->file_lock);
		    return -ENOENT;
		}

		path = &file->f_path;
		path_get(path);
		spin_unlock(&files->file_lock);

		tmp = (char *)__get_free_page(GFP_KERNEL);

		if (!tmp) {
		    path_put(path);
		    return -ENOMEM;
		}

		pathname = d_path(path, tmp, PAGE_SIZE);
		path_put(path);

		if (IS_ERR(pathname)) {
		    free_page((unsigned long)tmp);
		    return PTR_ERR(pathname);
		}

        printk(KERN_INFO "HOOK! : write system call\n");	
		printk(KERN_DEBUG "THREAD: %s, SIZE: %d ,FILE: %s\n", current->comm,f,pathname);
		
		free_page((unsigned long)tmp);
	//}
	return f;
	
}
static int __init syscall_init(void)
{
    printk(KERN_INFO "Initialize module hook!\n");
    unsigned long cr0;
  
    syscall_table = (void **)find_sys_call_table();

    if (!syscall_table) {
        printk(KERN_DEBUG "Cannot find the system call address\n"); 
        return -1;
    }

    cr0 = read_cr0();
    write_cr0(cr0 & ~CR0_WP);

    real_open = syscall_table[__NR_open];
    syscall_table[__NR_open] = my_sys_open;

	real_write = syscall_table[__NR_write];
    syscall_table[__NR_write] = my_sys_write;

    write_cr0(cr0);

    printk(KERN_INFO "Initialize succesfuly!\n");
    return 0;
}

static void __exit syscall_release(void)
{
    unsigned long cr0;
    cr0 = read_cr0();
    write_cr0(cr0 & ~CR0_WP);
    
    syscall_table[__NR_open] = real_open;
    syscall_table[__NR_write] = real_write;
    write_cr0(cr0);
    printk(KERN_INFO "Exit module hook!\n");
}

module_init(syscall_init);
module_exit(syscall_release);


