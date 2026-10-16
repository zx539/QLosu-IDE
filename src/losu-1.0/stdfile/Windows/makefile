OBJS = stdfile.o 
CFLAGS= -O2   -Wall -std=c99  -DLOSU_WINDOWS   -I ../buildenv -fPIC 
CC=x86_64-w64-mingw32-gcc
T=ElsLib_stdfile.lsd

all: $T 

$T:	$(OBJS)
	$(CC) -o $@ $(OBJS) -L ../buildenv -l:libeasylosu.dll   -shared -s -static
#-lUrlmon -lUser32 -lkernel32 -lWininet 
clean:
	rm -rf $T $(OBJS)  core core.*
