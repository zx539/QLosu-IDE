OBJS = lsmath.o 
CFLAGS= -O2   -Wall -std=c99  -DLOSU_WINDOWS   -I ../buildenv -fPIC 
CC=x86_64-w64-mingw32-gcc
T=ElsLib_lsmath.lsd


all: $T 

$T:	$(OBJS)
	$(CC) -o $@ $(OBJS)  -L ../buildenv -l:libeasylosu.dll -lm  -static -shared -s

clean:
	rm -rf lsmath.o