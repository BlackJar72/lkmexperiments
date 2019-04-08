 
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

// Module Info
MODULE_LICENSE("GPL");
MODULE_AUTHOR("JaredBGreat");
MODULE_DESCRIPTION("Say hello forever");
MODULE_VERSION("0.0.1");

#define DEVICE_NAME "hello"
#define EXAMPLE_MSG "Hello World! \n\0"
#define MSG_BUFFER_LEN 15


// Data variables and buffers
static int major_num;
static int device_open_count = 0;
static char msg_buffer[MSG_BUFFER_LEN];
static char *msg_ptr;


// This structure points to all device functions
static struct file_operations file_ops = {
  .read = device_read,
  .write = device_write,
  .open = device_open,
  .release = device_release
};


// When a process reads from this device this gets called
static ssize_t device_read(struct file* flip, char* buffer, size_t len, loff_t* offset) {
  int bytes_read = 0;
  // loop back to begining when at the end
  if(*msg_ptr == 0) {
    msg_ptr = msg_buffer;
  }
  while(len && *msg_ptr) {
    // Buffer is in user data, not kernel, so it can't be reference with a pointer
    // The functions put_user() handles this for us.
    put_user(*(msg_ptr+), buffer++);
    len--;
    bytes_read++;
  }
  return bytes_read;
}


// Called when a process tries to write to the device
static ssize_t device_write(struct file* flip, const char* buffer, size_t len, loff_t *offset) {
  // This is a read only device
  printk(KERN_ALERT "This operation is not supported (read only device).\n");
  return -EINVAL;
}


// Called when a process opens the device;
static int device_open(struct inode* inode, struct file* file) {
  // If device is open return busy.
  if(device_open_count) {
    return -EBUSY;
  }
  device_open_count++;
  try_module_get(THIS_MODULE);
}


// Called when a process closes out device
static int device_close(struct inode* inode, struct file* file) {
  // Decrement the open counter and usage count (else the device won't close).
  device_open_count--;
  module_put(THIS_MODULE);
  return 0;
}


// Load the module
static int __init lkm_hello_init(void) {
  printk(KERN_INFO "Hello Kernel!\n");
  // Fil buffer with out message
  int i;
  for(i = 0; i < MSG_BUFFER_LEN; i++) {
    msg_buffer[i] = EXAMPLE_MSG[i];
  }
  // Set the msg_ptr to the buffer
  msg_ptr = msg_buffer;
  // Try to register the device
  major_num = register_chrdev(0, DEVICE_NAME, &file_ops);
  if(major_num < 0) {
    printk(KERN_ALERT "Could not register device: %d\n", major_num);
    return major_num;
  } else {
    printk(KERN_INFO "Hello module is loded with device major number %d\n", major_num);
    return 0;
  }
}


// Unloade the module
static void __exit lkm_hello_exit(void) {
  // Remember, we have to clean up after ourselvevs; 
  // unregister the character device.
  unregister_chrdev(major_num, DEVICE_NAME);
  printk(KERN_INFO "Goodbye Kernel!\n");
}


module_init(lkm_hello_init);
module_exit(lkm_hello_exit);

