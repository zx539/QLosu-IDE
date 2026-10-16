#include "lssys.h"

#include <stdbool.h>

#ifdef LOSU_WINDOWS
#include "win_lssys.h"
#endif

#ifdef LOSU_LINUX
#include "linux_lssys.h"
#endif

/*
    本文件的注释，第一行是用到的底层函数的原型
    下面的参数和返回值都是指的脚本层的接口函数的
*/

int ELSAPI_lssys_lssys_getos(els_VmObj* vm)
{
    /*
        const char *Lssys_getos()

        # 参数
            无
        
        # 返回值
            Windows Branch / Linux Branch
    */
    arg_returnstr(vm, Lssys_getos());
    return 1;
}

int ELSAPI_lssys_lssys_getenv(els_VmObj* vm)
{
    /*
        const char *Lssys_getenv(const char* name, char *buffer, int length)
    
        # 参数
            1.name   要获取的环境变量的名称
        
        # 返回值
            1. 当name的类型为string或number时，返回包含环境变量信息的字符串
            2. 当name的类型非法时，返回null
    */
    if (arg_gettype(vm, 1) == ELS_API_TYPE_STRING)
    {
        const char *name = arg_getstr(vm, 1);
        char buffer[ELS_BUFF_TMP_SIZE];
        arg_returnstr(vm, Lssys_getenv(name, buffer, ELS_BUFF_TMP_SIZE));
    }
    else
        arg_returnnull(vm);
    return 1;
}

int ELSAPI_lssys_lssys_system(els_VmObj* vm)
{
    /*
        int Lssys_system(const char *cmd);

        # 参数
            1.cmd 要执行的命令

        # 返回值
            1. 当cmd的类型为string时, 返回命令执行完后的状态码
            2. 当cmd的类型非法时，返回null
    */
   if (arg_gettype(vm, 1) == ELS_API_TYPE_STRING)
    {
        const char *cmd = arg_getstr(vm, 1);
        arg_returnnum(vm, Lssys_system(cmd));
    }
    else
        arg_returnnull(vm);
    
    return 1;
}

int ELSAPI_lssys_lssys_popen(els_VmObj* vm)
{
    /*
        FILE *Lssys_popen(const char *cmd, const char *mod);

        # 参数
            1. cmd 要执行的命令
            2. mod 要执行的模式
        
        # 返回值
            1. fp 当cmd的类型为string时，返回管道的文件指针
            2. 当存在非法类型时，返回null
    */
    if (arg_gettype(vm, 1) == ELS_API_TYPE_STRING && arg_gettype(vm, 2)==ELS_API_TYPE_STRING)
    {
        const char *cmd = arg_getstr(vm, 1);
        const char *mod = arg_getstr(vm, 2);

        FILE *fp = Lssys_popen(cmd, mod);
        arg_returnptr(vm, (void*)fp);
    }
    else
        arg_returnnull(vm);

    return 1;
}

int ELSAPI_lssys_lssys_pwrite(els_VmObj* vm)
{
    /*
        int Lssys_pwrite(void *p, const char *s);

        # 参数
            1. p 管道的文件指针
            2. s 要写入的字符串
        # 返回值
            未知
    */
    //p的类型只能为ptr
    //s的类型可以为string或number
    if (arg_gettype(vm, 1)==ELS_API_TYPE_PTR && (arg_gettype(vm, 2) == ELS_API_TYPE_STRING || arg_gettype(vm, 2) == ELS_API_TYPE_NUMBER))
    {
        void *p = arg_getptr(vm, 1);
        const char *s = arg_getstr(vm, 2);
        int output = Lssys_pwrite(p, s);
        arg_returnnum(vm, output);
    }
    else
        arg_returnnull(vm);
    
    
    return 1;
}

int ELSAPI_lssys_lssys_pread(els_VmObj* vm)
{
    /*
        int Lssys_pread(void *p, int n, char *buffer);

        # 参数
            1. p 管道的文件指针
            2. n 要读取的字符数量
        
        # 返回值
            1. 成功读取时返回读取到的内容
            2. 失败时返回null
            3. 参数类型必须都为string，否则也返回null
    */
    //p的类型必须为ptr而且n的类型必须为number
    if (arg_gettype(vm, 1) == ELS_API_TYPE_PTR && arg_gettype(vm, 2) == ELS_API_TYPE_NUMBER)
    {
        void *p = arg_getptr(vm, 1);
        int n = arg_getnum(vm, 2);
        char *buffer = NULL;

        //n必须>0
        if (n>0)
        {
            buffer = malloc(sizeof(char)*(n+1));
            if (Lssys_pread(p, n, buffer))
                arg_returnstr(vm, buffer);
            else
                arg_returnnull(vm);
            free(buffer);
            return 1;
        }
        arg_returnnull(vm);
    }
    else
        arg_returnnull(vm);
    
    
    return 1;
}

