#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "els.h"
#include "els_heap.h"
#include "els_func.h"
#include "els_gc.h"
#include "els_object.h"
#include "els_bytecode.h"
#include "els_vmhost.h"
#include "els_string.h"
#include "els_unit.h"
#include "els_vmcore.h"
#include "els_mem.h"



#define EXTRA_STACK 8

int els_Vmcore_tonumber(StackObj *obj)
{
    if (ttype(obj) != ELS_TYPE_STRING)
        return 1;
    else
    {
        if (!els_Object_str2d(svalue(obj), &nvalue(obj)))
            return 2;
        ttype(obj) = ELS_TYPE_NUMBER;
        return 0;
    }
}

int els_Vmcore_tostring(els_VmObj *L, StackObj *obj)
{
    if (ttype(obj) != ELS_TYPE_NUMBER)
        return 1;
    else
    {
        char s[32];
        els_number2str(s, nvalue(obj));
        tsvalue(obj) = els_string_new(L, s);
        ttype(obj) = ELS_TYPE_STRING;
        return 0;
    }
}

ElsCfunc *els_Vmcore_csfunction(els_VmObj *L, int issues_num)
{
    ElsCfunc *c = els_ScriptFunc_new_csfunciotn(L, issues_num);
    L->top -= issues_num;
    while (issues_num--)
        c->upvalue[issues_num] = *(L->top + issues_num);
    clvalue(L->top) = c;
    ttype(L->top) = ELS_TYPE_FUNCTION;
    incr_top(L);
    return c;
}

void els_Vmcore_C_csfunciotn(els_VmObj *L, els_C_API_function c, int issues_num)
{
    ElsCfunc *cl = els_Vmcore_csfunction(L, issues_num);
    cl->f.c = c;
    cl->isC = 1;
}

void els_Vmcore_L_csfunciotn(els_VmObj *L, els_func_code *l, int issues_num)
{
    ElsCfunc *cl = els_Vmcore_csfunction(L, issues_num);
    cl->f.l = l;
    cl->isC = 0;
}

const StackObj *els_Vmcore_getunit(els_VmObj *L, StackObj* t)
{
    if (ttype(t) == ELS_TYPE_NULL)
    {
        return &els_Object_nullobject;
    }
    if (ttype(t) == ELS_TYPE_UNIT)
    {
        const StackObj *h = els_Unit_get(L, hvalue(t), L->top - 1);
        if (ttype(h) == ELS_TYPE_NULL)
            return &els_Object_nullobject;
        return h;
    }
    return &els_Object_nullobject;
}

void els_Vmcore_setunit(els_VmObj *L, StackObj* t, StackObj* key)
{
    int tg;
    if (ttype(t) == ELS_TYPE_UNIT && (tg = hvalue(t)->htag) == ELS_TYPE_UNIT )
        *els_Unit_set(L, hvalue(t), key) = *(L->top - 1);
    else
        vm_error(L,"试图向一个非索引对象赋值",ELS_ERRORBACK_RUN);
}

const StackObj *els_Vmcore_getglobal(els_VmObj *L, TString *s)
{
    return els_Unit_getstr(L->globalenv,s);
}

void els_Vmcore_setglobal(els_VmObj *L, TString *s)
{
    const StackObj *oldvalue = els_Unit_getstr(L->globalenv, s);
    if (oldvalue != &els_Object_nullobject)
    {
        *(StackObj *)oldvalue = *(L->top - 1);
    }
    else
    {
        StackObj key;
        ttype(&key) = ELS_TYPE_STRING;
        tsvalue(&key) = s;
        *els_Unit_set(L, L->globalenv, &key) = *(L->top - 1);
    }
    
}

static int els_Vmcore_strcomp(const TString *ls, const TString *rs)
{
    const char *l = ls->str;
    size_t ll = ls->len;
    const char *r = rs->str;
    size_t lr = rs->len;
    while (1)
    {
        int temp = strcoll(l, r);
        if (temp != 0)
            return temp;
        else
        {
            size_t len = strlen(l);
            if (len == ll)
                return (len == lr) ? 0 : -1;
            else if (len == lr)
                return 1;

            len++;
            l += len;
            ll -= len;
            r += len;
            lr -= len;
        }
    }
}

