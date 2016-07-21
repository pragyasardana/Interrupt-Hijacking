K_VER = $(shell uname -r)

obj-m += hooker.o

all : hw8

hw8 :
	$(MAKE) -C /lib/modules/$(K_VER)/build M=$(PWD) modules

clean :
	$(MAKE) -C /lib/modules/$(K_VER)/build M=$(PWD) clean


