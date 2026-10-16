OBJS = stdbyte.o 
CFLAGS= -O2   -Wall -std=c99  -DELS_CONF_OS_WINDOWS  -I ../buildenv -fPIC 
CC=x86_64-w64-mingw32-gcc
T=./ElsLib_stdbyte.lsd


all: $T 

$T:	$(OBJS)
	$(CC) -o $@ $(OBJS)  -L ../buildenv -l:libeasylosu.dll   -static -shared -s

clean:
	rm -rf $T $(OBJS)  core core.* 