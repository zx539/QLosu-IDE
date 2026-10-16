#include <string.h>

#include "els.h"

#include "els_mem.h"
#include "els_object.h"
#include "els_vmhost.h"
#include "els_string.h"


void els_string_init(els_VmObj *vm)
{
    vm->strpool.hash = els_Mem_newvector(vm, 1, TString *);
    vm->nblocks += sizeof(TString*);
    vm->strpool.size  = 1;
    vm->strpool.nuse  = 0;
    vm->strpool.hash[0] = NULL;
}

void els_string_freeall(els_VmObj *vm)
{
    vm->nblocks -= vm->strpool.size  * sizeof(TString *);
    els_Mem_free(vm, vm->strpool.hash);
}

static unsigned long hash_s(const char *s, size_t l)
{
    // JS hash算法
    unsigned long h = l;     
    size_t step = (l >> 5) | 1; 
    for (; l >= step; l -= step)
        h = h ^ ((h << 5) + (h >> 2) + (unsigned char)*(s++));
    return h;
}

void els_string_resize(els_VmObj *vm, stringunit *treeb, int newsize)
{
    TString **newhash = els_Mem_newvector(vm, newsize, TString *);
    int i;
    for (i = 0; i < newsize; i++)
        newhash[i] = NULL;
    
    for (i = 0; i < treeb->size; i++)
    {
        TString *p = treeb->hash[i];
        while (p)
        {                 
            TString *next = p->nexthash; 
            unsigned long h = p->u.s.hash;
            int h1 = h & (newsize - 1); 
            p->nexthash = newhash[h1]; 
            newhash[h1] = p;
            p = next;
        }
    }
    els_Mem_free(vm, treeb->hash);
    vm->nblocks += (newsize - treeb->size) * sizeof(TString *);
    treeb->size = newsize;
    treeb->hash = newhash;
}

static void newentry(els_VmObj *L, stringunit *tb, TString *ts, int h)
{
    ts->nexthash = tb->hash[h]; 
    tb->hash[h] = ts;
    tb->nuse++;
    if (tb->nuse > (lint32)tb->size && tb->size < MAX_INT / 2) 
        els_string_resize(L, tb, tb->size * 2);
}

#include<stdio.h>
TString *els_string_newlstr(els_VmObj *L, const char *str, size_t l)
{
    unsigned long h = hash_s(str, l);
    int h1 = h & (L->strpool.size - 1);
    TString *ts;
    for (ts = L->strpool.hash[h1]; ts; ts = ts->nexthash)
    {
        if(ts->u.s.hash==h)
            if (ts->len == l && (memcmp(str, ts->str, l) == 0))
                return ts;
    }
    ts = (TString *)els_Mem_malloc(L, sizestring(l));
    ts->marked = 0;
    ts->nexthash = NULL;
    ts->len = l;
    ts->u.s.hash = h;
    ts->u.s.constindex = 0;
    memcpy(ts->str, str, l);
    ts->str[l] = 0;
    L->nblocks += sizestring(l);
    newentry(L, &L->strpool, ts, h1); 
    return ts;
}

TString *els_string_new(els_VmObj *L, const char *str)
{
    return els_string_newlstr(L, str, strlen(str));
}

TString *els_string_newfixed(els_VmObj *L, const char *str)
{
    TString *ts = els_string_new(L, str);
    if (ts->marked == 0)
        ts->marked = FIXMARK; 
    return ts;
}

