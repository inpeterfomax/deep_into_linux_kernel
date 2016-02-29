ifneq($(KERNELRELEASE),) 
obj-m=completion.o
#completion-objs:=completion
else
	KERNELDIR ?= /lib/modules/$(shell umane -r)/build
	PWD := $(shell pwd)
default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif

clean:
	rm -rf *.ko *.o *.tmp* *.swp*