int ELSAPI_lssys_lssys_pclose(els_VmObj* vm)
{
    /*
        int Lssys_pclose(void *p);

        # 参数
            1. p 管道的文件指针
        
        # 返回值
            未知
    */
    //p的类型必须为ptr
    if (arg_gettype(vm, 1) == ELS_API_TYPE_PTR)
    {
        
        void *p = arg_getptr(vm, 1);
        arg_returnnum(vm, Lssys_pclose(p));
    }
    else
        arg_returnnull(vm);
    
    return 1;
}

int ELSAPI_lssys_lssys_getcwd(els_VmObj* vm)
{
    /*
        bool Lssys_getcwd(char *buffer, int maxlen);

        # 参数
            无
        
        # 返回值
            1. 返回当前工作目录的字符串
    */
    //工作目录的缓冲区大小为1024
    //大概不会溢出
    char buffer[1024];
    if (Lssys_getcwd(buffer, 1024))
        arg_returnstr(vm, buffer);
    else
        arg_returnnull(vm);
    return 1;
}

int ELSAPI_lssys_lssys_exsists(els_VmObj* vm)
{
    /*
        bool Lssys_exist(const char *path);

        # 参数
            1. path 路径
        
        # 返回值
            1. 在path类型为string且路径存在时返回1
            2. 不存在时返回null
    */
    if (arg_gettype(vm, 1) == ELS_API_TYPE_STRING)
    {
        const char *path = arg_getstr(vm, 1);
        if (Lssys_exsists(path))
            arg_returnnum(vm, 1);
        else
            arg_returnnull(vm);
    }
    else
        arg_returnnull(vm);
    return 1;
}

int ELSAPI_lssys_lssys_mkdir(els_VmObj* vm)
{
    /*
        bool Lssys_mkdir(const char *path);

        # 参数
            1. path 路径
        
        # 返回值
            1. 在path类型为string且成功时返回1
            2. 失败时返回null
    */
    if (arg_gettype(vm, 1) == ELS_API_TYPE_STRING)
    {    
        const char *path = arg_getstr(vm, 1);
        if (Lssys_mkdir(path))
            arg_returnnum(vm, 1);
        else
            arg_returnnull(vm);
    }
    else
        arg_returnnull(vm);
    
    return 1;
}
int ELSAPI_lssys_lssys_rmdir(els_VmObj* vm)
{
    /*
        bool Lssys_rmdir(const char *path);

        # 参数
            1. path 路径
        
        # 返回值
            1. 在path类型为string且成功时返回1
            2. 在path类型非法或失败时返回null
    */
    if (arg_gettype(vm, 1) == ELS_API_TYPE_STRING)
    {
        const char *path = arg_getstr(vm, 1);
        if (Lssys_rmdir(path))
            arg_returnnum(vm, 1);
        else
            arg_returnnull(vm);
    }
    else
        arg_returnnull(vm);
    
    return 1;
}

int ELSAPI_lssys_lssys_rename(els_VmObj* vm)
{
    /*
        bool Lssys_rename(const char *oldname, const char *newname);

        # 参数
            1. oldname 旧的文件名
            2. newname 新的文件名
        
        # 返回值
            1. 在oldname和newname类型均为string并且成功时返回1
            2. 在有非法类型或失败时返回null
    */
    //oldname和newname的类型都应该为string
    if (arg_gettype(vm, 1) == ELS_API_TYPE_STRING && arg_gettype(vm, 2) == ELS_API_TYPE_STRING)
    {
        const char *oldname = arg_getstr(vm, 1);
        const char *newname = arg_getstr(vm, 2);
        if (Lssys_rename(oldname, newname))
            arg_returnnum(vm, 1);
        else
            arg_returnnull(vm);
    }
    else
        arg_returnnull(vm);
    return 1;
}

int ELSAPI_lssys_lssys_remove(els_VmObj* vm)
{
    /*
        bool Lssys_remove(const char *filepath);

        # 参数
            1. path 文件路径

        # 返回值
            1. path的类型为sting并且成功时返回1
            2. path的类型非法并且失败时返回null
    */
    if (arg_gettype(vm, 1) == ELS_API_TYPE_STRING)
    {
        
        const char *path = arg_getstr(vm, 1);

        //当成功删除时，Lssys_remove会返回真值
        if (Lssys_remove(path))
            arg_returnnum(vm, 1);
        else
            arg_returnnull(vm);
    }
    else
        arg_returnnull(vm);
    
    return 1;
}

