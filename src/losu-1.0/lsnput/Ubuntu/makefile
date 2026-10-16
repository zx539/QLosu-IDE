OBJ = ElsLib_lsnput.lsd
CFLAGS= -O3  -std=c99 -Wall  -DLOSU_LINUX   -I ../buildenv -fPIC 
CC=gcc
T=./ElsLib_lsnput.lsd


all: $T 

$T:
	gcc ./*.c -c $(CFLAGS)
	$(CC) -o $(OBJ) ./*.o -shared -lm -s -lc -O3

clean:
	rm -rf $T ./*.o ./*.lsd