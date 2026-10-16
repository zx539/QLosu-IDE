OBJS = losu.o 

CFLAGS= -O2   -Wall -std=c99  -DLOSU_WINDOWS -I ../buildenv -fPIC -static
CC=x86_64-w64-mingw32-gcc
T=losu.exe


all: $T 

$T:	$(OBJS)
	$(CC)  -o $@ $(OBJS) logo.o -L ../buildenv -l:libeasylosu.dll -lm -static -s
#-rdynamic
clean:
	rm -rf $T $(OBJS)  core core.* 
