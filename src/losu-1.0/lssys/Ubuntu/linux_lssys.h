#ifndef LOSU_LINUX_LSSYS_H__
#define LOSU_LINUX_LSSYS_H__

#ifndef LOSU_LINUX
#warning "该文件用于Linux系统，如需使用，请包含LOSU_LINUX宏"
#endif

/*
    popen等函数基于GNU拓展，但洛书标准库规定基于c99标准
    如果在第一次包含<stdio.h>之前没有定义宏_GNU_SOURCE的话，编译器会报warning
    虽然最后都可以正常链接
    这里是为了消除warning，避免不知道的开发者误会
*/
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#include <stdio.h>
#undef _GNU_SOURCE
#else
#include <stdio.h>
#endif

#include <sys/unistd.h> //access
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <signal.h>
#include <pthread.h>

#include "els.h"

#ifdef LOSU_LINUX

//Windows操作系统的规范化分隔符是反斜杠“\”
#ifndef LOSU_LINUX_LSSYS_PATH_ALTSEP 
#define LOSU_LINUX_LSSYS_PATH_ALTSEP "/"
#endif

#define LSSYS_ARCH_UNKNOW -1
#define LSSYS_ARCH_ARM 1
#define LSSYS_ARCH_X86  4


//stdsys已经实现的
const char *Lssys_getos();
const char *Lssys_getenv(const char *, char *, int);
int Lssys_system(const char *);
void *Lssys_popen(const char *, const char *);
int Lssys_pwrite(void *, const char *);
int Lssys_pread(void *, int, char *);
int Lssys_pclose(void *);

//lssys要实现的
bool Lssys_getcwd(char *, int);
bool Lssys_exsists(const char *);
bool Lssys_mkdir(const char*);
bool Lssys_rmdir(const char*);
bool Lssys_rename(const char *, const char *);
bool Lssys_remove(const char*);
uint64_t Lssys_getpid();
bool Lssys_join(const char *, const char *, char *, int);
bool Lssys_chdir(const char*);
bool Lssys_killp(uint64_t);
uint64_t Lssys_lpcount();
int Lssys_architecture();


uint64_t Lssys_memtotal();
uint64_t Lssys_memfreep();
#endif

#endif