OBJS = random.o crandom.o lc.o mt.o randtext.o ELS_random.o
CFLAGS= -O3 -std=c99   -Wall  -DLOSU_WINDOWS   -I ../buildenv -fPIC 
CC=x86_64-w64-mingw32-gcc
T=ElsLib_random.lsd

all: $T 

$T:	$(OBJS)
	$(CC) -o $@ $(OBJS)  -L ../buildenv -l:libeasylosu.dll  -lm  -static -shared -s 

clean:
	rm -rf $T $(OBJS)  core core.* 