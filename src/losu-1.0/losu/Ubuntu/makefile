# DATAOBJ= els_func.o els_string.o els_unit.o els_object.o 
# COMPILER= els_codegen.o els_lex.o  els_parser.o  
# VMAPI= els_api.o  
# VMCORE=  els_mem.o els_vmcore.o els_vmhost.o els_heap.o els_gc.o
# LIBS= els_libload.o
# OBJS= $(DATAOBJ) $(COMPILER) $(VMAPI) $(VMCORE) $(LIBS) init.o
OBJS = losu.o

CFLAGS= -O2   -Wall -std=c99  -DLOSU_LINUX   -I ../buildenv -fPIC 
CC=x86_64-linux-gnu-gcc
T=./losu


all: $T 

$T:	$(OBJS)
	$(CC) -o $@ $(OBJS)  -lm -ldl -L ../buildenv -leasylosu -s
#-rdynamic
clean:
	rm -rf $T $(OBJS)  core core.* 

