OBJS = stdlib.o 
CFLAGS= -O2   -Wall -std=c99  -DELS_CONF_OS_LINUX   -I ../buildenv -fPIC 
CC=x86_64-linux-gnu-gcc
T=./ElsLib_stdlib.lsd


all: $T 

$T:	$(OBJS)
	$(CC) -o $@ $(OBJS)  -shared -s

clean:
	rm -rf $T $(OBJS)  core core.* 