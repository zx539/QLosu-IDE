OBJS = hash.o MD5.o SHA256.o SHA384.o SHA512.o
CFLAGS= -O3  -std=c99 -Wall  -DLOSU_LINUX   -I ../buildenv -fPIC 
CC=x86_64-linux-gnu-gcc
T=./ElsLib_hash.lsd


all: $T 

$T:	$(OBJS)
	$(CC) -o $@ $(OBJS)  -shared -lm -s

clean:
	rm -rf $T $(OBJS)  core core.* 