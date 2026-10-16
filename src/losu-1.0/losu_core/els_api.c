#include <string.h>

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

#define GCscale(x) ((int)((x) >> 10))
#define GCunscale(x) ((unsigned long)(x) << 10)
#define api_incr_top(L) incr_top(L)


extern int parse_file(els_VmObj *L, const char *filename);
extern int parse_buffer(els_VmObj *L, const char *buff, size_t size, const char *name);

ELS_API els_VmObj *els_createvm(int stacksize);
ELS_API void els_closevm(els_VmObj *L);
char *vm_win_togbk(const char *str);
ELS_API int els_call(els_VmObj *L, int nargs, int nresults);
extern ElsCfunc *els_Vmcore_csfunction(els_VmObj *L, int issues_num);

static int currentline(StackObj *f);
static int currentpc(StackObj *f);
static int trygetline(int *lineinfo, int pc, int refline, int *prefi);


static int trygetline(int *lineinfo, int pc, int refline, int *prefi)
{
    int refi;
    if (lineinfo == NULL || pc == -1)
        return -1; /* no line info */
    refi = prefi ? *prefi : 0;
    if (lineinfo[refi] < 0)
        refline += -lineinfo[refi++];
    while (lineinfo[refi] > pc)
    {
        refline--;
        refi--;
        if (lineinfo[refi] < 0)
            refline -= -lineinfo[refi--];
    }
    for (;;)
    {
        int nextline = refline + 1;
        int nextref = refi + 1;
        if (lineinfo[nextref] < 0)
            nextline += -lineinfo[nextref++];
        if (lineinfo[nextref] > pc)
            break;
        refline = nextline;
        refi = nextref;
    }
    if (prefi)
        *prefi = refi;
    return refline;
}

static int currentpc(StackObj *f)
{
    CallInfo *ci = f->value.i;
    if (ci->pc)
        return (*ci->pc - ci->func->f.l->code) - 1;
    else
        return -1;
}

static int currentline(StackObj *f)
{
    if (!(f && f->ttype == ELS_TYPE_MARK && !f->value.i->func->isC))
        return currentline(f - 1);
    CallInfo *ci = f->value.i;
    int *lineinfo = ci->func->f.l->lineinfo;
    return trygetline(lineinfo, currentpc(f), 1, NULL);
}




/*Vm api*/

ELS_API els_VmObj *vm_create(int stacksize);
ELS_API els_VmObj* vm_fork(els_VmObj*vm,int size);
ELS_API void vm_close(els_VmObj *vm);



ELS_API void vm_stop(els_VmObj *vm)
{
    els_Heap_breakrun(vm, 0);
}

