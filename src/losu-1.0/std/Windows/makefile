OBJS = std.o 
CFLAGS= -O2   -Wall -std=c99  -DLOSU_WINDOWS   -I ../buildenv -fPIC 
CC=x86_64-w64-mingw32-gcc
T=ElsLib_std.lsd


all: $T 

$T:	$(OBJS)
	$(CC) -o $@ $(OBJS)  -L ../buildenv -l:libeasylosu.dll   -static -shared -s
#-lUrlmon -lUser32 -lkernel32 -lWininet
clean:
	rm -rf $T $(OBJS)  core core.* 