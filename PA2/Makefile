obj-m :=	helloModule.o
obj-m +=	pa2_char_driver.o

all:
	#Build LKM and tester
	sudo make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc tester.c -o tester

	#Load LKM
	sudo insmod helloModule.ko
	sudo insmod pa2_char_driver.ko

	#Create device file
	sudo mknod -m 777 /dev/pa2_character_device c 240 0

load:
	sudo insmod helloModule.ko
	sudo insmod pa2_char_driver.ko

unload:
	sudo rmmod helloModule.ko
	sudo rmmod pa2_char_driver.ko

clean:
	sudo make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

	#Unload LKM
	sudo rmmod helloModule.ko
	sudo rmmod pa2_char_driver.ko

	#Clean device file
	sudo rm /dev/pa2_character_device

	#Clean tmp files and compiled kernel files
	rm -f *.o $(EXEC)
	rm -f *.ko $(EXEC)
	rm -f *.mod.c $(EXEC)
	rm -f *.symvers $(EXEC)
	rm -f *.order $(EXEC)
	rm tester