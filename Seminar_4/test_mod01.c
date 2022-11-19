#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kern_levels.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");

#define ECE_BUF_SIZE 4096

static char ece_buffer[ECE_BUF_SIZE];
int isReg;
int major;
int ece_offset_w;
int ece_offset_r;
int ece_size;

static ssize_t ece_write(struct file *fp, const char *buf, size_t count, loff_t *op)
{
    int ret = 0;
    if(ece_offset_w + count >= ECE_BUF_SIZE)
    {
        printk(KERN_INFO " ECE4310: Write OverFlow. Abort. \n");
        return -1;
    }

    printk(KERN_INFO " ECE4310: Copy from user. \n");
    ret = copy_from_user(&ece_buffer[ece_offset_w], buf, count);
    if(ret != 0)
    {
        printk(KERN_INFO " ECE4310: ERR copy from user. \n");
        return -1;
    }
    ece_offset_w = ece_offset_w + count;

    return count;
}

static ssize_t ece_read(struct file *fp,
    char *buf,
    size_t count,
    loff_t *offset)
{
    int ret;
    if(ece_offset_r + count >= ECE_BUF_SIZE)
    {
        printk(KERN_INFO " ECE4310: Read OverFlow. Abort. \n");
        return -1;
    }
    printk(KERN_INFO " ECE4310: Copy to user. \n");
    ret = copy_to_user(buf, &ece_buffer[ece_offset_r], count);
    if(ret != 0)
    {
        printk(KERN_INFO " ECE4310: ERR copy to user. \n");
        return -1;
    }

    ece_offset_r = ece_offset_r + count;

    return count;
}

static struct file_operations ece_fops = {
    .read = ece_read,
    .write = ece_write,
};

int ece_init(void)
{
    int ret = 0;
    major = register_chrdev(0, "test_mod01", &ece_fops);
    ece_offset_w = 0;
    ece_offset_r = 0;
    ece_size = 0;

    if(major < 0)
    {
        isReg = 0;
        printk(KERN_INFO " ECE4310: Start FAIL \n");
    }
    else
    {
        isReg = 1;
        printk(KERN_INFO " ECE4310: Start here \n");
    }
    return 0;
}

void ece_end(void)
{
    if(isReg)
    {
        unregister_chrdev(major, "test_mod01");
    }
    printk(KERN_INFO " ECE4310: End here \n");
}

module_init(ece_init);
module_exit(ece_end);
