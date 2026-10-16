OBJS = socket.o linux_sock.o
CFLAGS= -O2  -std=c99 -Wall  -DLOSU_LINUX   -I ../buildenv -fPIC 
CC=x86_64-linux-gnu-gcc
T=./ElsLib_socket.lsd


all: $T 

$T:	$(OBJS)
	$(CC) -o $@ $(OBJS)  -shared -lm -s

clean:
	rm -rf $T $(OBJS)  core core.* 