int els_Vmcore_lessthan(els_VmObj *L, const StackObj *l, const StackObj *r, StackObj* top)
{
    if(ttype(l)!=ttype(r)){
        if(ttype(l)==ELS_TYPE_NUMBER&&ttype(r)==ELS_TYPE_STRING)
            return nvalue(l)<atof(svalue(r));
        else if(ttype(r)==ELS_TYPE_NUMBER&&ttype(l)==ELS_TYPE_STRING)
            return atof(svalue(l))<nvalue(r);
        else 
            return 0;
    }

    switch (ttype(l))
    {
        case ELS_TYPE_NUMBER:
            return (nvalue(l) < nvalue(r));
            break;
        case ELS_TYPE_STRING:
            return (els_Vmcore_strcomp(tsvalue(l), tsvalue(r)) < 0);
        default:
            break;
    }
    return ttype(r)!=ELS_TYPE_NULL||ttype(l)!=ELS_TYPE_NULL;
}

void els_Vmcore_strconc(els_VmObj *L, StackObj* top)
{
    if (ttype(top - 1) == ELS_TYPE_NULL)
    {
        (*(top - 1)).ttype = ELS_TYPE_STRING;
        (*(top - 1)).value.ts = els_string_newlstr(L, "", 0);
    }
    if (ttype(top - 2) == ELS_TYPE_NULL)
    {
        (*(top - 2)).ttype = ELS_TYPE_STRING;
        (*(top - 2)).value.ts = els_string_newlstr(L, "", 0);
    }
    if (tostring(L, top - 2) || tostring(L, top - 1))
    {
        top += 2;
        L->top = top;
        tsvalue(top - 1) = els_string_newlstr(L, "::&", 3);
        ttype(top - 1) = ELS_TYPE_STRING;
        *(top - 2) = *(top - 4);
        *(top) = *els_Vmcore_getunit(L, top - 2);
        top += 3;
        L->top = top;
        *(top - 2) = *(top - 7);
        *(top - 1) = *(top - 6);
        if(ttype(top-3)!=ELS_TYPE_FUNCTION)
            vm_error(L,"& 运算符并未被定义如何重载",ELS_ERRORBACK_RUN);    
        els_Heap_call(L, top - 3, 1);
        *(top - 7) = *(top);
        top -= 6;
        L->top = top;
        return;
    }
    else if (tsvalue(top - 1)->len > 0)
    {
        lint32 tl = (lint32)tsvalue(top - 1)->len + (lint32)tsvalue(top - 2)->len;
        char *buffer;
        if (tl > MAX_SIZET)
            vm_error(L, "字符串大小溢出",ELS_ERRORBACK_RUN);
        buffer = els_Object_openspace(L, tl);
        tl = 0;
        size_t l = tsvalue(top - 2)->len;
        memcpy(buffer + tl, tsvalue(top - 2)->str, l);
        tl += l;
        l = tsvalue(top - 1)->len;
        memcpy(buffer + tl, tsvalue(top - 1)->str, l);
        tl += l;
        tsvalue(top - 2) = els_string_newlstr(L, buffer, tl);
    }
    top--;
}

static void els_Vmcore_pack(els_VmObj *L, StackObj* firstelem)
{
    int i;
    Hash *hunit = els_Unit_new(L, 0);
    for (i = 0; firstelem + i < L->top; i++)
        *els_Unit_setint(L, hunit, i + 1) = *(firstelem + i);

    els_Unit_setstrnum(L, hunit, els_string_new(L, "n"), i);
    #ifdef ELS_CONF_TOKEN_CN
        els_Unit_setstrnum(L, hunit, els_string_new(L, "个数"), i);
    #endif
    L->top = firstelem;
    ttype(L->top) = ELS_TYPE_UNIT;
    hvalue(L->top) = hunit;
    incr_top(L);
}

static void adjust_varargs(els_VmObj *L, StackObj* base, int nfixargs)
{
    int nvararg = (L->top - base) - nfixargs;
    if (nvararg < 0)
        els_Heap_adjusttop(L, base, nfixargs);
    els_Vmcore_pack(L, base + nfixargs);
}

#define dojump(pc, i)        \
    {                        \
        int d = GETARG_S(i); \
        pc += d;             \
    }

extern double pow(double x, double y);


