#include "stdvm.h"
#include "els_vmhost.h"
#include<string.h>
extern void els_Gc_checkGC(els_VmObj *L);

int ELSAPI_stdvm_stdvm_get_version(els_VmObj* vm){
    arg_returnstr(vm,ELS_VERSION);
    return 1;
} // stdvm_get_version(): str    '获取版本信息'

int ELSAPI_stdvm_stdvm_get_build(els_VmObj* vm){
    arg_returnstr(vm,ELS_BUILD);
    return 1;
} // stdvm_get_build():   str    '获取构建信息'

int ELSAPI_stdvm_stdvm_get_copy(els_VmObj* vm){
    arg_returnstr(vm,ELS_COPYRIGHT);
    return 1;
} // stdvm_get_copy():    str    '获取版权信息'

int ELSAPI_stdvm_stdvm_get_branch(els_VmObj* vm){
    arg_returnstr(vm,ELS_BRANCH);
    return 1;
} // stdvm_get_branch():  str    '获取分支信息'

int ELSAPI_stdvm_stdvm_set_gc(els_VmObj* vm){
    int i = 0;
    switch (arg_getstr(vm,3)[0])
    {
        case 'B': case 'b' : i = 1; break;
        case 'K': case 'k' : i = 1024; break;
        case 'M': case 'm' : i = 1024*1024; break;
        default : i = 1024;break;
    }
    vm_setGC(vm,(unsigned long)((unsigned long)(i) * (unsigned long)(arg_getnum(vm,2)) ));
    return 0;

} // stdvm_set_gc(o:object,size: int,p:str): null   '设置GC上限,p可以为 B,KB(默认),M'

int ELSAPI_stdvm_stdvm_get_mem(els_VmObj* vm){
    int i = 0;
    switch (arg_getstr(vm,2)[0])
    {
        case 'B': case 'b' : i = 1; break;
        case 'K': case 'k' : i = 1024; break;
        case 'M': case 'm' : i = 1024*1024; break;
        default : i = 1;break;
    }
    double n = vm->nblocks / i;
    arg_returnnum(vm,n);
    return 1;
} // stdvm_get_mem(o: obj,p:str): int        '获取内存占用大小,单位 B(默认),K,M'
int ELSAPI_stdvm_stdvm_get_gc(els_VmObj* vm){
    int i = 0;
    switch (arg_getstr(vm,2)[0])
    {
        case 'B': case 'b' : i = 1; break;
        case 'K': case 'k' : i = 1024; break;
        case 'M': case 'm' : i = 1024*1024; break;
        default : i = 1;break;
    }
    if(vm->GCnowmax>vm->GCMAX)
        vm->GCnowmax=vm->GCMAX;
    double n = vm->GCnowmax  / i;
    arg_returnnum(vm,n);
    return 1;
} // stdvm_get_gc(o: obj,p:str): int         '获取GC上限，单位 B(默认),K,M' 

int ELSAPI_stdvm_stdvm_gc(els_VmObj* vm){
    vm->GCnowmax=vm->nblocks;
    els_Gc_checkGC(vm);
    return 0;
} // stdvm_gc()                  '立即进行GC'