obj-m := bsc-slave.o

ifndef kernelver
		kernelver=$(shell uname -r)
	endif
KDIR := /lib/modules/$(kernelver)/build

PWD := $(shell pwd)

.PHONY: all clean config cleanjunk conf

all: clean config
	$(MAKE) -C $(KDIR) M=$(PWD) modules

config: get_peripheral_base
	cat config.h.in |sed 's/__BCM2708_PERI_BASE__/'$(shell ./get_peripheral_base)'/' |tee config.h

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

cleanjunk:
	#rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions Module.* modules.order .cache.mk *.o.ur-safe *.mod

conf:  config
	@echo kernelver: $(kernelver)
	@echo kdir: $(KDIR)
	@echo obj-m: $(obj-m)
	@./get_peripheral_base

get_peripheral_base:
	@gcc -I/opt/vc/include -L/opt/vc/lib -lbcm_host -o get_peripheral_base get_peripheral_base.c

