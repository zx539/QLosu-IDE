OBJS = random.o randtext.o lc.o mt.o crandom.o ELS_random.o
CFLAGS= -O3  -std=c99 -Wall  -DLOSU_LINUX   -I ../buildenv -fPIC 
CC=x86_64-linux-gnu-gcc
T=./ElsLib_random.lsd


all: $T 

$T:	$(OBJS)
	$(CC) -o $@ $(OBJS)  -shared -lm -s -lstdc++

clean:
	rm -rf $T $(OBJS)  core core.* 