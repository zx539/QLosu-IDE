OBJS = stdmath.o 
CFLAGS= -O2   -Wall -std=gnu99  -DELS_CONF_OS_LINUX  -I ../buildenv -fPIC 
CC=x86_64-linux-gnu-gcc
T=./ElsLib_stdmath.lsd


all: $T 

$T:	$(OBJS)
	$(CC) -o $@ $(OBJS)  -shared -lm -s

clean:
	rm -rf $T $(OBJS)  core core.* 