#cd /root/Test/
#vim Makefile

KDIR = /lib/modules/`uname -r`/build

all:
	make -C $(KDIR) M=`pwd`
clean:
	make -C $(KDIR) M=`pwd` clean