#include "stdsys.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <stdio.h>

int ELSAPI_stdsys_sys_getos(els_VmObj* vm) // sys_getos(): str '从内核中获取所在操作系统信息，一般为 Windwos/Linux Branch'
{
    arg_returnstr(vm,ELS_BRANCH);   
    return 1; 
}
int ELSAPI_stdsys_sys_getenv(els_VmObj* vm) // sys_getenv(n: str) '获取名称为n的环境变量'
{
    char * tmp = getenv(arg_getstr(vm,1));
    char* _t;
    if(tmp){
        _t = vm_win_togbk(tmp);
        arg_returnstr(vm,_t);
        free(_t);
        return 1;
    }
    arg_returnnull(vm);
    return 1;
}
int ELSAPI_stdsys_sys_system(els_VmObj* vm) // sys_system(cmd: str): int '调用一个命令'
{
    char* tmp = vm_win_togbk(arg_getstr(vm,1));
    arg_returnnum(vm,(Number)(system(tmp)));
    free(tmp);
    return 1;
}
int ELSAPI_stdsys_sys_popen(els_VmObj* vm) // sys_popen(cmd: str, mod:str): ptr '创建一个管道，mod可取为 r,w'
{
    char* tmp = vm_win_togbk(arg_getstr(vm,1));
    FILE *f = popen(tmp,arg_getstr(vm,2));free(tmp);
    if(f){
        arg_returnptr(vm,(char*)(f));
        return 1;
    }
    arg_returnnull(vm);
    return 1;

}
int ELSAPI_stdsys_sys_pwrite(els_VmObj* vm) // sys_pwrite(p: ptr,s: str): int '向指定管道 p 写入 s'
{
    char* tmp = vm_win_togbk(arg_getstr(vm,2));
    arg_returnnum(vm,(Number)fwrite(tmp,1,strlen(tmp),(FILE*)(arg_getptr(vm,1))));
    free(tmp);
    return 1;

}
int ELSAPI_stdsys_sys_pread(els_VmObj* vm) // sys_pread(p: ptr, n: int): int '从指定管道 p 读取 n 个字符'
{
    char tmp[ELS_BUFF_TMP_SIZE];
    char *_t;
    memset(tmp,'\0',ELS_BUFF_TMP_SIZE);
    if(arg_gettype(vm,2)==ELS_API_TYPE_NULL)
        fgets(tmp,ELS_BUFF_TMP_SIZE,(FILE*)(arg_getptr(vm,1)));
    else 
        fgets(tmp,(int)arg_getnum(vm,2),(FILE*)(arg_getptr(vm,1)));
    
    _t = vm_win_toutf8(tmp);
    arg_returnstr(vm,_t);
    free(_t);
    return 1;
}
int ELSAPI_stdsys_sys_pclose(els_VmObj* vm) // sys_pclose(p: ptr): int '关闭一个管道'
{
    arg_returnnum(vm, (Number)pclose((FILE*)(arg_getptr(vm,1))));
    return 1;
}
