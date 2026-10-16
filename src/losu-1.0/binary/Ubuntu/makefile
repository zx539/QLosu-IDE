OBJS = binary.o losubinary.o
CFLAGS= -O3  -std=c99 -Wall  -DLOSU_LINUX   -I ../buildenv -fPIC 
CC=x86_64-linux-gnu-gcc
T=./ElsLib_binary.lsd


all: $T 

$T:	$(OBJS)
	$(CC) -o $@ $(OBJS)  -shared -lm -s -lc

clean:
	rm -rf $T $(OBJS)  core core.* 