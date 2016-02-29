struct miscdevice *

static int init_func(void){
	misc_register(my_misc);
}
static void clean_func(void){
	misc_unregister(my_misc);
}

module_init( init_func);
module_exit( clean_func);

MODULE_AUTHOR("infomax.com.tw");
