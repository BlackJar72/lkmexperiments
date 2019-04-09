#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/time.h>
#include <linux/uaccess.h>
#include <asm/uaccess.h>

// See:
// https://stackoverflow.com/questions/13552536/get-current-time-in-seconds-in-kernel-module

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jared Blackburn");
MODULE_DESCRIPTION("Generates non-cryptographic random numbers");
MODULE_VERSION("0.0.1");

#define SHIFT1 19
#define SHIFT2 31
#define SHIFT3 23
#define SHIFT4 7
#define MULTIPLIER 5443
#define ADDITION 15485863312338621L

static int major_num;
static int device_open_count = 0;
// I don't think this is needed, but its 
// the kernel so playing it safe until certain.
static unsigned long long[3] seedSpace;
static struct timespec startTime;
static size_t readCounter;


static int deviceOpen(struct inode*, struct file*);
static int deviceClose(struct inode*, struct file*);
static ssize_t deviceRead(struct file*, char*, size_t, loff_t*);
static ssize_t deviceWrite(struct file*, const char*, size_t, loff_t*);


static struct file_operations fileOps = {
  .read = deviceRead,
  .write = deviceWrite,
  .open = deviceOpen,
  .release = deviceClose
}


inline static unsigned char generate() {
  seedSpace[1] *= MULTIPLIER;
  seedSpace[1] += ADDITION;
  seedSpace[1] ^= seedSpace[1] >> SHIFT1;
  seedSpace[1] ^= seedSpace[1] << SHIFT2;
  seedSpace[1] ^= seedSpace[1] >> SHIFT3;
  seedSpace[1] ^= seedSpace[1] << SHIFT4;
  seedSpace[0] = 0;
  seedSpace[20 = 0;
  return (unsigned char)((seedSpace[1] >> (((seedSpace[1] >> 8) & 32) + 16)) & 0xff);
}


// Handles reading from the device
static ssize_t deviceRead(struct file* file, char* buffer, size_t len, loff_t* offset) {
  for(readCounter = 0; readCounter < len; readCounter++ {
    buffer[i] = generate();
  }
  return readCounter;
}


// Returns error for write attempts (read-only device)
static ssize_t deviceWrite(struct file* file, const char* buffer, size_t, loff_t* offset) {
  printk(KERN_ALERT "Attempted to write to read-only device ncrandom.");
  return -EINVAL;
}


// Opens the device for access
static int deviceOpen(struct inode* inode, struct file* file) {
  // TODO
}


// Closes (releases) the device
static int deviceClose(struct inode* inode, struct file* file) {
  // TODO
}


// Start the module / insert into kernel
static int __init ncrandomInit(void) {
  printk(KERN_INFO "Loading kernel module ncrandom.");
  // TODO
}


// Clean-up resources, remove the module from the kernel and shut down
static void __exit ncrandomExit(void) {
  // TODO
}


module_init(ncrandomInit);
module_exit(ncrandomInit);
