
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "els.h"
#include "els_vmhost.h"


#ifdef ELS_CONF_LIBAPI_ELF
    #include "els_libload.h"
#endif

#ifdef ELS_CONF_LIBAPI_DL   // Linux 平台的动态库加载方式 libdl
    #include <unistd.h>
    #include <dlfcn.h>
#endif

#ifdef ELS_CONF_LIBAPI_WIN32    // Windows 平台的动态库加载方式 win32API
    #include <windows.h>
#endif

static int els_inline_libload(els_VmObj* vm);
static int els_inline_classnew(els_VmObj* vm);  
 
ELS_API void els_lib_init(els_VmObj *l)
{

    vm_register(l, "import", els_inline_libload);
    vm_register(l, "new", els_inline_classnew);
#ifdef ELS_CONF_TOKEN_CN
    vm_register(l, "引", els_inline_libload);
#endif
}


// new函数的C语言实现
static int els_inline_classnew(els_VmObj* vm){
    LosuObj* oldtop = vm->top;
    int args = arg_num(vm);

    //判断有没有  class 类型
    LosuObj *class = arg_get(vm,1);
    LosuObj *func = obj_indexunit(vm,*class,obj_newstr(vm,"class"));
    if(obj_type(vm,class)!=ELS_TYPE_UNIT || func==NULL || obj_type(vm,func)!= ELS_TYPE_FUNCTION){
        vm_error(vm,"new 函数的第一个参数必须为 包含class成员函数的 unit 对象",ELS_ERRORBACK_RUN);
    }
    stack_push(vm,*func);
    stack_push(vm,*class);
    for(int i=2;i<=args;i++)
        stack_push(vm,*arg_get(vm,i));
    stack_call(vm,args,-1);
    // els_call(vm,2,-1);
    // els_Heap_call(vm,vm->top-3,-1);
    // vm->top++;
    // return 1;
    return vm->top - oldtop;
}
// import 函数的 C 语言实现
static int els_inline_libload(els_VmObj *l)
{
    char tmp[512];
#ifdef ELS_CONF_LIBAPI_ELF
	const char * s = arg_getstr(l,1);
    for (int i = 0; i < sizeof(els_libload_alllib) / sizeof(els_libload_alllib[0]); i++)
        if(!strcmp( els_libload_alllib[i].name,s)){
            els_libload_alllib[i].func(l);
        }
        
#endif
	
#ifdef ELS_CONF_LIBAPI_WIN32
    #ifdef ELS_CONF_LIBAPI_DL
        #error "ELS_CONF_LIBAPI_WIN32 与 ELS_CONF_LIBAPI_DL 参数只能选择一个"
    #endif

    const char *libfile = arg_getstr(l, 1);
    typedef void (*api)(els_VmObj *l);
    sprintf(tmp, "./ElsLib_%s.lsd", libfile); // 先尝试加载工作目录的
    HMODULE dlibptr = LoadLibrary(tmp);
    if (dlibptr == NULL)
    {
        sprintf(tmp, "./lib/ElsLib_%s.lsd", libfile); // 尝试加载本地环境的
        dlibptr = LoadLibrary(tmp);
    }
    sprintf(tmp, "ElsLib_%s_libinit", libfile);
    api func = (api)GetProcAddress(dlibptr, tmp);
    if (func == NULL)
    {
        sprintf(tmp, "LibError: Couldn't Load Library \t%s\n", libfile);
        vm_error(l, tmp,ELS_ERRORBACK_LIB);
    }
    func(l);
    return 1;
#endif
#ifdef ELS_CONF_LIBAPI_DL
    #ifdef ELS_CONF_LIBAPI_WIN32
        #error "ELS_CONF_LIBAPI_WIN32 与 ELS_CONF_LIBAPI_DL 参数只能选择一个"
    #endif
    
    const char *libfile = arg_getstr(l, 1);
    typedef void (*api)(els_VmObj *l);
    sprintf(tmp, "./ElsLib_%s.lsd", libfile); // 先加载工作目录的
    void *dlibptr = dlopen(tmp, RTLD_LAZY);
    if (dlibptr == NULL)
    {
        char _tmp[256] = {0};
        readlink("/proc/self/exe", _tmp, 256);
        int _t = 0;
        for (int i = 0; i < strlen(_tmp); i++)
            if (_tmp[i] == '/')
                _t = i;

        _tmp[_t] = '\0';

        sprintf(tmp, "%s/ElsLib_%s.lsd", _tmp, libfile); // 加载容器目录的
        dlibptr = dlopen(tmp, RTLD_LAZY);
        if (dlibptr == NULL)
        {
            sprintf(tmp, "%s/lib/ElsLib_%s.lsd", _tmp, libfile); // 加载容器目录的
            dlibptr = dlopen(tmp, RTLD_LAZY);
        }
    }
    if (dlibptr == NULL)
    {
        sprintf(tmp, "/els/lib/ElsLib_%s.lsd", libfile); // 加载本地环境的
        dlibptr = dlopen(tmp, RTLD_LAZY);
    }
    if (dlibptr == NULL)
    {
        sprintf(tmp, "LibError: Couldn't Load Library\t%s\n", libfile);
        vm_error(l, tmp,ELS_ERRORBACK_LIB);
    }
    sprintf(tmp, "ElsLib_%s_libinit", libfile);
    api func = (api)dlsym(dlibptr, tmp);
    if (func == NULL)
    {
        sprintf(tmp, "LibError: Couldn't Load Library\t%s\n", libfile);
        vm_error(l, tmp,ELS_ERRORBACK_LIB);
    }
    func(l);
    return 1;
#endif
    sprintf(tmp, "LibError: Couldn't Load Library \t%s\n", s);
    return 1;
}