ELS_API void vm_error(els_VmObj *vm, const char *s,int errnum)
{
#ifdef ELS_CONF_CHAR_GBK
    char tmp[64];
    sprintf(tmp,"运行时错误: \t%s\n在第\t%d\t行\n",s,currentline(vm->top - 1));
    printf("%s",obj_toGBK(vm,tmp));
    // printf("%s: \t%s\n在第\t%d\t行\n", obj_toGBK(vm,"运行时错误"),obj_toGBK(vm,s), currentline(vm->top - 1));
#else
    printf("运行时错误: \t%s\n在第\t%d\t行\n", s, currentline(vm->top - 1));
#endif
    els_Heap_breakrun(vm, errnum);
}
ELS_API int vm_dofile(els_VmObj *vm, const char *filename)
{
    int i;
    if (!(i = parse_file(vm, filename)))
        return els_call(vm, 0, ELS_MULTRET);
    return i;
}
ELS_API int vm_dostring(els_VmObj *vm, const char *str)
{
    int i;
    if (!(i = parse_buffer(vm, str, strlen(str), str)))
        return els_call(vm, 0, ELS_MULTRET);
    return i;
}
ELS_API LosuObj *vm_getval(els_VmObj *vm, const char *name)
{
    return (LosuObj *)els_Unit_getstr(vm->globalenv, els_string_new(vm, name));
}
ELS_API void vm_setval(els_VmObj *vm, const char *name, LosuObj o)
{
    const StackObj *oldvalue = els_Unit_getstr(vm->globalenv, els_string_new(vm, name));
    if (oldvalue != &els_Object_nullobject)
    {
        *(StackObj *)oldvalue = o;
    }
    else
    {
        StackObj key;
        ttype(&key) = ELS_TYPE_STRING;
        tsvalue(&key) = els_string_new(vm, name);
        *els_Unit_set(vm, vm->globalenv, &key) = o;
    }
}
ELS_API void vm_register(els_VmObj *vm, const char *name, els_C_API_function f)
{
    LosuObj tmp;
    tmp.ttype = ELS_TYPE_FUNCTION;
    tmp.value.cl = els_Vmcore_csfunction(vm, 0);
    tmp.value.cl->isC = 1;
    tmp.value.cl->f.c = f;
    const StackObj *oldvalue = els_Unit_getstr(vm->globalenv, els_string_new(vm, name));
    if (oldvalue != &els_Object_nullobject)
    {
        *(StackObj *)oldvalue = tmp;
    }
    else
    {
        StackObj key;
        ttype(&key) = ELS_TYPE_STRING;
        tsvalue(&key) = els_string_new(vm, name);
        *els_Unit_set(vm, vm->globalenv, &key) = tmp;
    }
}

ELS_API void vm_setGC(els_VmObj*vm,unsigned long size){
    vm->GCnowmax = size;
    vm->GCMAX = size;
}


ELS_API unsigned long   vm_getmem(els_VmObj*vm){
    return vm->nblocks;
}


/* arg */
ELS_API LosuObj *arg_get(els_VmObj *vm, int i)
{
    if (i >= 0)
    {
        StackObj *o = vm->Cbase + (i - 1);
        if (o >= vm->top)
            o->ttype = ELS_TYPE_NULL;
        return o;
    }
    else
        return vm->top + i;
}
ELS_API void arg_return(els_VmObj *vm, LosuObj o)
{
    *(vm->top) = (o);
    if (vm->top == vm->stack_last)
        els_Heap_checkstack(vm, 1);
    vm->top++;
}
ELS_API int arg_num(els_VmObj *vm)
{
    return (int)(vm->top - vm->Cbase);
}



ELS_API Number arg_getnum(els_VmObj *vm, int i)
{
    return obj_tonum(vm, arg_get(vm, i));
}
ELS_API const char *arg_getstr(els_VmObj *vm, int i)
{
    return obj_tostr(vm, arg_get(vm, i));
}
ELS_API char *arg_getptr(els_VmObj *vm, int i)
{
    return obj_toptr(vm, arg_get(vm, i));
}
ELS_API VmCapi arg_getfunc(els_VmObj *vm, int i)
{
    return obj_tofunction(vm, arg_get(vm, i));
}
ELS_API const char* arg_getbyte(els_VmObj*vm,int i)
{
    return obj_tobyte(vm,arg_get(vm,i));
}
ELS_API int arg_gettype(els_VmObj *vm, int i)
{
    return obj_type(vm, arg_get(vm, i));
}



ELS_API void arg_returnnum(els_VmObj *vm, Number i)
{
    nvalue(vm->top) = i;
    ttype(vm->top) = ELS_TYPE_NUMBER;
    if (vm->top == vm->stack_last)
        els_Heap_checkstack(vm, 1);
    vm->top++;
}
ELS_API void arg_returnstr(els_VmObj *vm, const char *s)
{
    if (s == NULL)
        ttype(vm->top) = ELS_TYPE_STRING;
    else
    {
        ttype(vm->top) = ELS_TYPE_STRING;
        tsvalue(vm->top) = els_string_newlstr(vm, s, strlen(s));
    }
    if (vm->top == vm->stack_last)
        els_Heap_checkstack(vm, 1);
    vm->top++;
}
// ELS_API void arg_returnfunc(els_VmObj *vm, VmCapi f)
// {
//     vm->top->value.cl->f.c = f;
//     ttype(vm->top) = ELS_TYPE_FUNCTION;
//     if (vm->top == vm->stack_last)
//         els_Heap_checkstack(vm, 1);
//     vm->top++;
// }
ELS_API void arg_returnfunc(els_VmObj *vm, VmCapi f)
{
    arg_return(vm,obj_newfunction(vm,f));
}

