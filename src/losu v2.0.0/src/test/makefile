# main?=$(dir $(lastword $(MAKEFILE_LIST)))helloworld

cc?=clang
ar?=ar
optim?=-O0
debugflag?=-g
ldflag?=-lm
lib?=liblosu-v2.a


cflag = -std=c99\
	-I "$(CURDIR)/../" -I "$(CURDIR)"\
	-Wall -Wextra $(optim)\
	$(debugflag)\

src = $(shell find ../losu -name "*.c")
obj?= $(src:.c=.o)



all: $(lib)

# The v2 tree currently has no VM bootstrap/main entry point. Keep the
# native build honest by producing a static archive of all compiled modules;
# the repository CLI supplies execution through the bundled WASM runtime.
$(lib): $(obj)
	$(ar) rcs $@ $(obj)

objects: $(obj)

check: objects

cli:
	@printf '%s\n' 'Use: node scripts/losu2-cli.js --help'

clean:
	rm -f $(obj)
	rm -f $(lib)

%.o: %.c
	$(cc) -c $(cflag)  $< -o $@
