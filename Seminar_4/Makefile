obj-m += test_mod01.o

KDIR=/usr/src/linux-headers-5.15.0-52-generic

all:
	$(MAKE) -C $(KDIR) M=$(shell pwd) modules

clean:
	$(MAKE) -C $(KDIR) M=$(shell pwd) clean
	rm -f *.o
