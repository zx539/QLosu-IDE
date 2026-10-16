BASE = base.o
MAIN = base64.o
OBJS = $(BASE) $(MAIN)
CFLAGS= -O3 -std=c99   -Wall  -DLOSU_WINDOWS   -I ../buildenv -fPIC 
CC=x86_64-w64-mingw32-gcc
T=ElsLib_base64.lsd


all: $T 

$T:	$(OBJS)
	$(CC) -o $@ $(OBJS)  -lm  -static -shared -s  -L ../buildenv -l:libeasylosu.dll

clean:
	rm -rf $T
	rm -rf $(BASE)
	rm -rf $(MAIN)