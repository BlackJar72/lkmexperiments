 
obj-m += hellomod.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

test:
	sudo dmesg -C
	sudo insmod hellomod.ko
	sudo rmmod hellomod.ko
	dmesg
