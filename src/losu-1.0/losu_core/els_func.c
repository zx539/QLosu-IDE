#include <stdlib.h>
#include <string.h>
#include "els.h"
#include "els_func.h"
#include "els_mem.h"
#include "els_vmhost.h"

#define size_csfunciotn(n) ((int)sizeof(ElsCfunc) + (int)sizeof(StackObj) * ((n)-1))
#define codeirsize(f) (sizeof(els_func_code) + f->nknum * sizeof(Number) + f->nkstr * sizeof(TString *) + f->nkcodeir * sizeof(els_func_code *) + f->ncode * sizeof(Instruction) + f->nlocvars * sizeof(struct LocVar) + f->nlineinfo * sizeof(int))


els_func_code *els_ScriptFunc_newcodeir(els_VmObj *L)
{
    els_func_code *f = els_Mem_new(L, els_func_code);
    memset((void*)f,0,sizeof(els_func_code));
    f->next = L->rootcodeir;
    L->rootcodeir = f;
    return f;
}

ElsCfunc *els_ScriptFunc_new_csfunciotn(els_VmObj *L, int issues_num)
{
    int size = size_csfunciotn(issues_num);
    ElsCfunc *c = (ElsCfunc *)els_Mem_malloc(L, size);
    c->next = L->rootcl;
    L->rootcl = c;
    c->mark = c;
    c->nupvalues = issues_num;
    L->nblocks += size;
    return c;
}

void els_ScriptFunc_freecodeir(els_VmObj *L, els_func_code *f)
{
    if (f->ncode > 0)
        L->nblocks -= codeirsize(f);
    els_Mem_free(L, f->code);
    els_Mem_free(L, f->locvars);
    els_Mem_free(L, f->kstr);
    els_Mem_free(L, f->knum);
    els_Mem_free(L, f->kcodeir);
    els_Mem_free(L, f->lineinfo);
    els_Mem_free(L, f);
}

void els_ScriptFunc_free_csfunciotn(els_VmObj *L, ElsCfunc *c)
{
    L->nblocks -= size_csfunciotn(c->nupvalues);
    els_Mem_free(L, c);
}


void els_ScriptFunc_end(els_VmObj *L, els_func_code *f, int pc)
{
    f->ncode = pc;
    L->nblocks += codeirsize(f);
}

const char *els_ScriptFunc_getlocalname(const els_func_code *f, int local_number, int pc)
{
    int i;
    for (i = 0; i < f->nlocvars && f->locvars[i].startpc <= pc; i++)
    {
        if (pc < f->locvars[i].endpc)
        {
            local_number--;
            if (local_number == 0)
                return f->locvars[i].varname->str;
        }
    }
    return NULL;
}
