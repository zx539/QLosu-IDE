CFLAGS = -O2 -std=c99  -DELS_CONF_OS_WINDOWS   -I ../buildenv -L ../buildenv -fPIC -s -l:libeasylosu.dll
CPPFLAGS = -O2 -std=c++11 -DELS_CONF_OS_WINDOWS -I ../buildenv -L ../buildenv -l:libeasylosu.dll

re:
	gcc cre.c -c $(CFLAGS)
	g++ re.cpp -c $(CPPFLAGS)
	gcc -o ElsLib_re.lsd *.o -shared $(CFLAGS) -lstdc++

clean:
	rm ./*.o
