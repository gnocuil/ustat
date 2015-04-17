CC     := gcc
#CC     := /home/gnocuil/ar71xx/OpenWrt-Toolchain-ar71xx-for-mips_34kc-gcc-4.8-linaro_uClibc-0.9.33.2/toolchain-mips_34kc_gcc-4.8-linaro_uClibc-0.9.33.2/bin/mips-openwrt-linux-uclibc-gcc
CFLAGS := -O2 -lpthread -ldl
TARGET := ustat
OBJS   := main.o eth.o data.o

all: $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(CFLAGS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)
	
clean :
	rm -f $(TARGET)
	rm -f *.o
