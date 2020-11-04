obj-m := bsc-slave.o

ifndef kernelver
		kernelver=$(shell uname -r)
	endif
KDIR := /lib/modules/$(kernelver)/build

PWD := $(shell pwd)

all: clean
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

cleanjunk:
	#rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions Module.* modules.order .cache.mk *.o.ur-safe *.mod

conf:
	@echo kernelver: $(kernelver)
	@echo kdir: $(KDIR)
	@echo obj-m: $(obj-m)

