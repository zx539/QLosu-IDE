#ifndef __els_vmhost_h__
#define __els_vmhost_h__

#include <setjmp.h>
#include "els.h"
#include "els_object.h"


#define NONEXT -1
#define HOLD -2
#define COLLECTED -3
#define LOCK -4


struct Ref
{
    StackObj o;
    int st;
};

struct els_longjmp
{
    jmp_buf b;
    struct els_longjmp *previous;
    volatile int status;
};

typedef struct stringunit
{
    int size;
    lint32 nuse;
    TString **hash;
} stringunit;



struct els_VmObj
{
    struct els_VmObj * mainthread;
    StackObj* top;
    StackObj* stack;
    StackObj* stack_last;
    int stacksize;
    StackObj* Cbase;
    struct els_longjmp *errorJmp;
    char *BufferTmp;
    size_t BufferTmpSize;

    els_func_code *rootcodeir;
    ElsCfunc *rootcl;
    Hash *rootunit;
    stringunit strpool;
    Hash *globalenv;
    struct Ref *refArray;
    int refSize;
    int refFree;
    unsigned long GCnowmax;
    unsigned long nblocks;
    unsigned long GCMAX;
};

void els_unit_init(els_VmObj *L);
void els_unit_realtag(els_VmObj *L, int tag);
int els_unit_tag(const StackObj *o);
int els_unit_validevent(int t, int e);

#endif