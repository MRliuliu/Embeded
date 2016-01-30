DEBUG = n

ifeq ($(DEBUG),y)
	EXTRA_CFLAGS += -DHELLO_DEBUG
endif

obj-m	+= hello.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