int ELSAPI_lssys_lssys_getpid(els_VmObj* vm)
{
    /*
        uint64_t Lssys_getpid();

        # 参数
            无
        
        # 返回值
            1. id 当前进程的id
    */
    arg_returnnum(vm, Lssys_getpid()); //Lssys_getid已经封装了获取当前进程id的功能
    return 1;
}


int ELSAPI_lssys_lssys_join(els_VmObj* vm)
{
    /*
        bool Lssys_join(const char* a, const char * b, char * result, int len);

        # 参数
            1. a:str 前面的字符串
            2. b:str 后面的字符串
        
        # 返回值
            1. 用系统规范路径分隔符连接的a和b
            2. 当类型非法时，返回null
    */
    if (arg_gettype(vm, 1) == ELS_API_TYPE_STRING && arg_gettype(vm, 2) == ELS_API_TYPE_STRING)
    {
        const char *a = arg_getstr(vm, 1);
        const char *b = arg_getstr(vm, 2);
        char *result;
        int len;

        //计算预计缓冲区大小
        len = (strlen(a) + strlen(b) + 2) * sizeof(char);

        //分配缓冲区
        result = malloc(len);
        memset(result, 0, len);

        //得到结果
        if (Lssys_join(a, b, result, len))
            arg_returnstr(vm, result);
        else
            arg_returnnull(vm);
        free(result);
    }
    else
        arg_returnnull(vm);
    return 1;
}

int ELSAPI_lssys_lssys_chdir(els_VmObj* vm)
{
    /*
        bool Lssys_chdir(const char *path);

        # 参数
            1. path:str 目标路径
        
        # 返回值
            1. 当成功改变当前工作路径时返回1
            2. 当改变工作路径失败时返回null
            3. 当类型不合法时返回null
    */
    if (arg_gettype(vm, 1) == ELS_API_TYPE_STRING)
    {
        const char *path = arg_getstr(vm, 1);

        if (Lssys_chdir(path))
            arg_returnnum(vm, 1);
        else
            arg_returnnull(vm);
    }
    else
        arg_returnnull(vm);
    return 1;
}

int ELSAPI_lssys_lssys_killp(els_VmObj* vm)
{
    /*
        bool Lssys_killp(uint64_t pid);

        # 参数
            1. pid:int 要杀死的进程的id
        
        # 返回值
            1. 成功杀死时返回1
            2. 未成功杀死时返回null
            3. 当类型不合法时返回null
    */
    if (arg_gettype(vm, 1) == ELS_API_TYPE_NUMBER)
    {
        uint64_t pid = arg_getnum(vm, 1);
        if (Lssys_killp(pid))
            arg_returnnum(vm, 1);
        else
            arg_returnnull(vm);
    }
    else
        arg_returnnull(vm);
    return 1;
}

int ELSAPI_lssys_lssys_lpcount(els_VmObj* vm)
{
    /*
        uint32_t Lssys_lpcount();

        # 参数
            无

        # 返回值
            1. 当前计算机的逻辑处理器个数
    */
    int result = Lssys_lpcount();
    if (result == -1)
        arg_returnnull(vm);
    else
        arg_returnnum(vm, result);
    return 1;
}


int ELSAPI_lssys_lssys_architecture(els_VmObj* vm)
{
    /*
        int Lssys_architecture();

        可能的返回值有
        LSSYS_ARCH_ARM
        LSSYS_ARCH_X86
        LSSYS_ARCH_UNKNOW

        # 参数
            无
        
        # 返回值
            1. 当前CPU的架构
            2. 当架构未知时，返回null
    */
    const char *output;

    switch (Lssys_architecture())
    {
        case LSSYS_ARCH_ARM:
            output = "ARM";
            break;
        case LSSYS_ARCH_X86:
            output = "X86";
            break;
        default:
            output = NULL;
    }

    if (output!=NULL)
        arg_returnstr(vm, output);
    else
        arg_returnnull(vm);
    return 1;
}

int ELSAPI_lssys_lssys_memtotal(els_VmObj* vm)
{
    /*
        uint64_t Lssys_memtotal();

        # 参数
            无
        
        # 返回值
            1. 操作系统的总内存量
    */
    arg_returnnum(vm, Lssys_memtotal());
    return 1;
}

int ELSAPI_lssys_lssys_memfreep(els_VmObj* vm)
{
    /*
        uint64_t Lssys_memfreep();

        # 参数
            无
        
        # 返回值
            1. 当前线程的可用内存量
    */
    arg_returnnum(vm, Lssys_memfreep());
    return 1;
}