OBJS = lssys.o linux_lssys.o
CFLAGS= -O2  -std=c99 -Wall  -DLOSU_LINUX   -I ../buildenv -fPIC  -lc
CC=x86_64-linux-gnu-gcc
T=./ElsLib_lssys.lsd


all: $T 

$T:	$(OBJS)
	$(CC) -o $@ $(OBJS)  -shared -lm -s -lc

clean:
	rm -rf $T $(OBJS)  core core.* 