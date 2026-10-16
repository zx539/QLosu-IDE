
#ifndef els_parser_h
#define els_parser_h

#include "els_object.h"
#include "els_vmcore.h"

typedef enum
{
    VGLOBAL,
    VLOCAL,
    VINDEXED,
    VEXP
} expkind;

typedef struct expdesc
{
    expkind k;
    union
    {
        int index;
        struct
        {
            int t;
            int f;
        } l;
    } u;
} expdesc;

typedef struct ElsFuncObj
{
    els_func_code *f;
    struct ElsFuncObj *prev;
    struct LexObject *ls;
    struct els_VmObj *L;
    int pc;
    int lasttarget;
    int jlt;
    short stacklevel;
    short nactloc;
    short nupvalues;
    int lastline;
    struct Breaklabel *bl;
    expdesc upvalues[32];
    int actloc[128];
} ElsFuncObj;

els_func_code *els_parser_parse(els_VmObj *L, vm_iobuff *z);

#endif
