
CROSS_COMPILE = 
AS		= $(CROSS_COMPILE)as
LD		= $(CROSS_COMPILE)ld
CC		= $(CROSS_COMPILE)gcc
CPP		= $(CC) -E
AR		= $(CROSS_COMPILE)ar
NM		= $(CROSS_COMPILE)nm

STRIP		= $(CROSS_COMPILE)strip
OBJCOPY		= $(CROSS_COMPILE)objcopy
OBJDUMP		= $(CROSS_COMPILE)objdump

export AS LD CC CPP AR NM
export STRIP OBJCOPY OBJDUMP

CFLAGS := -Wall -O2 -g
CFLAGS += -I $(shell pwd)/applications/include \
		  -I $(shell pwd)/package/mymqtt/common/log \
		  -I $(shell pwd)/package/mymqtt/common \
		  -I $(shell pwd)/package/mymqtt/platform/linux \
		  -I $(shell pwd)/package/mymqtt/network/mbedtls/include \
		  -I $(shell pwd)/package/mymqtt/network/mbedtls/wrapper \
		  -I $(shell pwd)/package/mymqtt/network/mbedtls/include/mbedtls \
		  -I $(shell pwd)/package/mymqtt/mqtt \
		  -I $(shell pwd)/package/mymqtt/network 

LDFLAGS := -lpthread

export CFLAGS LDFLAGS

TOPDIR := $(shell pwd)
export TOPDIR

TARGET := MQTT


obj-y += applications/
obj-y += package/mymqtt/


all : start_recursive_build $(TARGET)
	@echo $(TARGET) has been built!

start_recursive_build:
	make -C ./ -f $(TOPDIR)/Makefile.build

$(TARGET) : built-in.o
	$(CC) -o $(TARGET) built-in.o $(LDFLAGS)

clean:
	rm -f $(shell find -name "*.o")
	rm -f $(TARGET)

distclean:
	rm -f $(shell find -name "*.o")
	rm -f $(shell find -name "*.d")
	rm -f $(TARGET)
	