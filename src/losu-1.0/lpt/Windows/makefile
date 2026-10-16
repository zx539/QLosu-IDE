OBJS = lpt.o 
CFLAGS= -O2   -Wall -std=c99  -DLOSU_WINDOWS   -I ../buildenv -fPIC 
CC=x86_64-w64-mingw32-gcc
T=ElsLib_lpt.lsd


all: $T 

$T:	$(OBJS)
	$(CC) -o $@ $(OBJS) -L ../buildenv -l:libeasylosu.dll -lurlmon -luser32 -lkernel32 -lwininet  -static -shared -s

clean:
	rm -f $T $(OBJS)  core core.* 