#include "lpt.h"
#include<windows.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <urlmon.h>
#include <wininet.h>
int ELSAPI_lpt_wget(els_VmObj* vm) // wget(url: str,file: str): bool   '''从指定的url下载到文件file，成功返回真,失败返回null'''
{
    DeleteUrlCacheEntry(arg_getstr(vm,1));
    if(URLDownloadToFile(NULL,arg_getstr(vm,1),arg_getstr(vm,2),0,NULL)){
        arg_returnnull(vm);
        return 1;
    }
    arg_returnnum(vm,0);
    return 1;
}

int ELSAPI_lpt_mkdir(els_VmObj* vm) // mkdir(dir: str): bool            '''创建一个文件夹'''
{
    char tmp[ELS_BUFF_TMP_SIZE];
    sprintf(tmp,"mkdir %s ",arg_getstr(vm,1));
    char* _t=vm_win_togbk(tmp);
    if(system(_t))
        arg_returnnull(vm);
    else 
        arg_returnnum(vm,0);
    free(_t);
    return 1;
}

int ELSAPI_lpt_rmfile(els_VmObj* vm) // rmfile(f: str): bool             '''删除一个文件'''
{
    char tmp[ELS_BUFF_TMP_SIZE];
    sprintf(tmp,"del %s ",arg_getstr(vm,1));
    char* _t=vm_win_togbk(tmp);
    for(int i =0;i<strlen(_t);i++)
        if(i[_t]=='/') i[_t]='\\';
    if(system(_t))
        arg_returnnull(vm);
    else 
        arg_returnnum(vm,0);
    free(_t);
    return 1;
}
