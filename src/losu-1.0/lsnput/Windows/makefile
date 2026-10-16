OBJ = ElsLib_lsnput.lsd
BASE = Els_lsnput.o lsnput.o
CFLAGE = -O3 -std=c99   -Wall  -DLOSU_WINDOWS  -I ../buildenv -fPIC   -L ../buildenv -l:libeasylosu.dll -lUrlmon -lUser32 -lkernel32 -lWininet -lm  -static -shared -s
CC = gcc

OBJ:
	$(CC) ./*.o -c $(CFLAGE)
	$(CC) -o $(OBJ) $(BASE) $(CFLAGE)

clean:
	rm -rf $T ./*.o ./*.lsd
