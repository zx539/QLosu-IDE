#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stddef.h>
#include<stdarg.h>

#include "els.h"
#include "els_heap.h"
#include "els_func.h"
#include "els_gc.h"
#include "els_mem.h"
#include "els_object.h"
#include "els_vmhost.h"
#include "els_string.h"
#include "els_unit.h"
#include "els_vmcore.h"

#ifdef __has_include
    #if __has_include("els_version.h")
        #include "els_version.h"
    #else
        #define ELS_CORE_VERSION "core_version 2.0.0"
    #endif
#else
    #define ELS_CORE_VERSION "core_version 2.0.0"
#endif

#ifndef ELS_CORE_VERSION 
    #define ELS_CORE_VERSION "core_version 2.0.0"
#endif

const int ELS_API_TYPE_BYTE	    =ELS_TYPE_BYTE;
const int ELS_API_TYPE_NULL	    =ELS_TYPE_NULL;
const int ELS_API_TYPE_NUMBER	    =ELS_TYPE_NUMBER;
const int ELS_API_TYPE_STRING	    =ELS_TYPE_STRING;
const int ELS_API_TYPE_UNIT	    =ELS_TYPE_UNIT;
const int ELS_API_TYPE_FUNCTION	=ELS_TYPE_FUNCTION;
const int ELS_API_TYPE_MARK       =ELS_TYPE_MARK;
const int ELS_API_TYPE_PTR        =ELS_TYPE_PTR;

#define STR2(x) #x
#define STR(x) STR2(x)

const char ELS_VERSION[]   = ELS_CORE_VERSION;
const char ELS_COPYRIGHT[] = "MIT LICENSE  2022-2023 chen-chaochen";
const char ELS_BUILD[128]= "Build ("__DATE__","__TIME__")  [GCC " STR(__GNUC__) "."STR(__GNUC_MINOR__)"."STR(__GNUC_PATCHLEVEL__)"]";
char els_buff_tmp[ELS_BUFF_TMP_SIZE];

#ifdef ELS_CONF_OS_LINUX
    const char ELS_BRANCH[]="Linux Branch";
#endif
#ifdef ELS_CONF_OS_WINDOWS
    const char ELS_BRANCH[]="Windows Branch";
#endif
#ifdef ELS_CONF_OS_RTTHREAD
    const char ELS_BRANCH[]="Rt-thread Branch";
#endif

const char *ELS_TYPESYSTEM[]={
    "byte",
    "null",
    "number",
    "string",
    "unit",
    "function",
    "mark",
    "pointer"
};


int losu_printf(const char * format){
    #ifdef ELS_CONF_CHAR_GBK
        char*  tmp = vm_win_togbk(format);
        printf("%s\n",tmp);
        free(tmp);
        return 0;
    #else
        printf("%s\n",format);
        return 0;
    #endif
}

int       els_argc;
char**    els_argv;

#ifdef ELS_CONF_OS_LINUX
int els_getargv (els_VmObj *l){
    if (obj_type(l,arg_get(l,1))!=ELS_TYPE_NUMBER) 
        arg_return(l,obj_newnull(l));
    else{
        int n = (int) obj_tonum(l,arg_get(l,1));
        if (n > els_argc-2) 
            arg_returnnull(l);
        else 
            arg_return(l,obj_newstr(l,els_argv[n+1]));
    }
    return 1;
}
int els_getargc (els_VmObj *l){
    arg_return(l,obj_newnum(l,els_argc-2));
    return 1;
}
#endif


#ifdef ELS_CONF_OS_WINDOWS
int els_getargv (els_VmObj *l){
    if (obj_type(l,arg_get(l,1))!=ELS_TYPE_NUMBER) 
        arg_return(l,obj_newnull(l));
    else{
        int n = (int) obj_tonum(l,arg_get(l,1));
        if (n > els_argc-2) 
           arg_returnnull(l);
        else {
            char* tmp = vm_win_toutf8((const char*)els_argv[n+1]);
            arg_return(l,obj_newstr(l,tmp));
            free(tmp);
        }
    }
    return 1;
}
int els_getargc (els_VmObj *l){
    arg_return(l,obj_newnum(l,els_argc-2));
    return 1;
}
#endif


#ifdef ELS_CONF_CHAR_GBK
#include<windows.h>
char *vm_win_togbk(const char *str)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
    wchar_t *strUnicode = (wchar_t *)malloc(len * sizeof(wchar_t *));
    wmemset(strUnicode, 0, len);
    MultiByteToWideChar(CP_UTF8, 0, str, -1, strUnicode, len);
    len = WideCharToMultiByte(CP_ACP, 0, strUnicode, -1, NULL, 0, NULL, NULL);
    char *strGbk = (char *)malloc(len * sizeof(char *));
    memset(strGbk, 0, len);
    WideCharToMultiByte(CP_ACP, 0, strUnicode, -1, strGbk, len, NULL, NULL);
    free(strUnicode);
    strUnicode = NULL;
    return strGbk;
}
char *vm_win_toutf8(const char *str)
{
    int len = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
    wchar_t *strUnicode = (wchar_t *)malloc(len * sizeof(wchar_t *));
    wmemset(strUnicode, 0, len);
    MultiByteToWideChar(CP_ACP, 0, str, -1, strUnicode, len);
    len = WideCharToMultiByte(CP_UTF8, 0, strUnicode, -1, NULL, 0, NULL, NULL);
    char *strutf8 = (char *)malloc(len * sizeof(char *));
    memset(strutf8, 0, len);
    WideCharToMultiByte(CP_UTF8, 0, strUnicode, -1, strutf8, len, NULL, NULL);
    free(strUnicode);
    strUnicode = NULL;
    return strutf8;
}
char * obj_toUTF8(els_VmObj*vm,char* str)
{
    char *s = vm_win_toutf8(str);
    LosuObj o = obj_newstr(vm,s);
    free(s);
    return o.value.ts->str;
}
char * obj_toGBK(els_VmObj*vm,char* str)
{
    char *s = vm_win_togbk(str);
    LosuObj o = obj_newstr(vm,s);
    free(s);
    return o.value.ts->str;
}




#endif


int ElsNewApp(int (*func)(els_VmObj*,const char* s),char *s,int stacksize,size_t gcmax,int argc,const char** argv)
{   
    int i = 0;
    els_VmObj *vm = vm_create(stacksize);
    if(vm==NULL)
        return ELS_ERRORBACK_OVER;
    vm_setGC(vm,gcmax);
    els_lib_init(vm);
    #ifdef ELS_CONF_TOKEN_EN 
        vm_register(vm,"argc",els_getargc);
        vm_register(vm,"argv",els_getargv);
    #endif
    i = func(vm,s);
    vm_close(vm);
    return i;
}

