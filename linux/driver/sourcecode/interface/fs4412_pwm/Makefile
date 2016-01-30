ifeq ($(KERNELRELEASE),)

KERNELDIR ?= /home/linux/linux-3.14-fs4412/

PWD := $(shell pwd)

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	cp *.ko /source/rootfs

modules_install:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules_install

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions Module* module* test

.PHONY: modules modules_install clean

else
    obj-m := fs4412_pwm.o
endif

