 
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("JaredBGreat");
MODULE_DESCRIPTION("Say hello forever");
MODULE_VERSION("0.0.1");

static int __init lkm_hello_init(void) {
  printk(KERN_INFO "Hello Kernel!\n");
  return 0;
}

static void __exit lkm_hello_exit(void) {
  printk(KERN_INFO "Goodbye Kernel!\n");
}

module_init(lkm_hello_init);
module_exit(lkm_hello_exit);

