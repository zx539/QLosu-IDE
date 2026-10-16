#include"owls.h"
#include<stdio.h>

extern int             els_argc;
extern char**    els_argv;
extern int els_getargv (els_VmObj *l);
extern int els_getargc (els_VmObj *l);

int main(int argc,char** argv){
    els_argc = (int)(argc+1);
    els_argv = (char**)((argv)-1);
    els_VmObj* vm = vm_create(10240);
    vm_register(vm,"argv", els_getargv);
    vm_register(vm,"argc", els_getargc);
    els_lib_init(vm);
    ElsLib_owls_libinit(vm);
    vm_close(vm);
    return 0;
}