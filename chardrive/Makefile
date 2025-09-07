obj-m += mydriver.o

all: 
	make -C ../../linux-6.12.43 ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- M=$(PWD) modules
clean: 
	make -C ../../linux-6.12.43 ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- M=$(PWD) clean
