#include "stdthread.h"
#include "els_vmhost.h"
#include "els_heap.h"
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>


static void ElsLib_lsthread_newthread(els_VmObj *vm){
    els_Heap_call(vm,vm->stack,-1);
    vm_close(vm);
}
int ELSAPI_stdthread_stdthread_exit(els_VmObj* vm)
{
// 这里填写API函数的 C 实现
    pthread_exit(NULL);
    return 0;
}
int ELSAPI_stdthread_stdthread_self(els_VmObj* vm)
{
// 这里填写API函数的 C 实现
    arg_returnnum(vm,(Number)((unsigned int)(pthread_self())));
    return 1;
}
int ELSAPI_stdthread_stdthread_join(els_VmObj* vm)
{
// 这里填写API函数的 C 实现
    pthread_t pt = (pthread_t)obj_tonum(vm,obj_indexunit(vm, *arg_get(vm,1) , obj_newstr(vm,"thread_t") ));
    arg_returnnum(vm,pthread_join(pt,NULL));
    return 1;
}
int ELSAPI_stdthread_stdthread_create(els_VmObj* vm)
{
// 这里填写API函数的 C 实现
    pthread_t _t;
    pthread_t *t=&_t;
    
    obj_setunit(vm,*arg_get(vm,1),obj_newstr(vm,"thread_t"),obj_newptr(vm,(char*)(t)));
    #ifdef ELS_CONF_TOKEN_EN 
        obj_setunit(vm,*arg_get(vm,1),obj_newstr(vm,"argn"),obj_newnum(vm,arg_num(vm)-2));
    #endif

    #ifdef ELS_CONF_TOKEN_CN 
        obj_setunit(vm,*arg_get(vm,1),obj_newstr(vm,"参数个数"),obj_newnum(vm,arg_num(vm)-2));
    #endif
    els_VmObj* n = vm_fork(vm,1024);
    *(n->stack) = *arg_get(vm,2);
    if(obj_type(vm,arg_get(vm,2))!=ELS_API_TYPE_FUNCTION){
        printf("PThread Error : Not Function,create failed!\n");
        return 0;
    }
    for(int i=2;i<arg_num(vm);i++)
        *(n->stack+ i-1) = *arg_get(vm,i+1);
    n->top  =n->stack + arg_num(vm) ;
    arg_returnnum(vm,pthread_create(t,NULL,(void*)(*ElsLib_lsthread_newthread),n));
    return 1;
}
int ELSAPI_stdthread_stdthread_cancel(els_VmObj* vm)
{
// 这里填写API函数的 C 实现
    pthread_t pt = (pthread_t)obj_tonum(vm,obj_indexunit(vm,*arg_get(vm,2),obj_newstr(vm,"thread_t") ));
    arg_returnnum(vm,pthread_cancel(pt));
    return 1;
}
int ELSAPI_stdthread_stdthread_detach(els_VmObj* vm)
{
// 这里填写API函数的 C 实现
    pthread_t pt = (pthread_t)obj_tonum(vm,obj_indexunit(vm,*arg_get(vm,1),obj_newstr(vm,"thread_t") ));
    arg_returnnum(vm,pthread_detach(pt));
    return 1;
}
