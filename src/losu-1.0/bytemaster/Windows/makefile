T = ElsLib_bytemaster.lsd
CFLAGS = -O2 -std=c99  -DELS_CONF_OS_WINDOWS   -I ../buildenv -L ../buildenv -fPIC -s -l:libeasylosu.dll
CPPFLAGS = -O3 -std=c++11 -DELS_CONF_OS_WINDOWS -I ./include -I ../buildenv

all: $T

$T:
	gcc bytemaster.c -o $(T) -shared $(CFLAGS)

clean:
	rm *.o
