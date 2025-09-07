# Character-Driver-Details
Documentation for developing a custom character driver in Linux that
can read and write to a device file through a fixed memory buffer. 

# C Program to read/write to a device file

The goal here is to enable a C program to allow a device to be 
read and written to. This means creating a custom character driver
and inserting it into the kernel. The device file will be read using
the cat commands and written to using echo commands. 

First a new-driver folder- inside includes the custom character driver
along with C files for different OS architectures (x86 & arm64) for
testing. This should be a shared folder when compilied into QEMU-ARM (
explicitly stated as a path). 

Inside is the chardrive folder, which would initially include the 
*Makefile* and *mydriver.c* file. 

The make file includes the commands for copying into our linux
architecture the custom character driver. It follows: 

```sh
obj-m += mydriver.o

all: 
	make -C ../../linux-6.12.43 ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- M=$(PWD) modules
clean: 
	make -C ../../linux-6.12.43 ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- M=$(PWD) clean
```

"obj-m" stands for a module object, which directs this make file to build a 
custom kernel module (Generated as *mydriver.ko*). *mydriver.o* is the 
object type file to be built from the C source code in *mydriver.c* and is
used to generate the final kernel module *mydriver.ko*. 

The make commands are standard make commands for compiling the kernel (-C
tells the command to first cd into the linux kernel) and uses the make
processes there on the custom *mydriver.c* module located in chardrive.

![Running Make](/Images/char1.PNG)

![Created Module Obj](/Images/char1.5.PNG)

![Generated Files](/Images/char2.PNG)

# Setting up the Shared Driver in ARM architecture: 

Start the ARM emulation with: 
```sh
qemu-system-aarch64 -machine virt -cpu cortex-a57 -machine type=virt \
-m 1024 -smp 4 -kernel linux-6.12.43/arch/arm64/boot/Image --append "noinitrd root=/dev/vda rw console=ttyAMA0 loglevel=8" -nographic \
-drive if=none,file=rootfs_ext4.img,id=hd0 \
-device virtio-blk-device,drive=hd0 \
--fsdev local,id=shared_path,path=$PWD/new-driver,security_model=none \
-device virtio-9p-device,fsdev=shared_path,mount_tag=host_mount
```

In the ARM64 architecture, mount the shared directory using
the mount_tag specified during launch: 
```sh
mount -t 9p -o trans=virtio,version=9p2000.L host_mount /mnt/host_files
```

Upload Character Driver into QEMU Virt. Since we know that the
shared directory is in /mnt/hostfiles, we can:
```sh
insmod /mnt/host_files/chardrive/mydriver.ko
```

This will print the logs depicting either success or failure 
as well as displaying the buffer memory size and the major num.

![init Details](/Images/char3.PNG)

Create the drive_test driver file in the /dev folder: 
(Must be named drive_test as that is what was listed for the
driver in mydriver.c)
```sh
mknod /dev/drive_test c 511 0
```
(Using 511 because thats the logged major number when initially
uploading the driver). 

![Driver file](/Images/char4.PNG)

Now cat can read and echo can write to drive_test. 

![Functions](/Images/char5.PNG)

The custom driver can also be added directly into the kernel. 
Within the linux-6.12.43 folder going to:
```sh
cd drivers
```

Displays folders with different drivers already integrated into the
Kernel. For this custom driver, we can upload it into the "Misc" 
folder for testing. ```sh cd misc``` Move "mydriver.c" into the Misc folder,
then modify the corresponding Makefile and KConfig files inside. 

In the Makefile, add the line (Builds driver into configuration):
```sh
obj-$(CONFIG_MYDRIVER)        += mydriver.o
```

In the KConfig file, add the section:
```sh
config MYDRIVER
        tristate "Add a custom character driver"
        help
          Custom Kernel module to read/write to device file
```

Go into makeconfig and select the newly added configuration:
```sh
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- makeconfig
```

![](/Images/char6.PNG)

![](/Images/char7.PNG)

Then rebuild and rerun into QEMU. Verify in the boot logs that 
mydriver starts upon boot, which removes the need to insmod mydriver.ko
everytime on startup. 
```sh
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -j4
```

![Integated Driver](/Images/char8.PNG)
