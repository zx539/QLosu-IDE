#include "els.h"

#include "els_heap.h"
#include "els_func.h"
#include "els_gc.h"
#include "els_mem.h"
#include "els_object.h"
#include "els_vmhost.h"
#include "els_string.h"
#include "els_unit.h"

#define MINBUFFER 256
#define strmark(s)            \
    {                         \
        if ((s)->marked == 0) \
            (s)->marked = 1;  \
    }
#define VALIDLINK(L, st, n) (NONEXT <= (st) && (st) < (n))

typedef struct GcObj
{
    Hash *tmark;
    ElsCfunc *cmark;
} GcObj;

static void markobject(GcObj *st, StackObj *o);

static void codeirmark(els_func_code *f)
{
    if (!f->marked)
    {
        int i;
        f->marked = 1;
        strmark(f->source);
        for (i = 0; i < f->nkstr; i++)
            strmark(f->kstr[i]);
        for (i = 0; i < f->nkcodeir; i++)
            codeirmark(f->kcodeir[i]);
        for (i = 0; i < f->nlocvars; i++)
            strmark(f->locvars[i].varname);
    }
}

static void markstack(els_VmObj *L, GcObj *st)
{
    StackObj* o;
    for (o = L->stack; o < L->top; o++)
        markobject(st, o);
}

static void marklock(els_VmObj *L, GcObj *st)
{
    int i;
    for (i = 0; i < L->refSize; i++)
    {
        if (L->refArray[i].st == LOCK)
            markobject(st, &L->refArray[i].o);
    }
}

static void mark_csfunciotn(GcObj *st, ElsCfunc *cl)
{
    if (!ismarked(cl))
    {
        if (!cl->isC)
            codeirmark(cl->f.l);
        cl->mark = st->cmark;
        st->cmark = cl;
    }
}

static void markobject(GcObj *st, StackObj *o)
{
    switch (ttype(o))
    {
    case ELS_TYPE_STRING:
    case ELS_TYPE_BYTE:
        strmark(tsvalue(o));
        break;
    case ELS_TYPE_MARK:
        mark_csfunciotn(st, infovalue(o)->func);
        break;
    case ELS_TYPE_FUNCTION:
        mark_csfunciotn(st, clvalue(o));
        break;
    case ELS_TYPE_UNIT:
    {
        if (!ismarked(hvalue(o)))
        {
            hvalue(o)->mark = st->tmark;
            st->tmark = hvalue(o);
        }
        break;
    }
    default:
        break;
    }
}

static void markall(els_VmObj *L)
{
    GcObj st;
    st.cmark = NULL;
    st.tmark = L->globalenv;
    L->globalenv->mark = NULL;
    markstack(L, &st);
    marklock(L, &st);
    while (1)
    {
        if (st.cmark)
        {
            int i;
            ElsCfunc *f = st.cmark;
            st.cmark = f->mark;
            for (i = 0; i < f->nupvalues; i++)
                markobject(&st, &f->upvalue[i]);
        }
        else if (st.tmark)
        {
            int i;
            Hash *h = st.tmark;
            st.tmark = h->mark;
            for (i = 0; i < h->size; i++)
            {
                Node *n = node(h, i);
                if (ttype(key(n)) != ELS_TYPE_NULL)
                {
                    if (ttype(val(n)) == ELS_TYPE_NULL)
                        els_Unit_remove(h, key(n));
                    markobject(&st, &n->key);
                    markobject(&st, &n->val);
                }
            }
        }
        else
            break;
    }
}

static int hasmark(const StackObj *o)
{

    switch (o->ttype)
    {
    case ELS_TYPE_STRING:
    case ELS_TYPE_BYTE:
        return tsvalue(o)->marked;
    case ELS_TYPE_UNIT:
        return ismarked(hvalue(o));
    case ELS_TYPE_FUNCTION:
        return ismarked(clvalue(o));
    default:
        return 1;
    }
}

static void invalidaterefs(els_VmObj *L)
{
    int n = L->refSize;
    int i;
    for (i = 0; i < n; i++)
    {
        struct Ref *r = &L->refArray[i];
        if (r->st == HOLD && !hasmark(&r->o))
            r->st = COLLECTED;
    }
}

static void collectcodeir(els_VmObj *L)
{
    els_func_code **p = &L->rootcodeir;
    els_func_code *next;
    while ((next = *p) != NULL)
    {
        if (next->marked)
        {
            next->marked = 0;
            p = &next->next;
        }
        else
        {
            *p = next->next;
            els_ScriptFunc_freecodeir(L, next);
        }
    }
}

static void collect_csfunciotn(els_VmObj *L)
{
    ElsCfunc **p = &L->rootcl;
    ElsCfunc *next;
    while ((next = *p) != NULL)
    {
        if (ismarked(next))
        {
            next->mark = next;
            p = &next->next;
        }
        else
        {
            *p = next->next;
            els_ScriptFunc_free_csfunciotn(L, next);
        }
    }
}

static void collectunit(els_VmObj *L)
{
    Hash **p = &L->rootunit;
    Hash *next;
    while ((next = *p) != NULL)
    {
        if (ismarked(next))
        {
            next->mark = next;
            p = &next->next;
        }
        else
        {
            *p = next->next;
            els_Unit_free(L, next);
        }
    }
}



static void collectstrings(els_VmObj *L, int all)
{
    int i;
    for (i = 0; i < L->strpool.size; i++)
    {
        TString **p = &L->strpool.hash[i];
        TString *next;
        while ((next = *p) != NULL)
        {
            if (next->marked && !all)
            {
                if (next->marked < FIXMARK)
                    next->marked = 0;
                p = &next->nexthash;
            }
            else
            {
                *p = next->nexthash;
                L->strpool.nuse--;
                L->nblocks -= sizestring(next->len);
                els_Mem_free(L, next);
            }
        }
    }
    if (L->strpool.nuse < (lint32)(L->strpool.size / 4) && L->strpool.size > 10)
        els_string_resize(L, &L->strpool, L->strpool.size / 2);
}



static void checkBufferTmp(els_VmObj *L)
{
    if (L->BufferTmpSize > MINBUFFER * 2)
    {
        size_t newsize = L->BufferTmpSize / 2;
        L->nblocks += (newsize - L->BufferTmpSize) * sizeof(char);
        L->BufferTmpSize = newsize;
        els_Mem_reallocvector(L, L->BufferTmp, newsize, char);
    }
}



static void els_Gc_collectgarbage(els_VmObj *L)
{
    markall(L);
    invalidaterefs(L);
    els_Gc_collect(L, 0);
    checkBufferTmp(L);
    L->GCnowmax = 2 * L->nblocks;
}



void els_Gc_collect(els_VmObj *L, int all)
{
    collectstrings(L, all);
    collectunit(L);
    collectcodeir(L);
    collect_csfunciotn(L);
}

void els_Gc_checkGC(els_VmObj *L)
{
    if (L->nblocks >= L->GCnowmax){
        els_Gc_collectgarbage(L);   
        if(L->GCnowmax > L->GCMAX)
            L->GCnowmax = L->GCMAX;
    }
}