ELS_API void arg_returnptr(els_VmObj *vm, char *p)
{
    vm->top->value.ptr = p;
    ttype(vm->top) = ELS_TYPE_PTR;
    if (vm->top == vm->stack_last)
        els_Heap_checkstack(vm, 1);
    vm->top++;
}
ELS_API void arg_returnnull(els_VmObj *vm)
{
    ttype(vm->top) = ELS_TYPE_NULL;
    if (vm->top == vm->stack_last)
        els_Heap_checkstack(vm, 1);
    vm->top++;
}
ELS_API void arg_returnbyte(els_VmObj*vm,char* p,size_t len)
{
    arg_return(vm,obj_newbyte(vm,p,len));
}


/* obj */
ELS_API const char *obj_tostr(els_VmObj *vm, LosuObj *o)
{
    return (o == NULL || tostring(vm, o)) ? "" : svalue(o);
}
ELS_API Number obj_tonum(els_VmObj *vm, LosuObj *o)
{
    return (o == NULL || tonumber(o)) ? 0 : nvalue(o);
}
ELS_API char *obj_toptr(els_VmObj *vm, LosuObj *o)
{
    if (ttype(o) == ELS_TYPE_PTR)
        return o->value.ptr;
    return NULL;
}
ELS_API VmCapi obj_tofunction(els_VmObj *vm, LosuObj *o)
{
    if (ttype(o) == ELS_TYPE_FUNCTION)
        return o->value.cl->f.c;
    return NULL;
}
ELS_API int obj_type(els_VmObj *vm, LosuObj *o)
{
    return ttype(o);
}
ELS_API const char* obj_tobyte(els_VmObj *vm,LosuObj *o)
{
    return ttype(o)==ELS_TYPE_BYTE ? svalue(o) : NULL;
}

ELS_API LosuObj obj_newstr(els_VmObj *vm, char *str)
{
    LosuObj tmp;
    tmp.ttype = ELS_TYPE_STRING;
    tmp.value.ts = els_string_new(vm, str);
    return tmp;
}
ELS_API LosuObj obj_newnum(els_VmObj *vm, Number n)
{
    LosuObj tmp;
    tmp.ttype = ELS_TYPE_NUMBER;
    tmp.value.n = n;
    return tmp;
}
ELS_API LosuObj obj_newfunction(els_VmObj *vm,VmCapi f)
{
    LosuObj tmp;
    tmp.ttype = ELS_TYPE_FUNCTION;
    tmp.value.cl = els_Vmcore_csfunction(vm, 0);
    tmp.value.cl->isC = 1;
    tmp.value.cl->f.c = f;
    return tmp;
}
ELS_API LosuObj obj_newnull(els_VmObj *vm)
{
    return els_Object_nullobject;
}
ELS_API LosuObj obj_newunit(els_VmObj *vm)
{
    LosuObj tmp;
    tmp.ttype = ELS_TYPE_UNIT;
    hvalue(&tmp) = els_Unit_new(vm, 0);
    hvalue((&tmp))->htag = ELS_TYPE_UNIT;
    return tmp;
}
ELS_API LosuObj obj_newptr(els_VmObj *vm, char *p)
{
    LosuObj tmp;
    tmp.ttype = ELS_TYPE_PTR;
    tmp.value.ptr = p;
    return tmp;
}
ELS_API LosuObj obj_newbyte(els_VmObj*vm,char*p,size_t len)
{
    LosuObj o;  
    o.value.ts = els_string_newlstr(vm,p,len);
    o.ttype = ELS_TYPE_BYTE;
    return o;

}

