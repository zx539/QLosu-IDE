#include "lpt.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int ELSAPI_lpt_mkdir(els_VmObj* vm)
{
// 这里填写API函数的 C 实现
    char tmp[ELS_BUFF_TMP_SIZE];
    sprintf(tmp,"mkdir %s ",arg_getstr(vm,1));
    if(system(tmp))
        arg_returnnull(vm);
    else 
        arg_returnnum(vm,0);
    return 1;
}
int ELSAPI_lpt_wget(els_VmObj* vm)
{
// 这里填写API函数的 C 实现
    char tmp[ELS_BUFF_TMP_SIZE];
    sprintf(tmp,"wget --no-check-certificate -q --show-progress   \"%s\" -O \"%s\"",arg_getstr(vm,1),arg_getstr(vm,2));
    if(system(tmp)){
        sprintf(tmp,"rm -f %s",arg_getstr(vm,2));
        system(tmp);
        arg_returnnull(vm);
        return 0;
    }else 
        arg_returnnum(vm,0);
    sprintf(tmp,"chmod 777 %s",arg_getstr(vm,2));
    system(tmp);
    return 1;
}
int ELSAPI_lpt_rmfile(els_VmObj* vm)
{
// 这里填写API函数的 C 实现
    char tmp[ELS_BUFF_TMP_SIZE];
    sprintf(tmp,"rm -rf %s ",arg_getstr(vm,1));
    if(system(tmp))
        arg_returnnull(vm);
    else 
        arg_returnnum(vm,0);
    return 1;
}
