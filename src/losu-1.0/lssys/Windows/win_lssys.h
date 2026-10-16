#ifndef LOSU_WIN_LSSYS_H__
#define LOSU_WIN_LSSYS_H__

#ifndef LOSU_WINDOWS
#warning "注意,这是用于Windows平台的代码!!!如需包含代码,请定义宏LOSU_WINDOWS"
#endif

#ifdef LOSU_WINDOWS

#include "els.h"

#include <windows.h>
#include <direct.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

//Windows操作系统的规范化分隔符是反斜杠“\”
#ifndef LOSU_WIN_LSSYS_PATH_ALTSEP 
#define LOSU_WIN_LSSYS_PATH_ALTSEP "\\"
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