StackObj* els_Vmcore_execute(els_VmObj *L, const ElsCfunc *cl, StackObj* base)
{
    const els_func_code *const tf = cl->f.l;
    StackObj* top;
    const Instruction *pc = tf->code;
    TString **const kstr = tf->kstr;
    infovalue(base - 1)->pc = &pc;
    els_Heap_checkstack(L, tf->maxstacksize + EXTRA_STACK);
    if (tf->is_vararg)
        adjust_varargs(L, base, tf->numparams);
    else
        els_Heap_adjusttop(L, base, tf->numparams);
    top = L->top;

    while (1)
    {
        const Instruction i = *pc++;
        switch (GET_OPCODE(i))
        {
        case BYTECODE_END:
        {
            L->top = top;
            return top;
        }
        case BYTECODE_RETURN:
        {
            L->top = top;
            return base + GETARG_U(i);
        }
        case BYTECODE_CALL:
        {
            int nres = GETARG_B(i);
            if (nres == MULT_RET)
                nres = ELS_MULTRET;
            L->top = top;
            // printf("STACK Start:\t%lx\n",top);
            LosuObj * func =  base + GETARG_A(i);
			// if(ttype(func)==ELS_TYPE_UNIT){
			// 	L->top++;
			// 	for(LosuObj* i = L->top-1;i != func;i--)
			// 		*i = *(i-1);
			// 	*func = *obj_indexunit(L,*func,obj_newstr(L,"class"));
			// }
			
			
            els_Heap_call(L, func, nres);

            top = L->top;
            // printf("STACK End:\t%lx\n",top);
            break;
        }
        case BYTECODE_PUSHNULL:
        {
            int n = GETARG_U(i);
            do
            {
                ttype(top++) = ELS_TYPE_NULL;
            } while (--n > 0);
            break;
        }
        case BYTECODE_POP:
        {
            top -= GETARG_U(i);
            break;
        }
        case BYTECODE_PUSHSTRING:
        {
            ttype(top) = ELS_TYPE_STRING;
            tsvalue(top) = kstr[GETARG_U(i)];
            top++;
            break;
        }
        case BYTECODE_PUSHNUM:
        {
            ttype(top) = ELS_TYPE_NUMBER;
            nvalue(top) = tf->knum[GETARG_U(i)];
            top++;
            break;
        }
        case BYTECODE_PUSHUPVALUE:
        {
            *top++ = cl->upvalue[GETARG_U(i)];
            break;
        }
        case BYTECODE_GETLOCAL:
        {
            *top++ = *(base + GETARG_U(i));
            break;
        }
        case BYTECODE_GETGLOBAL:
        {
            L->top = top;
            *top = *els_Vmcore_getglobal(L, kstr[GETARG_U(i)]);
            top++;
            break;
        }
        case BYTECODE_GETUNIT:
        {
            char *tmp;
            StackObj tmp2;
            StackObj *h;
            L->top = top;
            top--;
            switch (ttype(top - 1))
            {
            case ELS_TYPE_STRING:
                
                h = &tmp2;
                tmp = (char *)els_Mem_malloc(L, 2);
                if ((int)((*(top)).value.n) > strlen(((*(top - 1)).value.ts->str)) )
                    tmp[0]='\0';
                else
                    tmp[0] = ((*(top - 1)).value.ts->str)[(int)((*(top)).value.n) - 1];
                tmp[1] = '\0';
                h->ttype = ELS_TYPE_STRING;
                h->value.ts = els_string_newlstr(L, tmp, 1);
                *(top - 1) = *h;
                break;
            case ELS_TYPE_BYTE:
                h = &tmp2;
                tmp = (char *)els_Mem_malloc(L, 2);
                if ((int)((*(top)).value.n) > (((*(top - 1)).value.ts->len)) )
                    tmp[0]='\0';
                else
                    tmp[0] = ((*(top - 1)).value.ts->str)[(int)((*(top)).value.n) - 1];
                tmp[1] = '\0';
                h->ttype = ELS_TYPE_STRING;
                h->value.ts = els_string_newlstr(L, tmp, 1);
                *(top - 1) = *h;
                break;
            default:
                *(top - 1) = *els_Vmcore_getunit(L, top - 1);
                break;
            }
            
            break;
        }
        case BYTECODE_PUSHSELF:
        {
            StackObj receiver;
            receiver = *(top - 1);
            ttype(top) = ELS_TYPE_STRING;
            tsvalue(top++) = kstr[GETARG_U(i)];
            L->top = top;
            *(top - 2) = *els_Vmcore_getunit(L, top - 2);
            *(top - 1) = receiver;
            break;
        }
        case BYTECODE_CREATEUNIT:
        {
            L->top = top;
#if ELS_CONF_GC_ENABLE   == 1
            els_Gc_checkGC(L);
#endif
            hvalue(top) = els_Unit_new(L, GETARG_U(i));
            ttype(top) = ELS_TYPE_UNIT;
            top++;
            break;
        }
        case BYTECODE_SETLOCAL:
        {
            *(base + GETARG_U(i)) = *(--top);
            break;
        }
        case BYTECODE_SETGLOBAL:
        {
            L->top = top;
            els_Vmcore_setglobal(L, kstr[GETARG_U(i)]);
            top--;
            break;
        }
        case BYTECODE_SETUNIT:
        {
            StackObj* t = top - GETARG_A(i);
            L->top = top;
            els_Vmcore_setunit(L, t, t + 1);
            top -= GETARG_B(i);
            break;
        }
        case BYTECODE_SETLIST:
        {
            int aux = GETARG_A(i) * LFIELDS_PER_FLUSH;
            int n = GETARG_B(i);
            Hash *arr = hvalue(top - n - 1);
            L->top = top - n;
            for (; n; n--)
                *els_Unit_setint(L, arr, n + aux) = *(--top);
            break;
        }
        case BYTECODE_SETMAP:
        {
            int n = GETARG_U(i);
            StackObj* finaltop = top - 2 * n;
            Hash *arr = hvalue(finaltop - 1);
            L->top = finaltop;
            for (; n; n--)
            {
                top -= 2;
                *els_Unit_set(L, arr, top) = *(top + 1);
            }
            break;
        }
        case BYTECODE_ADD:
        {
            if (tonumber(top - 2) || tonumber(top - 1))
            {
                top += 2;
                L->top = top;
                tsvalue(top - 1) = els_string_newlstr(L, "::+", 3);
                ttype(top - 1) = ELS_TYPE_STRING;
                *(top - 2) = *(top - 4);
                *(top) = *els_Vmcore_getunit(L, top - 2);
                top += 3;
                L->top = top;
                *(top - 2) = *(top - 7);
                *(top - 1) = *(top - 6);
                if(ttype(top-3)!=ELS_TYPE_FUNCTION)
                    vm_error(L,"+ 运算符并未被定义如何重载",ELS_ERRORBACK_RUN);   
                els_Heap_call(L, top - 3, 1);
                *(top - 7) = *(top);
                top -= 6;
                L->top = top;
                break;
            }
            else
                nvalue(top - 2) += nvalue(top - 1);
            top--;
            break;
        }
        case BYTECODE_SUB:
        {
            if (tonumber(top - 2) || tonumber(top - 1))
            {
                top += 2;
                L->top = top;
                tsvalue(top - 1) = els_string_newlstr(L, "::-", 3);
                ttype(top - 1) = ELS_TYPE_STRING;
                *(top - 2) = *(top - 4);
                *(top) = *els_Vmcore_getunit(L, top - 2);
                top += 3;
                L->top = top;
                *(top - 2) = *(top - 7);
                *(top - 1) = *(top - 6);
                if(ttype(top-3)!=ELS_TYPE_FUNCTION)
                    vm_error(L,"- 运算符并未被定义如何重载",ELS_ERRORBACK_RUN);   
                els_Heap_call(L, top - 3, 1);
                *(top - 7) = *(top);
                top -= 6;
                L->top = top;
                break;
            }
            else
                nvalue(top - 2) -= nvalue(top - 1);
            top--;
            break;
        }
        case BYTECODE_MULT:
        {
            if (tonumber(top - 2) || tonumber(top - 1))
            {
                top += 2;
                L->top = top;
                tsvalue(top - 1) = els_string_newlstr(L, "::*", 3);
                ttype(top - 1) = ELS_TYPE_STRING;
                *(top - 2) = *(top - 4);
                *(top) = *els_Vmcore_getunit(L, top - 2);
                top += 3;
                L->top = top;
                *(top - 2) = *(top - 7);
                *(top - 1) = *(top - 6);
                if(ttype(top-3)!=ELS_TYPE_FUNCTION)
                    vm_error(L,"* 运算符并未被定义如何重载",ELS_ERRORBACK_RUN);   
                els_Heap_call(L, top - 3, 1);
                *(top - 7) = *(top);
                top -= 6;
                L->top = top;
                break;
            }
            else
                nvalue(top - 2) *= nvalue(top - 1);
            top--;
            break;
        }
        case BYTECODE_DIV:
        {
            if (tonumber(top - 2) || tonumber(top - 1))
            {
                top += 2;
                L->top = top;
                tsvalue(top - 1) = els_string_newlstr(L, "::/", 3);
                ttype(top - 1) = ELS_TYPE_STRING;
                *(top - 2) = *(top - 4);
                *(top) = *els_Vmcore_getunit(L, top - 2);
                top += 3;
                L->top = top;
                *(top - 2) = *(top - 7);
                *(top - 1) = *(top - 6);
                if(ttype(top-3)!=ELS_TYPE_FUNCTION)
                    vm_error(L,"/ 运算符并未被定义如何重载",ELS_ERRORBACK_RUN);   
                els_Heap_call(L, top - 3, 1);
                *(top - 7) = *(top);
                top -= 6;
                L->top = top;
                break;
            }
            else
                nvalue(top - 2) /= nvalue(top - 1);
            top--;
            break;
        }
        case BYTECODE_POW:
        {
            if (tonumber(top - 2) || tonumber(top - 1))
            {
                top += 2;
                L->top = top;
                tsvalue(top - 1) = els_string_newlstr(L, "::^", 3);
                ttype(top - 1) = ELS_TYPE_STRING;
                *(top - 2) = *(top - 4);
                *(top) = *els_Vmcore_getunit(L, top - 2);
                top += 3;
                L->top = top;
                *(top - 2) = *(top - 7);
                *(top - 1) = *(top - 6);
                if(ttype(top-3)!=ELS_TYPE_FUNCTION)
                    vm_error(L,"^ 运算符并未被定义如何重载",ELS_ERRORBACK_RUN);   
                els_Heap_call(L, top - 3, 1);
                *(top - 7) = *(top);
                top -= 6;
                L->top = top;
                break;
            }
            else
                nvalue(top - 2) = pow(nvalue(top - 2), nvalue(top - 1));
            top--;
            break;
        }
        case BYTECODE_CONCAT:
        {
            els_Vmcore_strconc(L, top);
            top--;
            L->top = top;
#if ELS_CONF_GC_ENABLE   == 1
            els_Gc_checkGC(L);
#endif
            break;
        }
        case BYTECODE_MINUS:
        {
            if (tonumber(top - 1))
            {
                ttype(top) = ELS_TYPE_NULL;
            }
            else
                nvalue(top - 1) = -nvalue(top - 1);
            break;
        }
        case BYTECODE_NOT:
        {
            ttype(top - 1) = (ttype(top - 1) == ELS_TYPE_NULL) ? ELS_TYPE_NUMBER : ELS_TYPE_NULL;
            nvalue(top - 1) = 1;
            break;
        }
        case BYTECODE_JMPNE:
        {
            top -= 2;
            if (!els_Object_equalObj(top, top + 1))
                dojump(pc, i);
            break;
        }
        case BYTECODE_JMPEQ:
        {
            top -= 2;
            if (els_Object_equalObj(top, top + 1))
                dojump(pc, i);
            break;
        }
        case BYTECODE_JMPLT:
        {
            top -= 2;
            if (els_Vmcore_lessthan(L, top, top + 1, top + 2))
                dojump(pc, i);
            break;
        }
        case BYTECODE_JMPLE:
        {
            top -= 2;
            if (!els_Vmcore_lessthan(L, top + 1, top, top + 2))
                dojump(pc, i);
            break;
        }
        case BYTECODE_JMPGT:
        {
            top -= 2;
            if (els_Vmcore_lessthan(L, top + 1, top, top + 2))
                dojump(pc, i);
            break;
        }
        case BYTECODE_JMPGE:
        {
            top -= 2;
            if (!els_Vmcore_lessthan(L, top, top + 1, top + 2))
                dojump(pc, i);
            break;
        }
        case BYTECODE_JMPT:
        {
            if (ttype(--top) != ELS_TYPE_NULL)
                dojump(pc, i);
            break;
        }
        case BYTECODE_JMPF:
        {
            if (ttype(--top) == ELS_TYPE_NULL)
                dojump(pc, i);
            break;
        }
        case BYTECODE_JMPONT:
        {
            if (ttype(top - 1) == ELS_TYPE_NULL)
                top--;
            else
                dojump(pc, i);
            break;
        }
        case BYTECODE_JMPONF:
        {
            if (ttype(top - 1) != ELS_TYPE_NULL)
                top--;
            else
                dojump(pc, i);
            break;
        }
        case BYTECODE_JMP:
        {
            dojump(pc, i);
            break;
        }
        case BYTECODE_PUSHNULLJMP:
        {
            ttype(top++) = ELS_TYPE_NULL;
            pc++;
            break;
        }
        case BYTECODE_FORPREP:
        {
            if (tonumber(top - 1))
            {
                vm_error(L, "错误的循环结构，缺少步幅",ELS_ERRORBACK_RUN);
            }
            if (tonumber(top - 2))
                vm_error(L, "错误的循环结构，缺少极值",ELS_ERRORBACK_RUN);
            if (tonumber(top - 3))
                vm_error(L, "错误的循环结构，缺少初值",ELS_ERRORBACK_RUN);
            if (nvalue(top - 1) > 0 ? nvalue(top - 3) > nvalue(top - 2) : nvalue(top - 3) < nvalue(top - 2))
            {
                top -= 3;
                dojump(pc, i);
            }
            break;
        }
        case BYTECODE_FORLOOP:
        {

            if (ttype(top - 3) != ELS_TYPE_NUMBER)
                vm_error(L, "错误的循环结构，缺少步幅",ELS_ERRORBACK_RUN);
            nvalue(top - 3) += nvalue(top - 1);
            if (nvalue(top - 1) > 0 ? nvalue(top - 3) > nvalue(top - 2) : nvalue(top - 3) < nvalue(top - 2))
                top -= 3;
            else
                dojump(pc, i);
            break;
        }
        case BYTECODE_LFORPREP:
        {
            Node *node;
            if (ttype(top - 1) != ELS_TYPE_UNIT)
                vm_error(L, "错误的循环结构，缺少要列举的对象",ELS_ERRORBACK_RUN);
            node = els_Unit_next(L, hvalue(top - 1), &els_Object_nullobject);
            if (node == NULL)
            {
                top--;
                dojump(pc, i);
            }
            else
            {
                top += 2;
                *(top - 2) = *key(node);
                *(top - 1) = *val(node);
            }
            break;
        }
        case BYTECODE_LFORLOOP:
        {
            Node *node;
            node = els_Unit_next(L, hvalue(top - 3), top - 2);
            if (node == NULL)
                top -= 3;
            else
            {
                *(top - 2) = *key(node);
                *(top - 1) = *val(node);
                dojump(pc, i);
            }
            break;
        }
        case BYTECODE_PUSHFUNCTION:
        {
            L->top = top;
            els_Vmcore_L_csfunciotn(L, tf->kcodeir[GETARG_A(i)], GETARG_B(i));
            top = L->top;
#if ELS_CONF_GC_ENABLE   == 1
            els_Gc_checkGC(L);
#endif
            break;
        }
        default:
        {
            vm_error(L, "错误的字节码，编译器完整性可能遭到破坏，拒绝进一步地执行",ELS_ERRORBACK_OVER);
        }
        }
#if ELS_CONF_GC_ENABLE   == 2
        els_Gc_checkGC(L);
#endif
    }
}

