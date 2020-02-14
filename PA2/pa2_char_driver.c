
#include<linux/init.h>
#include<linux/module.h>

#include<linux/fs.h>
#include<linux/slab.h>
#include<linux/uaccess.h>

#define BUFFER_SIZE 1024
#define DEVICE_NAME "pa2_char_device"
#define MAJOR_NUMBER 240

MODULE_LICENSE("GPL"); // GNU Public License v2

char* device_buffer;
int open_count = 0;
int close_count = 0;

/* Define device_buffer and other global data structures you will need here */

ssize_t pa2_char_driver_read (struct file *pfile, char __user *buffer, size_t length, loff_t *offset)
{
	/* *buffer is the userspace buffer to where you are writing the data you want to be read from the device file*/
	/* length is the length of the userspace buffer*/
	/* offset will be set to current position of the opened file after read*/
	/* copy_to_user function: source is device_buffer and destination is the userspace buffer *buffer */

	int copy, read_len;

	if(BUFFER_SIZE - *offset < 0) {
		printk(KERN_ALERT "ERROR: Attempted to read outside the buffer size.");
		return EFAULT;
	}

	// max_read = BUFFER_SIZE - *offset;
	// if(max_read < length) {
	// 	length = max_read;
	// }

	copy = copy_to_user(buffer, device_buffer + *offset, length); // Returns number of bytes that could not be copied
	read_len = length - copy; // Number of bytes read.
	*offset += read_len;

	printk(KERN_ALERT "Read %d bytes from the device\n", read_len);


	return 0;
}


ssize_t pa2_char_driver_write (struct file *pfile, const char __user *buffer, size_t length, loff_t *offset)
{
	/* *buffer is the userspace buffer where you are writing the data you want to be written in the device file*/
	/* length is the length of the userspace buffer*/
	/* current position of the opened file*/
	/* copy_from_user function: destination is device_buffer and source is the userspace buffer *buffer */

	int copy, write_len;

	if(*offset >= BUFFER_SIZE || BUFFER_SIZE - length < *offset) {
		printk(KERN_ALERT "ERROR: Attempted to write outside of buffer.");
		return EFAULT;
	}
	copy = copy_from_user(device_buffer + *offset, buffer, length); // Returns number of bytes that could not be copied
	write_len = length - copy;
	*offset += write_len;
	printk(KERN_ALERT "Wrote %d bytes to the device\n", write_len);

	return length;
}


int pa2_char_driver_open (struct inode *pinode, struct file *pfile)
{
	/* print to the log file that the device is opened and also print the number of times this device has been opened until now*/
	printk(KERN_ALERT "Device open function called.\n");
	open_count++;
	printk(KERN_ALERT "Device has been opened %d time(s)\n", open_count);

	return 0;
}

int pa2_char_driver_close (struct inode *pinode, struct file *pfile)
{
	/* print to the log file that the device is closed and also print the number of times this device has been closed until now*/
	printk(KERN_ALERT "Device close function called.\n");
	close_count++;
	printk(KERN_ALERT "Device has been closed %d time(s)\n", open_count);

	return 0;
}

loff_t pa2_char_driver_seek (struct file *pfile, loff_t offset, int whence)
{
	/* Update open file position according to the values of offset and whence */
	loff_t pos = 0;

	switch(whence) {
		case 0: // SEEK_SET, set current position to the value of the offset
			pos = offset;
			break;
		case 1: // SEEK_CUR, current file position incremented by offset bytes
			pos = pfile->f_pos + offset;
			break;
		case 2: // SEEK_END, current position is set to offset bytes before the end of the file
			pos = BUFFER_SIZE - offset;
			break;
		default:
			return EINVAL;
	}
	if(pos < 0 || pos >= BUFFER_SIZE) {
		printk(KERN_ALERT "ERROR: Seek Function attempted to go beyond the scope of the file.\n");
		return pfile->f_pos;
	}

	pfile->f_pos = pos;
	return pos;
}

struct file_operations pa2_char_driver_file_operations = {
	.owner = THIS_MODULE,
	.read = pa2_char_driver_read,
	.write = pa2_char_driver_write,
	.open = pa2_char_driver_open,
	.release = pa2_char_driver_close,
	.llseek = pa2_char_driver_seek
};

static int __init pa2_char_driver_init(void)
{
	/* print to the log file that the init function is called.*/
	printk(KERN_ALERT "PA2 Init Function Called\n");

	device_buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);

	register_chrdev(MAJOR_NUMBER, DEVICE_NAME, &pa2_char_driver_file_operations);

	return 0;
}

static void __exit pa2_char_driver_exit(void)
{
	/* print to the log file that the exit function is called.*/
	printk(KERN_ALERT "PA2 Exit Function Called\n");
	kfree(device_buffer);

	unregister_chrdev(MAJOR_NUMBER, DEVICE_NAME);
}

/* add module_init and module_exit to point to the corresponding init and exit function*/
module_init(pa2_char_driver_init);
module_exit(pa2_char_driver_exit);
