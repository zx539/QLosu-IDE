#include "els.h"

#include<string.h>
#include "els_mem.h"
#include "els_object.h"
#include "els_vmhost.h"
#include "els_string.h"
#include "els_unit.h"

#define gcsize(L, n) (sizeof(Hash) + (n) * sizeof(Node))

Node *els_Unit_mainposition(const Hash *t, const StackObj *key)
{
    unsigned long h;
    switch (key->ttype)
    {
    case ELS_TYPE_NUMBER:
        h = (unsigned long)(long)nvalue(key);
        break;
    case ELS_TYPE_STRING:
        h = tsvalue(key)->u.s.hash;
        break;
    case ELS_TYPE_UNIT:
        h = IntPoint(hvalue(key));
        break;
    case ELS_TYPE_FUNCTION:
        h = IntPoint(clvalue(key));
        break;
    default:
        return NULL;
    }
    return &t->node[h & (t->size - 1)];
}

const StackObj *els_Unit_getany(els_VmObj *L, const Hash *t, const StackObj *key)
{
    Node *n = els_Unit_mainposition(t, key);
    if(n){
        do
        {
            if (els_Object_equalObj(key, &n->key))
                return &n->val;
            n = n->next;
        } while (n);
    }
    return &els_Object_nullobject;
}

const StackObj *els_Unit_getnum(const Hash *t, Number key)
{
    Node *n = &t->node[(unsigned long)(long)key & (t->size - 1)];
    do
    {
        if (ttype(&n->key) == ELS_TYPE_NUMBER && nvalue(&n->key) == key)
            return &n->val;
        n = n->next;
    } while (n);
    return &els_Object_nullobject;
}

const StackObj *els_Unit_getstr(const Hash *t, TString *key)
{
    Node *n = &t->node[key->u.s.hash & (t->size - 1)];
    do
    {
        if (ttype(&n->key) == ELS_TYPE_STRING && tsvalue(&n->key)->u.s.hash==key->u.s.hash)
            if (!strcmp(n->key.value.ts->str, key->str))
                return &n->val;
        n = n->next;
    } while (n);
    return &els_Object_nullobject;
}

const StackObj *els_Unit_get(els_VmObj *L, const Hash *t, const StackObj *key)
{
    switch (ttype(key))
    {
    case ELS_TYPE_NUMBER:
        return els_Unit_getnum(t, nvalue(key));
    case ELS_TYPE_STRING:
        return els_Unit_getstr(t, tsvalue(key));
    default:
        return els_Unit_getany(L, t, key);
    }
}

Node *els_Unit_next(els_VmObj *L, const Hash *t, const StackObj *key)
{
    int i;
    if (ttype(key) == ELS_TYPE_NULL)
        i = 0;
    else
    {
        const StackObj *v = els_Unit_get(L, t, key);
        if (v == &els_Object_nullobject)
            vm_error(L, "错误的单元键值",ELS_ERRORBACK_RUN);
        i = (int)(((const char *)v - (const char *)(&t->node[0].val)) / sizeof(Node)) + 1;
    }
    for (; i < t->size; i++)
    {
        Node *n = node(t, i);
        if (ttype(val(n)) != ELS_TYPE_NULL)
            return n;
    }
    return NULL;
}

void els_Unit_remove(Hash *t, StackObj *key)
{
    if (ttype(key) == ELS_TYPE_NUMBER || (ttype(key) == ELS_TYPE_STRING && tsvalue(key)->len <= 30))
        return;
    else
    {
        Node *mp = els_Unit_mainposition(t, key);
        int n = mp - &t->node[0];
        while (els_Unit_getnum(t, n) != &els_Object_nullobject)
        {
            if (n >= MAX_INT - t->size)
                return;
            n += t->size;
        }
        ttype(key) = ELS_TYPE_NUMBER;
        nvalue(key) = n;
    }
}

