#ifndef __xkit_cxkit_h__
#define __xkit_cxkit_h__

#include <stdint.h>
#include <stdbool.h>

//包含els.h
#ifdef __cplusplus
extern "C" {
    #include "els.h"
}
#else
#include "els.h"
#endif

//用于unit内函数的处理宏
#define unit_arg(x) x+1
#define unit_argtype(vm, x) arg_gettype(vm, x+1)
#define unit_argisnum(vm, x) arg_gettype(vm, x+1) == ELS_API_TYPE_NUMBER
#define unit_argisstr(vm, x) arg_gettype(vm, x+1) == ELS_API_TYPE_STRING
#define unit_argisunit(vm, x) arg_gettype(vm, x+1) == ELS_API_TYPE_UNIT
#define unit_argisptr(vm, x) arg_gettype(vm, x+1) == ELS_API_TYPE_ptr
#define unit_argisbyte(vm, x) arg_gettype(vm, x+1) == ELS_API_TYPE_byte
#define unit_argisfunc(vm, x) arg_gettype(vm, x+1) == ELS_API_TYPE_function

//unit操作宏
#define unit_getval(vm, u, s) obj_indexunitbystr(vm, u, (char*)s)
#define unit_addfunc(vm, u, s, f) obj_setunitbystr(vm, u, s, obj_newfunction(vm, (els_C_API_function)f))

//API接口标识
#ifdef __cplusplus
#define XKIT_API extern "C"
#else
#define XKIT_API extern
#endif

//LosuObj的别称
typedef LosuObj LsUnit;
typedef LosuObj LsNumber;
typedef LosuObj LsString;
typedef LosuObj LsByte;
typedef LosuObj LsPoiter;
typedef LosuObj LsFunction;

//返回值的bool处理
#define arg_returntrue(vm) arg_return(vm, obj_newnum(vm, 1))
#define arg_returnfalse(vm) arg_returnnull(vm)

//关于gbk和utf8编码的输入输出宏
#ifdef _WIN32
#define outstr(vm, x) (char*)obj_toGBK(vm, (char*)x)
#define instr(vm, x) (char*)obj_toUTF8(vm, (char*)x)
#endif
#ifdef __linux__
#define outstr(vm, x) (char*)x
#define instr(vm, x) (char*)x
#endif

//操作系统宏开关
#ifdef __linux__
#ifndef ELS_CONF_OS_LINUX
#define ELS_CONF_OS_LINUX
#endif
#endif
#ifdef _WIN32
#ifndef ELS_CONF_OS_WINDOWS
#define ELS_CONF_OS_WINDOWS
#endif
#endif

#endif
