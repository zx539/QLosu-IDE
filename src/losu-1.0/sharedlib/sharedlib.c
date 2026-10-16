#include "els.h"
#include "sharedlib.h"

/*
    xkit的动态库操作接口
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __linux__

#include <unistd.h>
#include <dlfcn.h>

#endif

#ifdef _WIN32

#include <windows.h>

#endif

#define unit_arg(x) x+1
#define unit_arg_gettype(vm, x) arg_gettype(vm, x+1)

int ELSAPI_sharedlib_sharedlib_load(els_VmObj *vm)
{
    int argnum = arg_num(vm);

    if (argnum == unit_arg(0)) //当参数个数(除this指针外) == 0
    {
        arg_returnnull(vm);
        return 1;
    }
    else if (argnum == unit_arg(1)) //当参数个数(除this指针外) == 1
    {
        const char *name = arg_getstr(vm, unit_arg(1));
        if (name[0] == '\0')
        {
            arg_returnnull(vm);
            return 1;
        }
        #ifdef __linux__
        void *p = dlopen(name, RTLD_LAZY);
        #endif

        #ifdef _WIN32
        void *p = LoadLibrary(name);
        #endif

        if (p == NULL)
        {
            arg_returnnull(vm);
            return 1;
        }

        arg_returnptr(vm, p);
        return 1;
    }
    
    //当参数个数(除this指针外)>=2时会运行到这里
    LosuObj unit = obj_newunit(vm);
    for (int i = 1; i <= argnum - 1; i++)
    {
        const char *s = arg_getstr(vm, unit_arg(i));
        
        #ifdef __linux__
        void *p = dlopen(s, RTLD_LAZY); //Linux下的动态库打开方式
        #endif

        #ifdef _WIN32
        void *p = LoadLibrary(s);
        #endif

        if (p != NULL)
            obj_setunitbynum(vm, unit, i, obj_newptr(vm, p));
        else
            obj_setunitbynum(vm, unit, i, obj_newnull(vm));
    }
    arg_return(vm, unit);

    return 1;
}

int ELSAPI_sharedlib_sharedlib_unload(els_VmObj *vm)
{
    int argnum = arg_num(vm);

    if (argnum == unit_arg(0)) //当参数个数(除this指针外) == 0
    {
        arg_returnnull(vm);
        return 1;
    }
    else if (argnum == unit_arg(1)) //当参数个数(除this指针外) == 1
    {
        void *p = arg_getptr(vm, unit_arg(1));

        if (p == NULL)
        {
            arg_returnnull(vm);
            return 1;
        }

        #ifdef __linux__
        dlclose(p);
        #endif

        #ifdef _WIN32
        FreeLibrary(p);
        #endif

        arg_returnnull(vm);
        return 1;
    }

    //当参数个数(除this指针外)>=2时会运行到这里
    for (int i = 1; i <= argnum - 1; i++)
    {
        void *p = arg_getptr(vm, unit_arg(i));

        if (p == NULL)
        {
            continue;
        }

        #ifdef __linux__
        dlclose(p);
        #endif

        #ifdef _WIN32
        FreeLibrary(p);
        #endif
    }

    return 1;
}

int ELSAPI_sharedlib_sharedlib_sys(els_VmObj *vm)
{
    if (unit_arg_gettype(vm, 1) == ELS_API_TYPE_PTR && unit_arg_gettype(vm, 2) == ELS_API_TYPE_STRING)
    {
        void *p = arg_getptr(vm, unit_arg(1));
        const char *s = arg_getstr(vm, unit_arg(2));
        
        if (p == NULL || s[0] == '\0')
        {
            arg_returnnull(vm);
            return 1;
        }

        #ifdef __linux__
        void *proc = dlsym(p, s);
        #endif

        #ifdef _WIN32
        void *proc = GetProcAddress(p, s);
        #endif

        if (proc == NULL)
            arg_returnnull(vm);
        else
            arg_returnptr(vm, proc);

        return 1;
    }
    else
    {
        arg_returnnull(vm);
    }

    return 1;
}