static void setnodevector(els_VmObj *L, Hash *t, lint32 size)
{
    int i;
    if (size > MAX_INT)
        vm_error(L, "对象储存单元溢出",ELS_ERRORBACK_RUN);
    t->node = els_Mem_newvector(L, size, Node);
    for (i = 0; i < (int)size; i++)
    {
        ttype(&t->node[i].key) = ttype(&t->node[i].val) = ELS_TYPE_NULL;
        t->node[i].next = NULL;
    }
    L->nblocks += gcsize(L, size) - gcsize(L, t->size);
    t->size = size;
    t->firstfree = &t->node[size - 1];
}

Hash *els_Unit_new(els_VmObj *L, int size)
{
    Hash *t = els_Mem_new(L, Hash);
    t->htag = ELS_TYPE_UNIT;
    t->next = L->rootunit;
    L->rootunit = t;
    t->mark = t;
    t->size = 0;
    L->nblocks += gcsize(L, 0);
    t->node = NULL;
    setnodevector(L, t, els_Object_power2(size));
    return t;
}

void els_Unit_free(els_VmObj *L, Hash *t)
{
    L->nblocks -= gcsize(L, t->size);
    els_Mem_free(L, t->node);
    els_Mem_free(L, t);
}

static int numuse(const Hash *t)
{
    Node *v = t->node;
    int size = t->size;
    int realuse = 0;
    int i;
    for (i = 0; i < size; i++)
    {
        if (ttype(&v[i].val) != ELS_TYPE_NULL)
            realuse++;
    }
    return realuse;
}

static void rehash(els_VmObj *L, Hash *t)
{
    int oldsize = t->size;
    Node *nold = t->node;
    int issues_num = numuse(t);
    int i;
    if (issues_num >= oldsize - oldsize / 4)
        setnodevector(L, t, (lint32)oldsize * 2);
    else if (issues_num <= oldsize / 4 && oldsize > 4)
        setnodevector(L, t, oldsize / 2);
    else
        setnodevector(L, t, oldsize);
    for (i = 0; i < oldsize; i++)
    {
        Node *old = nold + i;
        if (ttype(&old->val) != ELS_TYPE_NULL)
            *els_Unit_set(L, t, &old->key) = old->val;
    }
    els_Mem_free(L, nold);
}

StackObj *els_Unit_set(els_VmObj *L, Hash *t, const StackObj *key)
{
    Node *mp = els_Unit_mainposition(t, key);
    Node *n = mp;
    if (!mp){
        vm_error(L,"创建一个单元元素时，其索引不能为空值",ELS_ERRORBACK_RUN);
    }
    do
    {
        if (els_Object_equalObj(key, &n->key))
            return &n->val;
        else
            n = n->next;
    } while (n);
    if (ttype(&mp->key) != ELS_TYPE_NULL)
    {
        Node *othern;
        n = t->firstfree;
        if (mp > n && (othern = els_Unit_mainposition(t, &mp->key)) != mp)
        {

            while (othern->next != mp)
                othern = othern->next;
            othern->next = n;
            *n = *mp;
            mp->next = NULL;
        }
        else
        {

            n->next = mp->next;
            mp->next = n;
            mp = n;
        }
    }
    mp->key = *key;
    while(1)
    {
        if (ttype(&t->firstfree->key) == ELS_TYPE_NULL)
            return &mp->val;
        else if (t->firstfree == t->node)
            break;
        else
            (t->firstfree)--;
    }
    rehash(L, t);
    return els_Unit_set(L, t, key);
}

StackObj *els_Unit_setint(els_VmObj *L, Hash *t, int key)
{
    StackObj index;
    ttype(&index) = ELS_TYPE_NUMBER;
    nvalue(&index) = key;
    return els_Unit_set(L, t, &index);
}

void els_Unit_setstrnum(els_VmObj *L, Hash *t, TString *key, Number val)
{
    StackObj *value, index;
    ttype(&index) = ELS_TYPE_STRING;
    tsvalue(&index) = key;
    value = els_Unit_set(L, t, &index);
    ttype(value) = ELS_TYPE_NUMBER;
    nvalue(value) = val;
}

const StackObj *els_Unit_getglobal(els_VmObj *L, const char *name)
{
    return els_Unit_getstr(L->globalenv, els_string_new(L, name));
}
