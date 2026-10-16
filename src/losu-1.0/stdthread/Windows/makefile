OBJS = stdthread.o 
CFLAGS= -O2   -Wall -std=c99  -DLOSU_WINDOWS   -I ../buildenv -fPIC 
CC=x86_64-w64-mingw32-gcc
T=ElsLib_stdthread.lsd


all: $T 

$T:	$(OBJS)
	$(CC) -o $@ $(OBJS)  -shared -lpthread -s -L ../buildenv -l:libeasylosu.dll -static

clean:
	rm -rf $T $(OBJS)  core core.* 