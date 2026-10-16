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


#ifdef ELS_CONF_OS_WINDOWS
    #ifndef LOSU_WINDOWS
        #define LOSU_WINDOWS             1
    #endif
    #define ELS_CONF_TOKEN_CN        1
    #define ELS_CONF_TOKEN_EN        1
    #define ELS_CONF_LIBAPI_WIN32    1
    #define ELS_CONF_LIBAPI_ELF      1
    #define ELS_CONF_CHAR_GBK        1
    #define ELS_CONF_GC_ENABLE       1
    #define ELS_VM_STACK_SIZE        1024
    #define ELS_BUFF_TMP_SIZE        1024*1024
    #define ELS_VMIO_SIZE		     64
    #define ELS_HEAPMEM_REALLOC      realloc
    #define ELS_HEAPMEM_FREE         free
#endif

#ifdef ELS_CONF_OS_LINUX
    #ifndef LOSU_LINUX
        #define LOSU_LINUX               1
    #endif
    #define ELS_CONF_TOKEN_CN        1
    #define ELS_CONF_TOKEN_EN        1
    #define ELS_CONF_LIBAPI_DL       1
    #define ELS_CONF_LIBAPI_ELF      1
    #define ELS_CONF_GC_ENABLE       1
    #define ELS_VM_STACK_SIZE        1024
    #define ELS_BUFF_TMP_SIZE        1024*1024
    #define ELS_VMIO_SIZE		     64
    #define ELS_HEAPMEM_REALLOC      realloc
    #define ELS_HEAPMEM_FREE         free
#endif

#ifdef ELS_CONF_OS_EMBEDDEDOS
    #define ELS_CONF_TOKEN_CN        1
    #define ELS_CONF_TOKEN_EN        1
    #define ELS_CONF_LIBAPI_ELF      1
    #define ELS_CONF_GC_ENABLE       1
    #define ELS_VM_STACK_SIZE        128
    #define ELS_BUFF_TMP_SIZE        1024
    #define ELS_VMIO_SIZE		     64
    #define ELS_HEAPMEM_REALLOC             // 所在嵌入式 OS 的内存管理函数
    #define ELS_HEAPMEM_FREE                // 所在嵌入式 OS 的内存管理函数
#endif

#ifdef ELS_CONF_OS_RTTHREAD
    #define ELS_CONF_TOKEN_CN        1
    #define ELS_CONF_TOKEN_EN        1
    #define ELS_CONF_LIBAPI_ELF      1
    #define ELS_CONF_GC_ENABLE       1
    #define ELS_VM_STACK_SIZE        128
    #define ELS_BUFF_TMP_SIZE        1024
    #define ELS_VMIO_SIZE		     64
    #define ELS_HEAPMEM_REALLOC      rt_realloc
    #define ELS_HEAPMEM_REALLOC      rt_free
#endif



#ifndef __els_limits_h__
#define __els_limits_h__

    #include <limits.h>
    #include <stddef.h>

    // 16/32 位 模式选择（建议 在 32/64位 物理计算机上运行洛书解释器）
    #ifndef BITS_SIZE
        #if INT_MAX - 20 < 32760
            #define BITS_SIZE 16
        #else
            #if INT_MAX > 2147483640L
                #define BITS_SIZE 32
            #else
                #error "you must define BITS_SIZE with number of bits in an integer"
            #endif
        #endif
    #endif


    typedef double Number;
    typedef unsigned long lint32;
    typedef unsigned long Instruction;
    #define els_number2str(s, n) sprintf((s), "%.16g", (n))
    #define els_str2number(s, p) strtod((s), (p))
    
    #define MAX_SIZET ((size_t)(~(size_t)0) - 2)
    #define MAX_INT (INT_MAX - 2)
    #define IntPoint(p) ((unsigned long)(p))
#endif

