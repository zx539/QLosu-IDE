# 为 AMD64 处理器的Linux 系统编译可使用的动态库

DATAOBJ= els_func.o els_string.o els_unit.o els_object.o 
COMPILER= els_codegen.o els_lex.o  els_parser.o  
VMAPI= els_api.o  
VMCORE=  els_mem.o els_vmcore.o els_vmhost.o els_heap.o els_gc.o
LIBS= els_libload.o
OBJS= $(DATAOBJ) $(COMPILER) $(VMAPI) $(VMCORE) $(LIBS) els.o 


CFLAGS= -O2  -Wall -std=gnu99  -DELS_CONF_OS_LINUX   -fPIC 
CC=x86_64-linux-gnu-gcc
T=libeasylosu.so


all: $T 

$T:	$(OBJS)
	$(CC) -o $@ $(OBJS)  -shared -lm -ldl -s 
clean:
	rm  -f $T $(OBJS) 


