#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/fs_struct.h>
#include <linux/time.h>
#include <linux/uaccess.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jared Blackburn");
MODULE_DESCRIPTION("Generates non-cryptographic random numbers");
MODULE_VERSION("1.0.0");

#define SHIFT1 19
#define SHIFT2 31
#define SHIFT3 23
#define SHIFT4 7
#define MULTIPLIER 5443
#define ADDITION 15485863312338621L
#define DEVICE_NAME "ncrandom"

static int major_num;
static int devCount = 0;
// I don't think this is needed, but its 
// the kernel so playing it safe until certain.
static unsigned long long seedSpace[3];
static struct timespec startTime;
static size_t readCounter;


static int deviceOpen(struct inode*, struct file*);
static int deviceClose(struct inode*, struct file*);
static ssize_t deviceRead(struct file*, char*, size_t, loff_t*);
static ssize_t deviceWrite(struct file*, const char*, size_t, loff_t*);
static unsigned char generate(void);


static struct file_operations fileOps = {
  .read = deviceRead,
  .write = deviceWrite,
  .open = deviceOpen,
  .release = deviceClose
};


static unsigned char generate(void) {
  seedSpace[1] *= MULTIPLIER;
  seedSpace[1] += ADDITION;
  seedSpace[1] ^= seedSpace[1] >> SHIFT1;
  seedSpace[1] ^= seedSpace[1] << SHIFT2;
  seedSpace[1] ^= seedSpace[1] >> SHIFT3;
  seedSpace[1] ^= seedSpace[1] << SHIFT4;
  seedSpace[0] = 0;
  seedSpace[2] = 0;
  return (unsigned char)((seedSpace[1] >> (((seedSpace[1] >> 8) & 31) + 16)) & 0xff);
}


// Handles reading from the device
static ssize_t deviceRead(struct file* file, char* buffer, size_t len, loff_t* offset) {
  for(readCounter = 0; readCounter < len; readCounter++) {
    put_user(generate(), buffer + readCounter);
  }
  return readCounter;
}


// Returns error for write attempts (read-only device)
static ssize_t deviceWrite(struct file* file, const char* buffer, size_t len, loff_t* offset) {
  printk(KERN_ALERT "Attempted to write to read-only device ncrandom.");
  return -EINVAL;
}


// Opens the device for access
static int deviceOpen(struct inode* inode, struct file* file) {
  if(devCount) {
    return -EBUSY;
  } else {
    devCount++;
    try_module_get(THIS_MODULE);
    return 0;
  }
}


// Closes (releases) the device
static int deviceClose(struct inode* inode, struct file* file) {
  devCount--;
  module_put(THIS_MODULE);
  return 0;
}


// Start the module / insert into kernel
static int __init ncrandomInit(void) {
  printk(KERN_INFO "Loading kernel module ncrandom.\n");  
  getnstimeofday(&startTime);
  seedSpace[1] = startTime.tv_nsec;
  major_num = register_chrdev(0, DEVICE_NAME, &fileOps);
  if(major_num < 0) {
    printk(KERN_ALERT "Failed to register ncrandom device: %d\n", major_num);
    return major_num;
  } else {
    printk(KERN_INFO "Successfully registered ncrandom device as %d.\n", major_num);
    return 0;
  } 
}


// Clean-up resources, remove the module from the kernel and shut down
static void __exit ncrandomExit(void) {
  unregister_chrdev(major_num, DEVICE_NAME);
  printk(KERN_INFO "Remove ncrandom device module.\n");
}


module_init(ncrandomInit);
module_exit(ncrandomExit);