ELS_API LosuObj *obj_indexunit(els_VmObj *vm, LosuObj unit, LosuObj key)
{
    if (unit.ttype == ELS_TYPE_UNIT)
        return (LosuObj *)els_Unit_get(vm, hvalue(&unit), &key);
    return NULL;
}
ELS_API void obj_setunit(els_VmObj *vm, LosuObj unit, LosuObj key, LosuObj value)
{
    if (unit.ttype == ELS_TYPE_UNIT)
        *els_Unit_set(vm, hvalue(&unit), &key) = value;
    return;
}
ELS_API LosuObj* obj_indexunitbynum(els_VmObj*vm,LosuObj unit,Number i){
    if (unit.ttype == ELS_TYPE_UNIT)
        return (LosuObj *)els_Unit_getnum(hvalue(&unit),i);
    return NULL;
}
ELS_API LosuObj* obj_indexunitbystr(els_VmObj*vm,LosuObj unit,char* s){
    LosuObj key = obj_newstr(vm,s);
    if (unit.ttype == ELS_TYPE_UNIT)
        return (LosuObj *)els_Unit_get(vm, hvalue(&unit), &key);
    return NULL;
}
ELS_API void obj_setunitbynum(els_VmObj*vm,LosuObj unit,Number i ,LosuObj value){
    LosuObj key = obj_newnum(vm,i);
    if (unit.ttype == ELS_TYPE_UNIT)
        *els_Unit_set(vm, hvalue(&unit), &key) = value;
    return;
}
ELS_API void obj_setunitbystr(els_VmObj*vm,LosuObj unit,char* s ,LosuObj value){
    LosuObj key = obj_newstr(vm,s);
    if (unit.ttype == ELS_TYPE_UNIT)
        *els_Unit_set(vm, hvalue(&unit), &key) = value;
    return;
}


ELS_API void            stack_push (els_VmObj*vm,LosuObj o){
    *(vm->top)=o;
    if (vm->top == vm->stack_last)
        els_Heap_checkstack(vm, 1);
    vm->top++;
}
ELS_API void            stack_pop  (els_VmObj*vm,int i){
    vm->top-=i;
}
ELS_API void            stack_call (els_VmObj*vm,int argnum,int resnum){
    els_Heap_call(vm,vm->top - (argnum+1) ,resnum);
}

ELS_API Node *          obj_unit_first(els_VmObj*vm,LosuObj unit){
    if(obj_type(vm,&unit)!=ELS_TYPE_UNIT)
        return NULL;
    return els_Unit_next(vm, hvalue(&unit),&els_Object_nullobject);
}
ELS_API Node           obj_unit_location(els_VmObj*vm,LosuObj unit,LosuObj key){
    Node n;
    if(obj_type(vm,&unit)!=ELS_TYPE_UNIT){
        n.val = obj_newnull(vm);
        n.key = obj_newnull(vm);
        return n;
    }
    LosuObj* v = (LosuObj*)els_Unit_get(vm,hvalue(&unit),&key);
    if(v==NULL)
        n.val = obj_newnull(vm);
    else
        n.val = *v;
    n.key = key;
    return n;
}
ELS_API Node *          obj_unit_next(els_VmObj*vm,LosuObj unit,Node*n){
    if(obj_type(vm,&unit)!=ELS_TYPE_UNIT)
        return NULL;
    
    return els_Unit_next(vm,hvalue(&unit),(const LosuObj*)(&n->key));

}
ELS_API LosuObj         obj_unit_nodekey(els_VmObj*vm,Node* n){
    return n->key;
}
ELS_API LosuObj         obj_unit_nodevalue(els_VmObj*vm,Node* n){
    return n->val;
}