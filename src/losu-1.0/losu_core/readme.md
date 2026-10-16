#!/bin/owls.wiki.losu.tech
# 洛书内核
## 标准配置宏
``` C++
// 文件 els.h

#ifdef LOSU_WINDOWS
    #ifndef ELS_CONF_OS_WINDOWS
        #define ELS_CONF_OS_WINDOWS
    #endif
#endif

#ifdef LOSU_LINUX
    #ifndef ELS_CONF_OS_LINUX
        #define ELS_CONF_OS_LINUX
    #endif
#endif


#ifdef ELS_CONF_OS_WINDOWS  // Windows 平台配置宏
    #ifndef LOSU_WINDOWS    // 兼容接口
        #define LOSU_WINDOWS             1
    #endif
    #define ELS_CONF_TOKEN_CN        1      // 开启中文关键词与模块支持,删去后模块中文接口将随之关闭
    #define ELS_CONF_TOKEN_EN        1      // 开启英文关键词与模块支持
    #define ELS_CONF_LIBAPI_WIN32    1      // 将 libapi 的实现模式设定为 Win32
    // #define ELS_CONF_LIBAPI_DL       1   // 不开启 dl 库实现(Linux平台开启)
    #define ELS_CONF_LIBAPI_ELF      1      // 兼容 elf 模式的 libapi (即被编译到内核中的内置模块)
    #define ELS_CONF_CHAR_GBK        1      // 开启 GBK 兼容层
    #define ELS_CONF_GC_ENABLE       1      // 开启 GC
    #define ELS_VM_STACK_SIZE        1024   // 虚拟机默认栈大小 (1KB 虚拟内存)
    #define ELS_BUFF_TMP_SIZE        1024*1024  // 字符串缓冲区大小 ( 1M )
    #define ELS_VMIO_SIZE		     64         // 虚拟机 IO 缓冲区 (64B)
    #define ELS_HEAPMEM_REALLOC      realloc    // 内存管理函数
    #define ELS_HEAPMEM_FREE         free
#endif

```

## 虚拟机 API 接口

``` C++
ELS_API void els_lib_init(els_VmObj* );     //  初始化内核，加载 import 函数

ELS_API int ElsNewApp(int (*func)(els_VmObj*,const char* s),char *s,int stacksize,size_t gcmax,int argc,const char** argv);

ELS_API els_VmObj*      vm_create(int);
ELS_API els_VmObj*      vm_fork(els_VmObj*vm,int size);
ELS_API void            vm_stop(els_VmObj*);
ELS_API void            vm_error(els_VmObj*,const char* ,int );
ELS_API int             vm_dofile(els_VmObj*,const char* );
ELS_API int             vm_dostring(els_VmObj*,const char* );
ELS_API LosuObj*        vm_getval(els_VmObj*,const char* );
ELS_API void            vm_setval(els_VmObj *,const char* ,LosuObj* );
ELS_API void            vm_register(els_VmObj*,const char* ,VmCapi );
ELS_API void            vm_close(els_VmObj*);
ELS_API void            vm_setGC(els_VmObj*,unsigned long );
ELS_API els_VmObj*      vm_newthread(els_VmObj*);
ELS_API void            vm_endthread(els_VmObj*);
ELS_API unsigned long   vm_getmem(els_VmObj*);

ELS_API LosuObj*        arg_get(els_VmObj*,int );
ELS_API void            arg_return(els_VmObj *,LosuObj );
ELS_API int             arg_num(els_VmObj*);

ELS_API Number          arg_getnum(els_VmObj*,int );
ELS_API const char*     arg_getstr(els_VmObj*,int );
ELS_API const char*     arg_getbyte(els_VmObj*,int);
ELS_API char*           arg_getptr(els_VmObj*,int );
ELS_API VmCapi          arg_getfunc(els_VmObj*,int );
ELS_API int             arg_gettype(els_VmObj*,int);

ELS_API void            arg_returnnum(els_VmObj*,Number);
ELS_API void            arg_returnstr(els_VmObj*,const char* );
ELS_API void            arg_returnfunc(els_VmObj*,VmCapi );
ELS_API void            arg_returnptr(els_VmObj*,char* );
ELS_API void            arg_returnnull(els_VmObj*);
ELS_API void            arg_returnbyte(els_VmObj*,char* ,size_t);


ELS_API const char*     obj_tostr(els_VmObj* ,LosuObj *);
ELS_API Number          obj_tonum(els_VmObj* ,LosuObj* );
ELS_API char*           obj_toptr(els_VmObj*,LosuObj*);
ELS_API VmCapi          obj_tofunction(els_VmObj*,LosuObj*);
ELS_API const char*     obj_tobyte(els_VmObj *,LosuObj *);
ELS_API int             obj_type(els_VmObj*,LosuObj*);

ELS_API LosuObj         obj_newstr(els_VmObj* ,char* );
ELS_API LosuObj         obj_newnum(els_VmObj* ,Number );
ELS_API LosuObj         obj_newfunction(els_VmObj*,els_C_API_function );
ELS_API LosuObj         obj_newnull(els_VmObj* );
ELS_API LosuObj         obj_newunit(els_VmObj* );
ELS_API LosuObj         obj_newptr(els_VmObj*,char* );
ELS_API LosuObj         obj_newbyte(els_VmObj*,char*,size_t );

ELS_API LosuObj*        obj_indexunit(els_VmObj*,LosuObj ,LosuObj );
ELS_API LosuObj*        obj_indexunitbynum(els_VmObj*vm,LosuObj unit,Number i);
ELS_API LosuObj*        obj_indexunitbystr(els_VmObj*vm,LosuObj unit,char* s);
ELS_API void            obj_setunit(els_VmObj*,LosuObj ,LosuObj ,LosuObj );
ELS_API void            obj_setunitbynum(els_VmObj*,LosuObj unit,Number key ,LosuObj value);
ELS_API void            obj_setunitbystr(els_VmObj*,LosuObj unit,char* key ,LosuObj value);


// 下面4个 API 仅在 Windows 内核中被启用，负责字符集的转换。
    // 洛书的内核全部为 utf-8编码，可以使用转换函数，将其转换为 GBK 后传递给 Windows API
        // 手动转换函数，转换后的字符串被存放在堆区中，需要手动释放堆区
ELS_API char*           vm_win_togbk(const char * );    
ELS_API char*           vm_win_toutf8(const char* );
        // 自动转换函数，转换后的字符串被存放在 Vm 的字符池中，会在下一个周期被 GC 自动回收 ( 如果GC 被 启动)
ELS_API char*           obj_toUTF8(els_VmObj*,char*);
ELS_API char*           obj_toGBK(els_VmObj*,char* );


ELS_API const char *ELS_TYPESYSTEM[];
ELS_API char els_buff_tmp[];

```

### 创建解释器
+ 标准 API 

```C++
els_VmObj *l=vm_create(1024);   // 创建虚拟机
els_lib_init(l);                // 初始化内核
vm_dostring(l,"");              // 解释字符串
vm_dofile(l,"");                // 解释文件

```

+ 快速 API(测试中)
```C++
int main(int argc,const char** argv){
    ElsNewApp(vm_dostring,"print('Hello')",1024 ,0 , argc,argv); // 创建一个 栈大小 1024B，GC上限 0B，解释字符串的 洛书应用(Losu APP)
}
```