#ifndef __els_h__
#define __els_h__

    #include <stddef.h>
    
    #define ELS_API extern
    #define ELS_REFNULL	(-1)    
    #define ELS_ANYTAG	(-1)
    #define ELS_MULTRET	(-1)
    #define NUM_TAGS 6
    
    // 错误代码 子系统
    #define ELS_ERRORBACK_RUN	 1  // 运行时错误
    #define ELS_ERRORBACK_FILE	 2  // 文件错误
    #define ELS_ERRORBACK_SYNTAX 3  // 语法错误
    #define ELS_ERRORBACK_MEM	 4  // 内存错误
    #define ELS_ERRORBACK_HEAP	 5  // 虚拟机堆栈溢出
    #define ELS_ERRORBACK_LIB    6  // 模块加载
    #define ELS_ERRORBACK_OVER  -1  // 灾难性崩溃
    

    // 数据类型 子系统
    #define ELS_TYPE_BYTE	    0
    #define ELS_TYPE_NULL	    1
    #define ELS_TYPE_NUMBER	    2   
    #define ELS_TYPE_STRING	    3
    #define ELS_TYPE_UNIT	    4
    #define ELS_TYPE_FUNCTION	5
    #define ELS_TYPE_MARK       6
    #define ELS_TYPE_PTR        7
    
    extern const int ELS_API_TYPE_BYTE	    ;
    extern const int ELS_API_TYPE_NULL	    ;
    extern const int ELS_API_TYPE_NUMBER	    ;
    extern const int ELS_API_TYPE_STRING	    ;
    extern const int ELS_API_TYPE_UNIT	    ;
    extern const int ELS_API_TYPE_FUNCTION	;
    extern const int ELS_API_TYPE_MARK        ;
    extern const int ELS_API_TYPE_PTR         ;
    
    
    typedef struct els_VmObj els_VmObj;
    typedef int (*els_C_API_function) (els_VmObj *L);

    #define TSPACK 4 // just for fun

    typedef struct 
    {
        int ttype;
        union
        {
            struct TString *ts;
            struct ElsCfunc *cl;
            struct Hash *a;
            struct CallInfo *i;
            double n;
            char*  ptr;
        } value;
    } StackObj;

    // 数据类型的声明与实现
    typedef struct TString
    {
        union
        {
            struct
            {
                unsigned long hash;
                int constindex;
            } s;
        } u;
        size_t len;
        struct TString *nexthash;
        int marked; 
        char str[TSPACK];
    } TString;
    typedef struct ElsCfunc
    {
        union
        {
            els_C_API_function c;
            struct els_func_code *l;
        } f;
        struct ElsCfunc *next;
        struct ElsCfunc *mark;
        short isC;
        short nupvalues;
        StackObj upvalue[1];
    } ElsCfunc;
    typedef struct Node
    {
        StackObj key;
        StackObj val;
        struct Node *next;
    } Node;
    typedef struct Hash
    {
        Node *node;
        int htag;
        int size;
        Node *firstfree;
        struct Hash *next;
        struct Hash *mark;
    } Hash;
    typedef struct CallInfo
    {
        struct ElsCfunc *func;
        const Instruction **pc;
        int lastpc;
        int line;
        int refi;
    } CallInfo;
    // Hash = key & (size-1)

    typedef struct LocVar
    {
        TString *varname;
        int startpc;
        int endpc;
    } LocVar;

    typedef struct els_func_code
    {
        Number *knum;
        int nknum;
        struct TString **kstr;
        int nkstr;
        struct els_func_code **kcodeir;
        int nkcodeir;
        Instruction *code;
        int ncode;
        short numparams;
        short is_vararg;
        short maxstacksize;
        short marked;
        struct els_func_code *next;
        int *lineinfo;
        int nlineinfo;
        int nlocvars;
        struct LocVar *locvars;
        int lineDefined;
        TString *source;
    } els_func_code;

    typedef StackObj LosuObj;
    typedef els_C_API_function VmCapi;
    
    extern const char ELS_VERSION[];
    extern const char ELS_COPYRIGHT[];
    extern const char ELS_BUILD[];
    extern const char ELS_BRANCH[];
    extern char els_buff_tmp[];
    extern int       els_argc;
    extern char**    els_argv;
    extern int els_getargv (els_VmObj *l);
    extern int els_getargc (els_VmObj *l); 
    extern int losu_printf(const char *);
    

#endif


/*api lib*/
ELS_API void els_lib_init(els_VmObj* );     //  初始化内核，加载 import 函数


ELS_API int ElsNewApp(int (*func)(els_VmObj*,const char* s),char *s,int stacksize,size_t gcmax,int argc,const char** argv);

ELS_API els_VmObj*      vm_create(int);
ELS_API els_VmObj*      vm_fork(els_VmObj*vm,int size);
ELS_API void            vm_stop(els_VmObj*);
ELS_API void            vm_error(els_VmObj*,const char* ,int );
ELS_API int             vm_dofile(els_VmObj*,const char* );
ELS_API int             vm_dostring(els_VmObj*,const char* );
ELS_API LosuObj*        vm_getval(els_VmObj*,const char* );
ELS_API void            vm_setval(els_VmObj *,const char* ,LosuObj );
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


ELS_API Node *          obj_unit_first(els_VmObj*vm,LosuObj unit);
ELS_API Node            obj_unit_location(els_VmObj*vm,LosuObj unit,LosuObj key);      
ELS_API Node *          obj_unit_next(els_VmObj*vm,LosuObj unit,Node*n);
ELS_API LosuObj         obj_unit_nodekey(els_VmObj*vm,Node* n);
ELS_API LosuObj         obj_unit_nodevalue(els_VmObj*vm,Node* n);



// 栈操作 API 暂时不对其进行拓展，仅提供最基本的功能，可以用于在虚拟机栈上构建函数并调用
ELS_API void            stack_push (els_VmObj*vm,LosuObj o);
ELS_API void            stack_pop  (els_VmObj*vm,int i);
ELS_API void            stack_call (els_VmObj*vm,int argnum,int resnum);



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



