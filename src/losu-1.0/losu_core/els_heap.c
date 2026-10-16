#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#include "els.h"
#include "els_heap.h"
#include "els_gc.h"
#include "els_mem.h"
#include "els_object.h"
#include "els_parser.h"
#include "els_vmhost.h"
#include "els_string.h"
#include "els_unit.h"
#include "els_vmcore.h"


#define ELS_MINSTACK	16

void els_Heap_init(els_VmObj *L, int stacksize)
{
    L->stack = els_Mem_newvector(L, stacksize, StackObj);
    L->nblocks += stacksize * sizeof(StackObj);
    L->stack_last = L->stack + (stacksize - 1);
    L->stacksize = stacksize;
    L->Cbase = L->top = L->stack;
}

void els_Heap_checkstack(els_VmObj *L, int n)
{
    if (L->stack_last - L->top <= n)
        vm_error(L, "虚拟机堆栈溢出",ELS_ERRORBACK_HEAP);
}

void restore_stack_limit(els_VmObj *L)
{
    if (L->top - L->stack < L->stacksize - 1)
        L->stack_last = L->stack + (L->stacksize - 1);
}

void els_Heap_adjusttop(els_VmObj *L, StackObj* base, int extra)
{
    int diff = extra - (L->top - base);
    if (diff <= 0)
        L->top = base + extra;
    else
    {
        els_Heap_checkstack(L, diff);
        while (diff--)
            ttype(L->top++) = ELS_TYPE_NULL;
    }
}

void els_Heap_openstack(els_VmObj *L, StackObj* pos)
{
    int i = L->top - pos;
    while (i--)
        pos[i + 1] = pos[i];
    incr_top(L);
}

StackObj* callC_csfunciotn(els_VmObj *L, const struct ElsCfunc *cl, StackObj* base)
{
    int nup = cl->nupvalues;
    StackObj* old_Cbase = L->Cbase;
    int n;
    L->Cbase = base;
    els_Heap_checkstack(L, nup + ELS_MINSTACK);
    for (n = 0; n < nup; n++)
        *(L->top++) = cl->upvalue[n];
    n = ((*cl->f.c)(L));
    L->Cbase = old_Cbase;
    return L->top - n;
}

int els_Heap_call(els_VmObj *L, StackObj* func, int nResults)
{
    StackObj* firstResult;
    CallInfo ci;
    ElsCfunc *cl;
    // if(ttype(func)==ELS_TYPE_UNIT){
    //         *func = *obj_indexunit(L,*func,obj_newstr(L,"class"));
    // }
    if (ttype(func) != ELS_TYPE_FUNCTION)
    {

        vm_error(L, "被调用的变量不是函数",ELS_ERRORBACK_RUN);
        return 1;
    }
    cl = clvalue(func);
    ci.func = cl;
    infovalue(func) = &ci;
    ttype(func) = ELS_TYPE_MARK;
    firstResult = (cl->isC ? callC_csfunciotn(L, cl, func + 1) : els_Vmcore_execute(L, cl, func + 1));
    if (nResults == ELS_MULTRET)
    {
        while (firstResult < L->top)
            *func++ = *firstResult++;
        L->top = func;
    }
    else
    {
        for (; nResults > 0 && firstResult < L->top; nResults--)
            *func++ = *firstResult++;
        L->top = func;
        for (; nResults > 0; nResults--)
        {
            ttype(L->top) = ELS_TYPE_NULL;
            incr_top(L);
        }
    }
#ifdef ELS_CONF_GC_ENABLE
    els_Gc_checkGC(L);
#endif
    return 0;
}


struct CallS
{
    StackObj* func;
    int nresults;
};

void f_call(els_VmObj *L, void *ud)
{
    struct CallS *c = (struct CallS *)ud;
    els_Heap_call(L, c->func, c->nresults);
}

ELS_API int els_call(els_VmObj *L, int nargs, int nresults)
{
    StackObj* func = L->top - (nargs + 1);
    struct CallS c;
    int status;
    c.func = func;
    c.nresults = nresults;

    
    StackObj* oldCbase = L->Cbase;
    StackObj* oldtop = L->top;
    struct els_longjmp lj;
    lj.status = 0;
    lj.previous = L->errorJmp;
    L->errorJmp = &lj;
    if (setjmp(lj.b) == 0)
        f_call(L, &c);
    else
    {
        L->Cbase = oldCbase;
        L->top = oldtop;
        restore_stack_limit(L);
    }
    L->errorJmp = lj.previous;
    status = lj.status;

    L->top = func;
    return status;
}

struct ParserS
{
    vm_iobuff *z;
    int bin;
};

void f_parser(els_VmObj *L, void *ud)
{
    struct ParserS *p = (struct ParserS *)ud;
    els_func_code *tf = els_parser_parse(L, p->z);
    els_Vmcore_L_csfunciotn(L, tf, 0);
}

int protectedparser(els_VmObj *L, vm_iobuff *z, int bin)
{
    struct ParserS p;
    unsigned long old_blocks;
    int status;
    p.z = z;
    p.bin = bin;
#ifdef ELS_CONF_GC_ENABLE
    els_Gc_checkGC(L);
#endif
    old_blocks = L->nblocks;

    StackObj* oldCbase = L->Cbase;
    StackObj* oldtop = L->top;
    struct els_longjmp lj;
    lj.status = 0;
    lj.previous = L->errorJmp;
    L->errorJmp = &lj;
    if (setjmp(lj.b) == 0)
        f_parser(L, &p);
    else
    {
        L->Cbase = oldCbase;
        L->top = oldtop;
        restore_stack_limit(L);
    }
    L->errorJmp = lj.previous;
    status = lj.status;

    if (status == 0)
    {
        L->GCnowmax += (L->nblocks - old_blocks);
    }
    else if (status == ELS_ERRORBACK_RUN)
        status = ELS_ERRORBACK_SYNTAX;
    return status;
}

int parse_file(els_VmObj *L, const char *filename)
{
    vm_iobuff z;
    int status;
    FILE *f = fopen(filename, "r");
    if (f == NULL)
    {
        char tmp[256];
        sprintf(tmp, "警告：找不到文件 `%s'", filename);
        losu_printf(tmp);
        return ELS_ERRORBACK_FILE;
    }
    els_vmio_Fopen(&z, f, filename);
    status = protectedparser(L, &z, 0);
    if (f != stdin)
        fclose(f);
    return status;
}

int parse_buffer(els_VmObj *L, const char *buff, size_t size, const char *name)
{
    vm_iobuff z;
    els_vmio_mopen(&z, buff, size, name);
    return protectedparser(L, &z, 0);
}


void els_Heap_breakrun(els_VmObj *L, int errcode)
{
    if (L->errorJmp)
    {
        L->errorJmp->status = errcode;
        longjmp(L->errorJmp->b, 1);
    }
    else
    {
        losu_printf("无法维持，系统已崩溃\n");
        exit(ELS_ERRORBACK_OVER);
    }
}


