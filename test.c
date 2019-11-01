#include<stdio.h>

#include<stdlib.h>

#include<unistd.h>

#include<sys/ioctl.h>

#include<sys/types.h>

#include<sys/stat.h>

#include<fcntl.h>

#include<sys/select.h>

#include<sys/time.h>

#include<errno.h>

int main(int argc, char** argv){
     int ret, fd;

     printf("Starting device test code example...\n");
     fd = open(argv[1], O_RDWR);            
     if (fd < 0){
          perror("Failed to open the device...");
          return errno;
     }
     
     int i;
     read(fd,&i,sizeof(i));

     printf("Random value: %d\n",i);
     printf("End of the program\n");
     return 0;

}
