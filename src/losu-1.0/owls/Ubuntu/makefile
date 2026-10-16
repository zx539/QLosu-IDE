OBJS = owls.o 
CFLAGS= -O2   -Wall -std=c99  -DLOSU_LINUX   -I ../buildenv -fPIC 
CC=x86_64-linux-gnu-gcc
T=./ElsLib_owls.lsd


all: $T 

$T:	$(OBJS)
	$(CC) -o $@ $(OBJS)  -shared  -s

clean:
	rm -rf $T $(OBJS)  core core.* 