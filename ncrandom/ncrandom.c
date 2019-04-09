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

static unsigned long long[2] seedSpace;
static timespec startTime;

