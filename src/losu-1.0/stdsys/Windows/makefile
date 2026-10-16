OBJS = stdsys.o 
CFLAGS= -O2   -Wall -std=c99  -DELS_CONF_OS_WINDOWS   -I ../buildenv -fPIC 
CC=x86_64-w64-mingw32-gcc
T=./ElsLib_stdsys.lsd


all: $T 

$T:	$(OBJS)
	$(CC) -o $@ $(OBJS)  -shared -s -L ../buildenv -l:libeasylosu.dll -static

clean:
	rm -rf $T $(OBJS)  core core.* 