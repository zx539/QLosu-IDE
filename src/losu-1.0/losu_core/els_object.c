#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "els.h"
#include "els_mem.h"
#include "els_object.h"
#include "els_vmhost.h"


const StackObj els_Object_nullobject = {
    ELS_TYPE_NULL, {NULL}
};

lint32 els_Object_power2(lint32 n)
{
    lint32 p = 4;
    while (p <= n)
        p <<= 1;
    return p;
}

int els_Object_equalObj(const StackObj *t1, const StackObj *t2)
{
    if (ttype(t1) != ttype(t2)) 
    {
        if(  (ttype(t1)==ELS_TYPE_NUMBER&&(ttype(t2)==ELS_TYPE_STRING)) )
            return nvalue(t1) == atof(t2->value.ts->str);    
        else if( (ttype(t2)==ELS_TYPE_NUMBER&&(ttype(t1)==ELS_TYPE_STRING)) )
            return nvalue(t2)==atof(t1->value.ts->str);
        else
            return 0;
    }
    switch (ttype(t1))
    {
        case ELS_TYPE_BYTE:
            return tsvalue(t1)->len == tsvalue(t2)->len ? !memcmp(t1,t2,tsvalue(t1)->len) : 0;
        case ELS_TYPE_NUMBER:
            return nvalue(t1) == nvalue(t2);
        case ELS_TYPE_STRING:
            return  tsvalue(t1)->u.s.hash == tsvalue(t2)->u.s.hash? !strcmp(svalue(t1),svalue(t2)) : 0 ; 
        case ELS_TYPE_UNIT:
            return hvalue(t1) == hvalue(t2);
        case ELS_TYPE_FUNCTION:
            return clvalue(t1) == clvalue(t2);
        default:
            return 0; 
    }
}

char *els_Object_openspace(els_VmObj *L, size_t n)
{
    if (n > L->BufferTmpSize)
    {
        els_Mem_reallocvector(L, L->BufferTmp, n, char);
        L->nblocks += (n - L->BufferTmpSize) * sizeof(char);
        L->BufferTmpSize = n;
    }
    return L->BufferTmp;
}

int els_Object_str2d(const char *s, Number *result)
{ 
    char *endptr;
    Number res = els_str2number(s, &endptr);
    if (endptr == s)
        return 0; 
    while (isspace((unsigned char)*endptr))
        endptr++;
    if (*endptr != '\0')
        return 0; 
    *result = res;
    return 1;
}