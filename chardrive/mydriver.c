#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/kernel.h>

static char mems[64];


static int major; 

static ssize_t device_write(struct file *filep, const char __user *user_buffer, size_t len, loff_t *offset){

	if(*offset >= sizeof(mems)){
		printk(KERN_INFO "Reached End of Memory\n"); 
		return 0; 
	}

	int errors = 0; 
	int bytes_read = len; 
	if((len + *offset) > sizeof(mems)){
		bytes_read = sizeof(mems) - *offset;
	};
	
	errors = copy_from_user(&mems[*offset], user_buffer, bytes_read);
	//if(errors == 0){
		//return len; 
	//}
	
	*offset += (bytes_read - errors); 

	printk(KERN_INFO "Write Test\n"); 
	return (bytes_read - errors); // num of bytes put into buffer

};

static ssize_t device_read(struct file *filep, char __user *user_buffer, size_t len, loff_t *offset){ //4096 bytes

	
	if(*offset >= sizeof(mems)){
		printk(KERN_INFO "Reached End of Memory\n"); 
		return 0; 
	}

	int errors = 0; 
	int bytes_read = len; 
	if((len + *offset) > sizeof(mems)){
		bytes_read = sizeof(mems) - *offset;
	};
	errors = copy_to_user(user_buffer, &mems[*offset], bytes_read);

	
	*offset += (bytes_read - errors); 
	
	printk(KERN_INFO "Read Test\n"); 
	return (bytes_read - errors); 
};

static struct file_operations fops = {
	.read = device_read,
	.write = device_write,
};

static int drive_init(void){

	major = register_chrdev(0, "drive_test", &fops); 
	printk(KERN_INFO "Mem Size is: %lu\n", sizeof(mems));

	if(major < 0){
		printk(KERN_INFO "Load Failed\n"); 
		return major; 
	}

	printk(KERN_INFO "Major is %d\n", major);
	return 0; 
};

static void drive_exit(void){
	unregister_chrdev(major, "drive_test"); 
	printk(KERN_INFO "Exit\n"); 
};


module_init(drive_init); 
module_exit(drive_exit); 
MODULE_LICENSE("GPL"); 