static int zmfilbuf(vm_iobuff *z)
{
    (void)z;
    return EOZ;
}

vm_iobuff *els_vmio_mopen(vm_iobuff *z, const char *b, size_t size, const char *name)
{
    if (b == NULL)
        return NULL;
    z->n = size;
    z->p = (const unsigned char *)b;
    z->filbuf = zmfilbuf;
    z->u = NULL;
    z->name = name;
    return z;
}

vm_iobuff *els_vmio_sopen(vm_iobuff *z, const char *s, const char *name)
{
    if (s == NULL)
        return NULL;
    return els_vmio_mopen(z, s, strlen(s), name);
}

static int zffilbuf(vm_iobuff *z)
{
    size_t n=0;
    if (feof((FILE *)(z->u)))
        return EOZ;
    n = fread((z->buffer),sizeof(char) , ELS_VMIO_SIZE, (FILE *)(z->u)); 
    if (n == 0)
        return EOZ;
    z->n = n - 1;
    z->p = z->buffer;
    return *(z->p++);
}

vm_iobuff *els_vmio_Fopen(vm_iobuff *z, FILE *f, const char *name)
{
    if (f == NULL)
        return NULL;
    z->n = 0;
    z->p = z->buffer;
    z->filbuf = zffilbuf;
    z->u = f;
    z->name = name;
    return z;
}

size_t els_vmio_read(vm_iobuff *z, void *b, size_t n)
{
    while (n)
    {
        size_t m;
        if (z->n == 0)
        {
            if (z->filbuf(z) == EOZ)
                return n;
            els_vmio_ugetc(z);
        }
        m = (n <= z->n) ? n : z->n;
        memcpy(b, z->p, m);
        z->n -= m;
        z->p += m;
        b = (char *)b + m;
        n -= m;
    }
    return 0;
}
