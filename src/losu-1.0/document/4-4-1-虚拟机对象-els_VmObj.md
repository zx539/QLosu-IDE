#!/bin/owls.wiki.losu.tech

# 4-4-1-虚拟机对象-els_VmObj
## 4-4-1-1-概述
1. 对象API是以 vm_ 为前缀的一系列函数。
2. 对象API提供了在 C 中创建、管理、销毁虚拟机的一系列操作。
## 4-4-1-2-数据类型
```C++
struct els_VmObj
{
    // 
};
typedef struct els_VmObj els_VmObj;
```
+ 一般而言，其以指针的形式被使用 `els_VmObj*`，它指向的是一个存放在内存中的虚拟机对象
## 4-4-1-3-对象API
### 4-4-1-3-1-创建虚拟机对象
+ 原型
```C++
ELS_API els_VmObj*      vm_create(int stacksize);
```
+ 功能：
    1. 在内存中申请一块空余地址，创建一个运行时栈大小为 stacksize 的虚拟机
    2. 如果创建失败，返回 NULL，否则返回虚拟机对象的指针

### 4-4-1-3-2-fork虚拟机

> 此API在多线程与协程相关章节会详细讲解

+ `ELS_API els_VmObj*      vm_fork(els_VmObj*vm,int size);`
+ 功能：并将vm指向的虚拟机分支到新的空间，并设置运行时栈为 size

### 4-4-1-3-3-打断运行
+ `ELS_API void            vm_stop(els_VmObj*vm);`
+ 打断指定虚拟机的运行，即使是在死循环中
+ 这不会销毁虚拟机对象，该对象仍可被重新传入执行指令

### 4-4-1-3-4-报错
+ `ELS_API void            vm_error(els_VmObj* vm,const char* einfo,int e );`
+ 打断运行，输出错误信息 einfo，并将虚拟机的返回值设为 e
+ 这不会销毁虚拟机对象，该对象仍可被重新传入执行指令

### 4-4-1-3-5-执行脚本
+ 用 vm 虚拟机执行 f 文件
+ `ELS_API int             vm_dofile(els_VmObj* vm,const char* f);`

+ 用 vm 虚拟机执行字符串 s
+ `ELS_API int             vm_dostring(els_VmObj*vm,const char* s );`


### 4-4-1-3-6-获取设定全局变量
> 关于 LosuObj 类型，请参考相关章节

+ 获取 vm 中变量名为 s 的全局变量，返回它对应的 LosuObj 的指针
+ `ELS_API LosuObj*        vm_getval(els_VmObj* vm,const char*s);`

+ 设定 vm 中变量 s 的值为 o
+ `ELS_API void            vm_setval(els_VmObj *vm,const char* s ,LosuObj  o );`

+ 在 vm 中注册 C 函数 cfunc 为脚本函数 s
+ `ELS_API void            vm_register(els_VmObj* vm,const char* s,VmCapi cfunc);`

### 4-4-1-3-7-垃圾回收
> 有关垃圾回收器的相关内容，可以参考内核原理的相关章节

+ 设定 vm 的最大 GC 上阈值为 max
+ `ELS_API void            vm_setGC(els_VmObj* vm,unsigned long max);`

+ 获取 vm 占用资源的相对值(当前资源 - 创建时资源)
+ `ELS_API unsigned long   vm_getmem(els_VmObj*vm);`

### 4-4-1-3-8-销毁虚拟机
+ 打断虚拟机的一切活动，强行回收虚拟机的相关资源
+ `ELS_API void            vm_close(els_VmObj*vm);`