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
#include <linux/unistd.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kokov A.T., Rogalenko N.A.");
MODULE_DESCRIPTION("lab1");
MODULE_VERSION("0.313");

static dev_t first;
static struct cdev c_dev;
static struct class *cl;

static struct proc_dir_entry* entry;

void proc_file_handler(char* input);
const char* SWAP_FILE = "/swap_file ";

// -------------- function for string processing --------------

int get_sum_of_nums_in_string(char str[])
{
    int sum = 0;
    char curr_num_in_string[256];
    int curr_num_i = 0;
    int i;
    long num;

    for (i = 0; i < strlen(str) - 1; ++i)
    {
        if (str[i] >= '0' && str[i] <= '9')
        {
            curr_num_in_string[curr_num_i] = str[i];
            curr_num_i += 1;
        }
        else
        {
            curr_num_in_string[curr_num_i] = '\0';
            if (kstrtol(curr_num_in_string, 10, &num) == 0)
            {
                sum += num;
            }
            curr_num_i = 0;
        }
    }

    curr_num_in_string[curr_num_i] = 0;
    if (kstrtol(curr_num_in_string, 10, &num) == 0) sum += num;

    return sum;
}


// -------------- data structure to store results --------------

typedef struct ArrayList
{
    int *data;
    size_t capacity;
    size_t length;
} ArrayList;
ArrayList res_list;

ArrayList create_list(void)
{
    ArrayList arr_list = {
        .data = vmalloc(sizeof(int)),
        .capacity = 1,
        .length = 0
    };
    return arr_list;
}

void append_list(size_t element, ArrayList *arr_list)
{
    if (arr_list->length == (arr_list->capacity - 1))
    {
        // we can't increase our list capacity further if it exceeds available RAM
        // if (arr_list->capacity >= sysconf(_SC_AVPHYS_PAGES) * sysconf(_SC_PAGESIZE) || arr_list->capacity * 2 >= SIZE_MAX) return -ENOMEM;
        arr_list->capacity = arr_list->capacity * 2; // we need to make list bigger
        int* resized_data = (int*)vmalloc(arr_list->capacity * sizeof(int));
        memcpy(resized_data, arr_list->data, arr_list->length * sizeof(int));
        vfree(arr_list->data);
        arr_list->data = resized_data;
    }
    // have some extra space, can append without resize
    arr_list->data[arr_list->length++] = element;
}

// -------------- ch_dev functions --------------

static int ch_dev_open(struct inode *i, struct file *f)
{
    printk(KERN_INFO "Driver: open()\n");
    return 0;
}

static int ch_dev_close(struct inode *i, struct file *f)
{
    printk(KERN_INFO "Driver: close()\n");
    return 0;
}

static ssize_t ch_dev_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
    int i = 0;
    for (i = 0; i < res_list.length; i++) printk(KERN_DEBUG "%d\n", res_list.data[i]);
    return 0;
}

static ssize_t ch_dev_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
    char* input = (char*) vmalloc(len * sizeof(char));
    memcpy(input, buf, len * sizeof(char));
    input[len] = 0;
    if (*input == '/') {
        proc_file_handler(input);
    }
    else {
        int sum_of_num = get_sum_of_nums_in_string(input);
        append_list(sum_of_num, &res_list);
    }
    *off = len;
    return len;
}

// -------------- proc_file functions --------------

static ssize_t proc_write(struct file *file, const char __user * ubuf, size_t count, loff_t* ppos)
{
    printk(KERN_DEBUG "Attempt to write proc file");
    return -EPERM;
}

static ssize_t proc_read(struct file *file, char __user * buf, size_t len, loff_t* off)
{
    char sum_of_num_string[256];
    int i = 0;
    int processed = 0;
    for (i = 0; i < res_list.length; i++)
    	processed += snprintf(&sum_of_num_string[processed], 256 - processed, "%d\n", res_list.data[i]);

    sum_of_num_string[processed] = 0;
    size_t count = strlen(sum_of_num_string);

    if ((len < count) || (*off > 0)) return 0;
    if (copy_to_user(buf, sum_of_num_string, count) != 0) return -EFAULT;

    *off = count;
    return count;
}

static struct file_operations mychdev_fops =
{
    .owner = THIS_MODULE,
    .open = ch_dev_open,
    .release = ch_dev_close,
    .read = ch_dev_read,
    .write = ch_dev_write
};

static struct file_operations proc_fops = {
    .owner = THIS_MODULE,
    .read = proc_read,
    .write = proc_write,
};

void proc_file_handler(char* input) {
    int i;
    for (i = 0; i < strlen(SWAP_FILE) - 1; i++) {
        if (*input != SWAP_FILE[i]) return;
        input++;
    }

    while (*input == ' ') input++;

    proc_remove(entry);

    char* temp = input;
    while (*temp != '\n') temp++;
    *temp = 0;
    entry = proc_create(input, 0444, NULL, &proc_fops);
    printk(KERN_INFO "%s: new proc file (%s) is created\n", THIS_MODULE->name, input);
}


static int __init lab_init(void)
{
    res_list = create_list();

    entry = proc_create("var3", 0444, NULL, &proc_fops);
    printk(KERN_INFO "%s: proc file is created\n", THIS_MODULE->name);

    printk(KERN_INFO "%s: initializing character device\n", THIS_MODULE->name);

    if (alloc_chrdev_region(&first, 0, 1, "ch_dev") < 0) goto destroy_proc;
    if ((cl = class_create(THIS_MODULE, "chardrv")) == NULL) goto destroy_region;
    if (device_create(cl, NULL, first, NULL, "var3") == NULL) goto destroy_class;

    cdev_init(&c_dev, &mychdev_fops);
    if (cdev_add(&c_dev, first, 1) == -1) goto destroy_device;

    printk(KERN_INFO "%s: initialization complete\n", THIS_MODULE->name);
    return 0;

    destroy_device:
        device_destroy(cl, first);
    destroy_class:
        class_destroy(cl);
    destroy_region:
        unregister_chrdev_region(first, 1);
    destroy_proc:
        proc_remove(entry);
    return -EIO;
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
