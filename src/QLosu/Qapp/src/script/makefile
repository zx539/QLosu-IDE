main=src

cc=clang
optim=-O2
include=-I losu/modules/losu/include -I $(main) -I ../../ -I ../../build
debugflag=-DNDEBUG
ldflag=-static -lm -lstdc++

# --target=x86_64-linux-musl
cflag = -std=c99 --target=x86_64-linux-musl  -Wall -Wextra\
	$(include) $(optim) $(debugflag)
src = $(shell find losu/modules $(main) -name "*.c")
obj = $(src:.c=.o)

build: $(obj)
	$(cc) $(cflag) $(obj) -o QLosu -L ../../build -l:libQuEST.a $(ldflag)

clean:
	rm -f $(obj)
	rm -f QLosu

%.o: %.c
	$(cc) $(cflag) -c $< -o $@
