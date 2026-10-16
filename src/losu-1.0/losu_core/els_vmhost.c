#include <setjmp.h>
#include <stdio.h>

#include "els.h"
#include "els_heap.h"
#include "els_gc.h"
#include "els_lex.h"
#include "els_mem.h"
#include "els_vmhost.h"
#include "els_string.h"
#include "els_unit.h"

#define api_incr_top(L) incr_top(L)

static void els_newunit(els_VmObj *L)
{
    hvalue(L->top) = els_Unit_new(L, 0);
    ttype(L->top) = ELS_TYPE_UNIT;
    api_incr_top(L);
}
static int els_heap_ref(els_VmObj *L, int lock)
{
    int ref;
    if (ttype(L->top - 1) == ELS_TYPE_NULL)
        ref = ELS_REFNULL;
    else
    {
        if (L->refFree != NONEXT)
        {
            ref = L->refFree;
            L->refFree = L->refArray[ref].st;
        }
        else
        {
            els_Mem_growvector(L, L->refArray, L->refSize, 1, struct Ref, "初始化单元溢出", MAX_INT);
            L->nblocks += sizeof(struct Ref);
            ref = L->refSize++;
        }
        L->refArray[ref].o = *(L->top - 1);
        L->refArray[ref].st = lock ? LOCK : HOLD;
    }
    L->top--;
    return ref;
}




static void f_elsopen(els_VmObj *L, void *ud)
{
    int stacksize = *(int *)ud;
    if (stacksize == 0)
        stacksize = ELS_VM_STACK_SIZE;
    if(L->mainthread==L)
        L->globalenv = els_Unit_new(L, 0);
    else
        L->globalenv = L->mainthread->globalenv;
    els_Heap_init(L, stacksize);
    els_string_init(L);
    els_lexer_init(L);
    els_newunit(L);
    els_heap_ref(L, 1);
}

void restore_stack_limit(els_VmObj *L);
ELS_API els_VmObj *vm_create(int stacksize)
{
    els_VmObj *L = els_Mem_new(NULL, els_VmObj);
    if (L == NULL)
        return NULL;
    L->mainthread = L;
    L->stack = NULL;
    L->strpool.size = 0;
    L->strpool.nuse = 0;
    L->strpool.hash = NULL;
    L->BufferTmp = NULL;
    L->BufferTmpSize = 0;
    L->rootcodeir = NULL;
    L->rootcl = NULL;
    L->rootunit = NULL;
    L->refArray = NULL;
    L->refSize = 0;
    L->refFree = NONEXT;
    L->nblocks = sizeof(els_VmObj);
    L->GCnowmax = MAX_INT;
    L->errorJmp = NULL;
    L->GCMAX = 0;
    StackObj* oldCbase = L->Cbase;
    StackObj* oldtop = L->top;
    struct els_longjmp lj;
    lj.status = 0;
    lj.previous = L->errorJmp;
    L->errorJmp = &lj;
    if (setjmp(lj.b) == 0)
        f_elsopen(L, &stacksize);
    else
    {
        L->Cbase = oldCbase;
        L->top = oldtop;
        restore_stack_limit(L);
    }
    L->errorJmp = lj.previous;
    int status = lj.status;

    if (status != 0)
    {
        vm_close(L);
        return NULL;
    }


    L->GCnowmax = 0;
    return L;
}

ELS_API els_VmObj* vm_fork(els_VmObj*vm,int stacksize){
    els_VmObj *L = els_Mem_new(NULL, els_VmObj);
    if (L == NULL)
        return NULL;
    L->mainthread = vm;
    L->stack = NULL;
    L->strpool.size= 0;
    L->strpool.nuse = 0;
    L->strpool.hash = NULL;
    L->BufferTmp = NULL;
    L->BufferTmpSize = 0;
    L->rootcodeir = NULL;
    L->rootcl = NULL;
    L->rootunit = NULL;
    L->refArray = NULL;
    L->refSize = 0;
    L->refFree = NONEXT;
    L->nblocks = vm->nblocks;
    L->GCnowmax = MAX_INT;
    L->errorJmp = NULL;
    L->GCMAX = 0;
    StackObj* oldCbase = L->Cbase;
    StackObj* oldtop = L->top;
    struct els_longjmp lj;
    lj.status = 0;
    lj.previous = L->errorJmp;
    L->errorJmp = &lj;
    if (setjmp(lj.b) == 0)
        f_elsopen(L, &stacksize);
    else
    {
        L->Cbase = oldCbase;
        L->top = oldtop;
        restore_stack_limit(L);
    }
    L->errorJmp = lj.previous;
    int status = lj.status;

    if (status != 0)
    {
        vm_close(L);
        return NULL;
    }


    L->GCnowmax = 0;
    return L;
}

ELS_API void vm_close(els_VmObj *L)
{
    if(L->mainthread!=L)
        // L->globalenv = els_Unit_new(L, 0);
        L->globalenv = NULL;
#ifdef ELS_CONF_GC_ENABLE
    els_Gc_collect(L, 1);
#endif
    els_string_freeall(L);
    if (L->stack)
        L->nblocks -= (L->stack_last - L->stack + 1) * sizeof(StackObj);
    els_Mem_free(L, L->stack);
    L->nblocks -= (L->refSize) * sizeof(struct Ref);
    els_Mem_free(L, L->refArray);
    L->nblocks -= (L->BufferTmpSize) * sizeof(char);
    els_Mem_free(L, L->BufferTmp);
    els_Mem_free(L, L);
}
