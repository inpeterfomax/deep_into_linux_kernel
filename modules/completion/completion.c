/*
* @date: 2010-8-6
* @describion: completion信号量机制的简单实例
* @filename: completion_test.c
***************************************************/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/completion.h>

struct cdev *com_cdev;
static int com_major = 0;
static int com_minor = 0;
module_param(com_major, int, S_IRUGO);
module_param(com_minor, int, S_IRUGO);
//声明一个completion,为静态声明，可以动态声明，但还需要初始化
DECLARE_COMPLETION(comp);

static ssize_t completion_read (struct file *filp, char __user *buf, size_t count, loff_t *pos){
	printk(KERN_DEBUG"process %i (%s) going to sleep\n", current->pid, current->comm);
	//等待completion，如果没有人会完成该任务，则将产生一个不可杀的进程
	wait_for_completion(&comp);
	printk(KERN_DEBUG"awoken %i (%s)\n", current->pid, current->comm);
	return 0;
}
static ssize_t completion_write (struct file *filp, const char __user *buf, size_t count, loff_t *pos){
	printk(KERN_DEBUG "process %i (%s) awakening the readers.....\n", current->pid, current->comm);
	//唤醒一个等待线程
	complete(&comp);
	return count;
}

struct file_operations com_fops = {
	.owner = THIS_MODULE,
	.read = completion_read,
	.write = completion_write,
};

void completion_exit(void){
	dev_t devno = MKDEV(com_major, com_minor);
		if (com_cdev){
			cdev_del(com_cdev);
			kfree(com_cdev);
		}
	unregister_chrdev_region(devno, 1);
}

int completion_init(void){
	int result, err;
	dev_t dev = 0;
	if(com_major){
		dev = MKDEV(com_major, com_minor);
		result = register_chrdev_region(dev, 1, "completion");
	}else{
		result = alloc_chrdev_region(&dev, com_minor, 1, "completion");
		com_major = MAJOR(dev);
	}
	if(result < 0){
		printk(KERN_WARNING"completion:can't get major %d \n", com_major);
		return result;
	}
	com_cdev = kmalloc(sizeof(struct cdev), GFP_KERNEL);
	if(!com_cdev){
		result = -ENOMEM;
		goto fail;
	}
	cdev_init(com_cdev, &com_fops);
	com_cdev->owner = THIS_MODULE;
	err = cdev_add(com_cdev, dev, 1);
	if(err)
		printk(KERN_NOTICE"Error %d adding com_cdev", err);
	return 0;
fail:
	completion_exit();
	
	return result;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Infomax.Petet");
module_init(completion_init);
module_exit(completion_exit);
