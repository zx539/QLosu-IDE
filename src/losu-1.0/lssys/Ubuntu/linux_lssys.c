#include "linux_lssys.h"

#ifdef LOSU_LINUX

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
    if (tmp)
    {
        for (int i=0; i<length-1; i++)
        {
            buffer[i] = tmp[i];
            if (buffer[i]=='\0')
                break;
        }
        return buffer;
    }
    buffer = NULL;

    return buffer;
}

int Lssys_system(const char *cmd)
{
    int output = system(cmd);
    return output;
}

void *Lssys_popen(const char *cmd, const char *mod)
{
    FILE *output = popen(cmd, mod);
    return output;
}

int Lssys_pwrite(void *p, const char *s)
{
    int output = fwrite(s, 1, strlen(s), p);
    return output;
}

int Lssys_pread(void *p, int n, char *buffer)
{
    char tmp[ELS_BUFF_TMP_SIZE];
    memset(tmp, '\0', ELS_BUFF_TMP_SIZE);

    //顶层函数会处理n的值，n值只会>=0
    n = (n>0 && n<ELS_BUFF_TMP_SIZE) ? n:ELS_BUFF_TMP_SIZE;

    //fget的返回值不能忽略
    //写成这样，结果一样，但编译器不会报warning
    if (fgets(tmp, n, p))
        strcpy(buffer, tmp);
    else
        strcpy(buffer, tmp);
    return n;
}

int Lssys_pclose(void *p)
{
    return pclose(p);
}

bool Lssys_getcwd(char *buffer, int length)
{
    /*
        char *getcwd(char *buffer, int size); <unistd.h>
    */
    if (getcwd(buffer, length)==NULL)
        return false;
    return true;
}

bool Lssys_exsists(const char *path)
{
    /*
        int access(const char *pathname, int mode); <unistd.h>

        成功返回0

        mode:
            R_OK 只判断是否有读权限
		    W_OK 只判断是否有写权限
		    X_OK 判断是否有执行权限
		    F_OK 只判断是否存在
    */
    return access(path, F_OK)==0 ? true : false;
}

bool Lssys_mkdir(const char* path)
{
    /*
        int mkdir(const char *pathname, mode_t mode);
            参数：
                1：创建目录路径
 	            2：创建文件的模式
            
            创建成功则返回0
            
            mode:
                S_IRWXU	00700权限，代表该文件所有者拥有读，写和执行操作的权限
                S_IRUSR(S_IREAD)	00400权限，代表该文件所有者拥有可读的权限
                S_IWUSR(S_IWRITE)	00200权限，代表该文件所有者拥有可写的权限
                S_IXUSR(S_IEXEC)	00100权限，代表该文件所有者拥有执行的权限
                S_IRWXG	00070权限，代表该文件用户组拥有读，写和执行操作的权限
                S_IRGRP	00040权限，代表该文件用户组拥有可读的权限
                S_IWGRP	00020权限，代表该文件用户组拥有可写的权限
                S_IXGRP	00010权限，代表该文件用户组拥有执行的权限
                S_IRWXO	00007权限，代表其他用户拥有读，写和执行操作的权限
                S_IROTH	00004权限，代表其他用户拥有可读的权限
                S_IWOTH	00002权限，代表其他用户拥有可写的权限
                S_IXOTH	00001权限，代表其他用户拥有执行的权限
    */
    return mkdir(path, S_IRWXU)==0 ? true : false;
}

bool Lssys_rmdir(const char* path)
{
    /*
        int rmdir(const char *pathname);
        成功时返回0

    */
    return rmdir(path)==0 ? true:false;
}

bool Lssys_rename(const char *oldn, const char *newn)
{
    /*
        int rename(const char *oldname, const char *newname); <stdio.h>
        成功时返回0
    */
    return rename(oldn, newn)==0 ? true : false;
}

bool Lssys_remove(const char* path)
{
    /*
        int remove(char * filename);
        成功时返回0
    */
    return remove(path);
}

uint64_t Lssys_getpid()
{
    return getpid();
}

bool Lssys_join(const char *a, const char *b, char *result, int len)
{
    strcat(result, a);
    strcat(result, LOSU_LINUX_LSSYS_PATH_ALTSEP);
    strcat(result, b);
    return true;
}

bool Lssys_chdir(const char* path)
{
    /*
        chdir(const char *path); <unistd.h>
        成功返回0，失败返回-1
    */
    if (chdir(path)==0)
        return true;
    return false;
}

bool Lssys_killp(uint64_t pid)
{
    if (kill(pid, SIGKILL) == 0)
        return true;
    return false;
}

uint64_t Lssys_lpcount()
{
    return get_nprocs();
}

int Lssys_architecture()
{
    /*
        struct utsname {
            char sysname[];    / Operating system name (e.g., "Linux") /
            char nodename[];   / Name within "some implementation-defined
                          network" /
            char release[];    / Operating system release (e.g., "2.6.28") /
            char version[];    / Operating system version /
            char machine[];    / Hardware identifier /
        #ifdef _GNU_SOURCE
            char domainname[]; / NIS or YP domain name /
        #endif
        };

        int uname(struct utsname *buf);
    
    */
    struct utsname info;
    uname(&info);
    
    if  (strcmp(info.machine, "x86_64") == 0)
        return LSSYS_ARCH_X86;
    else if (strcmp(info.machine, "amd64") == 0)
        return LSSYS_ARCH_X86;
    else if (strcmp(info.machine, "ARM") == 0)
        return LSSYS_ARCH_ARM;
    else
        return LSSYS_ARCH_UNKNOW;
}

/*
    Linux 内存信息储存在 /proc/meminfo 文件中
*/

uint64_t Lssys_memtotal()
{
    FILE *fp = fopen("/proc/meminfo", "r");
    uint64_t size;
    
    if (fp == NULL)
        return -1;

    if (fscanf(fp, "MemTotal:        %ld kB", &size))
        return size * 1024;
    return -1;
}

uint64_t Lssys_memfreep()
{
    FILE *fp = fopen("/proc/meminfo", "r");
    uint64_t size;
    
    if (fp == NULL)
        return -1;

    for (int i = 0; i<2; i++)
        for (;;)
        {
            char ch = fgetc(fp);
            if (ch == '\n')
                break;
        }

    if (fscanf(fp, "MemAvailable:     %ld kB",&size) == 1)
        return size * 1024;
    return -1;
}

#endif