#include "win_lssys.h"

const char *Lssys_getos()
{
    return ELS_BRANCH;
}

const char *Lssys_getenv(const char *name, char *buffer, int length)
{
    /*
        char *getenv(const char *varname);
        返回指向环境变量的指针。操作该指针并不安全，
        故建议加const
        失败时返回NULL
    */
    char *tmp = getenv(name);
    char *_t = NULL;
    if (tmp)
    {
        _t = vm_win_togbk(tmp);
        for (int i=0; i<length-1; i++)
        {
            buffer[i] = _t[i];
            if (buffer[i]=='\0')
                break;
        }
            
        free(_t);
        return buffer;
    }
    buffer = NULL;

    return buffer;
}

int Lssys_system(const char *cmd)
{
    char *tmp = vm_win_togbk(cmd);
    int output = system(cmd);
    free(tmp);
    return output;
}

void *Lssys_popen(const char *cmd, const char *mod)
{
    char *cmd_tmp = vm_win_togbk(cmd);
    FILE *output = popen(cmd, mod);
    free(cmd_tmp);
    return output;
}

int Lssys_pwrite(void *p, const char *s)
{
    char *s_tmp = vm_win_togbk(s);
    int output = fwrite(s_tmp, 1, strlen(s_tmp), p);
    free(s_tmp);
    return output;
}

int Lssys_pread(void *p, int n, char *buffer)
{
    char tmp[ELS_BUFF_TMP_SIZE];
    char *result;
    memset(tmp, '\0', ELS_BUFF_TMP_SIZE);

    //顶层函数会处理n的值，n值只会>=0
    n = (n>0 && n<ELS_BUFF_TMP_SIZE) ? n:ELS_BUFF_TMP_SIZE;
    fgets(tmp, n, p);
    result = vm_win_toutf8(tmp);
    strcpy(buffer, result);
    free(result);
    return n;
}

int Lssys_pclose(void *p)
{
    return pclose(p);
}

bool Lssys_getcwd(char *buffer, int length)
{
    /*
        char *_getcwd(char *buffer, int maxlen); <direct.h>
        成功时返回指向buffer的指针
        失败时返回NULL
    */
    char *output = _getcwd(buffer, length);
    return output==NULL ? false:true;
}

bool Lssys_exsists(const char * path)
{
    /*
        看不懂原型
        反正GetFileAttributes()的唯一参数是const char *
        找不到时返回INVALID_FILE_ATTRIBUTES
    */
    int output;

    if (path==NULL)
        return false;

    output = GetFileAttributes(path);
    if (output==INVALID_FILE_ATTRIBUTES)
        return false;
    return true;
}

bool Lssys_mkdir(const char* path)
{
    /*
        int _mkdir(const char *dirname); <direct.h>
        成功创建时返回0
    */
    int output = _mkdir(path);
    return output==0 ? true:false;
}

bool Lssys_rmdir(const char* path)
{
    /*
        int _rmdir(const char *dirname); <direct.h>
        成功删除时返回0
    */
    int output = _rmdir(path);
    return output==0 ? true:false;
}

bool Lssys_rename(const char *oldn, const char *newn)
{
    /*
        int rename(const char *oldname, const char *newname); <windows.h>
        成功时返回0
    */
    int output = rename(oldn, newn);
    return output==0 ? true:false;
}

bool Lssys_remove(const char* filepath)
{
    /*
        int remove(const char *filename); <stdio.h>
        成功时返回0
    */
    int output = remove(filepath);
    return output==0 ? true:false;
}

uint64_t Lssys_getpid()
{
    /*
        DWORD GetCurrentProcessId();
        返回值是调用进程的进程标识符。在进程终止之前，进程标识符会在整个系统中唯一标识进程。
    */
    return GetCurrentProcessId();
}

bool Lssys_join(const char *a, const char *b, char *result, int len)
{
    /*
        将字符串a和b以系统标准路径分隔符连接
    */
    strcat(result, a);
    strcat(result, LOSU_WIN_LSSYS_PATH_ALTSEP);
    strcat(result, b);
    return true;
}

bool Lssys_chdir(const char*path)
{
    /*
        BOOL SetCurrentDirectory(
            [in] LPCTSTR lpPathName
        );
        当成功时，函数返回非0值
    */
    if (SetCurrentDirectory(path) != 0)
        return true;
    return false;
}

bool Lssys_killp(uint64_t pid)
{
    HANDLE h = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (h == NULL)
        return false;
    if (!TerminateProcess(h,0))
        return false;
    return true;
}

uint64_t Lssys_lpcount()
{
    /*
        BOOL GetLogicalProcessorInformation(
            [out]     PSYSTEM_LOGICAL_PROCESSOR_INFORMATION Buffer,
            [in, out] PDWORD                                ReturnedLength
        );
    */
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    return info.dwNumberOfProcessors;
}

int Lssys_architecture()
{
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    switch (info.wProcessorArchitecture)
    {
        case PROCESSOR_ARCHITECTURE_AMD64:
        case PROCESSOR_ARCHITECTURE_INTEL:
            return LSSYS_ARCH_X86;
        case PROCESSOR_ARCHITECTURE_ARM:
        case PROCESSOR_ARCHITECTURE_ARM64:
            return LSSYS_ARCH_ARM;
        default:
            return LSSYS_ARCH_UNKNOW;
    }
}

uint64_t Lssys_memtotal()
{
    MEMORYSTATUSEX result;
    result.dwLength = sizeof(result); //调用GlobalMemoryStatuEx之前要在dwLength成员中储存整个结构的大小
    GlobalMemoryStatusEx(&result);
    return result.ullTotalPhys;
}

uint64_t Lssys_memfreep()
{
    MEMORYSTATUSEX result;
    result.dwLength = sizeof(result); //调用GlobalMemoryStatuEx之前要在dwLength成员中储存整个结构的大小
    GlobalMemoryStatusEx(&result);
    return result.ullAvailPhys;
}