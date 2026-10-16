OBJS = socket.o win_sock.o
CFLAGS= -O2 -std=c99   -Wall  -DLOSU_WINDOWS   -I ../buildenv -fPIC 
CC=x86_64-w64-mingw32-gcc
T=ElsLib_socket.lsd


all: $T 

$T:	$(OBJS)
	$(CC) -o $@ $(OBJS)  -L ../buildenv -l:libeasylosu.dll  -lws2_32 -lm  -static -shared -s

clean:
	rm -rf $T $(OBJS)  core core.* 