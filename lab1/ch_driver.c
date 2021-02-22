#include <linux/module.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kokov A.T., Rogalenko N.A.");
MODULE_DESCRIPTION("lab1");
MODULE_VERSION("0.313");
 
static dev_t first;
static struct cdev c_dev;
static struct class *cl;
 
static struct proc_dir_entry* entry;
 
static char char_buf[256];
 
static int my_open(struct inode *i, struct file *f)
{
    printk(KERN_INFO "Driver: open()\n");
    return 0;
}
 
static int my_close(struct inode *i, struct file *f)
{
    printk(KERN_INFO "Driver: close()\n");
    return 0;
}
 
static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
    size_t count = strlen(char_buf);
    if (len < count)
    {
        return 0;
    }
    if (copy_to_user(buf, char_buf, count) != 0)
    {
        return -EFAULT;
    }
    *off = count;
    return count;
}
 
static ssize_t my_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
    if (len < 0 || len > 256)
    {
        return 0;
    }
    if (copy_from_user(char_buf, buf, len) != 0)
    {
        return -EFAULT;
    }
    char_buf[len] = '\0';
    *off = len;
    return len;
}
 
static ssize_t proc_write(struct file *file, const char __user * ubuf, size_t count, loff_t* ppos)
{
    printk(KERN_DEBUG "Attempt to write proc file");
    return -1;
}
 
static ssize_t proc_read(struct file *file, char __user * buf, size_t len, loff_t* off)
{
    size_t count = strlen(char_buf);
    if ((len < count) || (*off > 0))
    {
        return 0;
    }
    if (copy_to_user(buf, char_buf, count) != 0)
    {
        return -EFAULT;
    }
    *off = count;
    return count;
}
 
static struct file_operations mychdev_fops =
{
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_close,
    .read = my_read,
    .write = my_write
};
 
static struct file_operations proc_fops = {
    .owner = THIS_MODULE,
    .read = proc_read,
    .write = proc_write,
};
 
static int __init lab_init(void)
{
    char_buf[0] = '\0';
    entry = proc_create("var3", 0444, NULL, &proc_fops);
    printk(KERN_INFO "%s: proc file is created\n", THIS_MODULE->name);
 
    printk(KERN_INFO "%s: initializing character device\n", THIS_MODULE->name);
    if (alloc_chrdev_region(&first, 0, 1, "ch_dev") < 0)
      {
        return -1;
      }
    if ((cl = class_create(THIS_MODULE, "chardrv")) == NULL)
      {
        unregister_chrdev_region(first, 1);
        return -1;
      }
    if (device_create(cl, NULL, first, NULL, "var3") == NULL)
      {
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        return -1;
      }
    cdev_init(&c_dev, &mychdev_fops);
    if (cdev_add(&c_dev, first, 1) == -1)
      {
        device_destroy(cl, first);
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        return -1;
      }
    printk(KERN_INFO "%s: initialization complete\n", THIS_MODULE->name);
    return 0;
}
 
static void __exit lab_exit(void)
{
    proc_remove(entry);
    printk(KERN_INFO "%s: proc file is deleted\n", THIS_MODULE->name);
 
    printk(KERN_INFO "%s: deinitializing character device\n", THIS_MODULE->name);
    cdev_del(&c_dev);
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
    printk(KERN_INFO "%s: deinitialization complete\n", THIS_MODULE->name);
}
 
module_init(lab_init);
module_exit(lab_exit);
 
