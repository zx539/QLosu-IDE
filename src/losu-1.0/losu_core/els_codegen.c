#include "stdlib.h"

#include "els.h"

#include "els_codegen.h"
#include "els_heap.h"
#include "els_lex.h"
#include "els_mem.h"
#include "els_object.h"
#include "els_bytecode.h"
#include "els_parser.h"
#include "els_vmcore.h"
#include<string.h>

#define code_label(fs,op,arg) (els_codegen_getlabel(fs),els_codegen_code_arg1(fs,op,arg))

static const struct
{
    ByteIR opcode;
    int arg;
} codes[] = {
    {BYTECODE_ADD, 0}, 
    {BYTECODE_SUB, 0}, 
    {BYTECODE_MULT, 0}, 
    {BYTECODE_DIV, 0}, 
    {BYTECODE_POW, 0}, 
    {BYTECODE_CONCAT, 0}, 
    {BYTECODE_JMPNE, NO_JUMP}, 
    {BYTECODE_JMPEQ, NO_JUMP}, 
    {BYTECODE_JMPLT, NO_JUMP}, 
    {BYTECODE_JMPLE, NO_JUMP}, 
    {BYTECODE_JMPGT, NO_JUMP}, 
    {BYTECODE_JMPGE, NO_JUMP}
};


const struct Bytecodesubframe els_codegen_bytecodesubframe[] = {
    {iO, 0, 0},   /* BYTECODE_END */
    {iU, 0, 0},   /* BYTECODE_RETURN */
    {iAB, 0, 0},  /* BYTECODE_CALL */
    {iU, VD, 0},  /* BYTECODE_PUSHNULL */
    {iU, VD, 0},  /* BYTECODE_POP */
    {iU, 1, 0},   /* BYTECODE_PUSHSTRING */
    {iU, 1, 0},   /* BYTECODE_PUSHNUM */
    {iU, 1, 0},   /* BYTECODE_PUSHUPVALUE */
    {iU, 1, 0},   /* BYTECODE_GETLOCAL */
    {iU, 1, 0},   /* BYTECODE_GETGLOBAL */
    {iO, 1, 2},   /* BYTECODE_GETUNIT */
    {iU, 2, 1},   /* BYTECODE_PUSHSELF */
    {iU, 1, 0},   /* BYTECODE_CREATEUNIT */
    {iU, 0, 1},   /* BYTECODE_SETLOCAL */
    {iU, 0, 1},   /* BYTECODE_SETGLOBAL */
    {iAB, VD, 0}, /* BYTECODE_SETUNIT */
    {iAB, VD, 0}, /* BYTECODE_SETLIST */
    {iU, VD, 0},  /* BYTECODE_SETMAP */
    {iO, 1, 2},   /* BYTECODE_ADD */
    {iO, 1, 2},   /* BYTECODE_SUB */
    {iO, 1, 2},   /* BYTECODE_MULT */
    {iO, 1, 2},   /* BYTECODE_DIV */
    {iO, 1, 2},   /* BYTECODE_POW */
    {iO, 1, 2},  /* BYTECODE_CONCAT */
    {iO, 1, 1},   /* BYTECODE_MINUS */
    {iO, 1, 1},   /* BYTECODE_NOT */
    {iS, 0, 2},   /* BYTECODE_JMPNE */
    {iS, 0, 2},   /* BYTECODE_JMPEQ */
    {iS, 0, 2},   /* BYTECODE_JMPLT */
    {iS, 0, 2},   /* BYTECODE_JMPLE */
    {iS, 0, 2},   /* BYTECODE_JMPGT */
    {iS, 0, 2},   /* BYTECODE_JMPGE */
    {iS, 0, 1},   /* BYTECODE_JMPT */
    {iS, 0, 1},   /* BYTECODE_JMPF */
    {iS, 0, 1},   /* BYTECODE_JMPONT */
    {iS, 0, 1},   /* BYTECODE_JMPONF */
    {iS, 0, 0},   /* BYTECODE_JMP */
    {iO, 0, 0},   /* BYTECODE_PUSHNULLJMP */
    {iS, 0, 0},   /* BYTECODE_FORPREP */
    {iS, 0, 3},   /* BYTECODE_FORLOOP */
    {iS, 2, 0},   /* BYTECODE_LFORPREP */
    {iS, 0, 3},   /* BYTECODE_LFORLOOP */
    {iAB, VD, 0}  /* BYTECODE_PUSHFUNCTION */
};

int els_codegen_jump(ElsFuncObj *fs)
{
    int j = els_codegen_code_arg1(fs, BYTECODE_JMP, NO_JUMP);
    if (j == fs->lasttarget)
    {
        els_codegen_concat(fs, &j, fs->jlt);
        fs->jlt = NO_JUMP;
    }
    return j;
}

static void els_codegen_fixjump(ElsFuncObj *fs, int pc, int dest)
{
    Instruction *jmp = &fs->f->code[pc];
    if (dest == NO_JUMP)
        SETARG_S(*jmp, NO_JUMP);
    else
    {
        int offset = dest - (pc + 1);
        if (abs(offset) > MAX_ARG_S){
            els_codegen_error(fs->ls, "控制结构超出长度限制");
        }
        SETARG_S(*jmp, offset);
    }
}

static int els_codegen_getjump(ElsFuncObj *fs, int pc)
{
    int offset = GETARG_S(fs->f->code[pc]);
    if (offset == NO_JUMP)
        return NO_JUMP;
    else
        return (pc + 1) + offset;
}

int els_codegen_getlabel(ElsFuncObj *fs)
{
    if (fs->pc != fs->lasttarget)
    {
        int lasttarget = fs->lasttarget;
        fs->lasttarget = fs->pc;
        els_codegen_patchlist(fs, fs->jlt, lasttarget);
        fs->jlt = NO_JUMP;
    }
    return fs->pc;
}

void els_codegen_deltastack(ElsFuncObj *fs, int delta)
{
    fs->stacklevel += delta;
    if (fs->stacklevel > fs->f->maxstacksize)
    {
        if (fs->stacklevel > MAXSTACK){
            els_codegen_error(fs->ls, "表达式过于复杂");
        }
        fs->f->maxstacksize = fs->stacklevel;
    }
}

static int number_constant(ElsFuncObj *fs, Number r)
{
    els_func_code *f = fs->f;
    int c = f->nknum;
    int lim = c < LOOKBACKNUMS ? 0 : c - LOOKBACKNUMS;
    while (--c >= lim)
        if (f->knum[c] == r)
            return c;
    els_Mem_growvector(fs->L, f->knum, f->nknum, 1, Number, "内存单元溢出", MAX_ARG_U);
    c = f->nknum++;
    f->knum[c] = r;
    return c;
}

void els_codegen_number(ElsFuncObj *fs, Number f)
{
    els_codegen_code_arg1(fs, BYTECODE_PUSHNUM, number_constant(fs, f));
}

void els_codegen_adjuststack(ElsFuncObj *fs, int n)
{
    if (n > 0)
        els_codegen_code_arg1(fs, BYTECODE_POP, n);
    else
        els_codegen_code_arg1(fs, BYTECODE_PUSHNULL, -n);
}

int els_codegen_lastisopen(ElsFuncObj *fs)
{
    Instruction i = next_instruction(fs);
    if (GET_OPCODE(i) == BYTECODE_CALL && GETARG_B(i) == MULT_RET)
        return 1;
    else
        return 0;
}

void els_codegen_setcallreturns(ElsFuncObj *fs, int nresults)
{
    if (els_codegen_lastisopen(fs))
    {
        SETARG_B(fs->f->code[fs->pc - 1], nresults);
        els_codegen_deltastack(fs, nresults);
    }
}

static int discharge(ElsFuncObj *fs, expdesc *var)
{
    switch (var->k)
    {
    case VLOCAL:
        els_codegen_code_arg1(fs, BYTECODE_GETLOCAL, var->u.index);
        break;
    case VGLOBAL:
        els_codegen_code_arg1(fs, BYTECODE_GETGLOBAL, var->u.index);
        break;
    case VINDEXED:
        els_codegen_code_arg0(fs, BYTECODE_GETUNIT);
        break;
    case VEXP:
        return 0;
    }
    var->k = VEXP;
    var->u.l.t = var->u.l.f = NO_JUMP;
    return 1;
}

static void discharge1(ElsFuncObj *fs, expdesc *var)
{
    discharge(fs, var);
    if (var->u.l.t == NO_JUMP && var->u.l.f == NO_JUMP)
        els_codegen_setcallreturns(fs, 1);
}

void els_codegen_storevar(LexObject *ls, const expdesc *var)
{
    ElsFuncObj *fs = ls->fs;
    switch (var->k)
    {
    case VLOCAL:
        els_codegen_code_arg1(fs, BYTECODE_SETLOCAL, var->u.index);
        break;
    case VGLOBAL:
        els_codegen_code_arg1(fs, BYTECODE_SETGLOBAL, var->u.index);
        break;
    case VINDEXED:
        els_codegen_code_arg2(fs, BYTECODE_SETUNIT, 3, 3);
        break;
    default:;
    }
}

static ByteIR invertjump(ByteIR op)
{
    switch (op)
    {
    case BYTECODE_JMPNE:
        return BYTECODE_JMPEQ;
    case BYTECODE_JMPEQ:
        return BYTECODE_JMPNE;
    case BYTECODE_JMPLT:
        return BYTECODE_JMPGE;
    case BYTECODE_JMPLE:
        return BYTECODE_JMPGT;
    case BYTECODE_JMPGT:
        return BYTECODE_JMPLE;
    case BYTECODE_JMPGE:
        return BYTECODE_JMPLT;
    case BYTECODE_JMPT:
    case BYTECODE_JMPONT:
        return BYTECODE_JMPF;
    case BYTECODE_JMPF:
    case BYTECODE_JMPONF:
        return BYTECODE_JMPT;
    default:
        return BYTECODE_END;
    }
}

static void els_codegen_patchlistaux(ElsFuncObj *fs, int list, int target, ByteIR special, int special_target)
{
    Instruction *code = fs->f->code;
    while (list != NO_JUMP)
    {
        int next = els_codegen_getjump(fs, list);
        Instruction *i = &code[list];
        ByteIR op = GET_OPCODE(*i);
        if (op == special)
            els_codegen_fixjump(fs, list, special_target);
        else
        {
            els_codegen_fixjump(fs, list, target);
            if (op == BYTECODE_JMPONT)
                SET_OPCODE(*i, BYTECODE_JMPT);
            else if (op == BYTECODE_JMPONF)
                SET_OPCODE(*i, BYTECODE_JMPF);
        }
        list = next;
    }
}

void els_codegen_patchlist(ElsFuncObj *fs, int list, int target)
{
    if (target == fs->lasttarget)
        els_codegen_concat(fs, &fs->jlt, list);
    else
        els_codegen_patchlistaux(fs, list, target, BYTECODE_END, 0);
}

static int need_value(ElsFuncObj *fs, int list, ByteIR hasvalue)
{
    for (; list != NO_JUMP; list = els_codegen_getjump(fs, list))
        if (GET_OPCODE(fs->f->code[list]) != hasvalue)
            return 1;
    return 0;
}

void els_codegen_concat(ElsFuncObj *fs, int *l1, int l2)
{
    if (*l1 == NO_JUMP)
        *l1 = l2;
    else
    {
        int list = *l1;
        while(1)
        {
            int next = els_codegen_getjump(fs, list);
            if (next == NO_JUMP)
            {
                els_codegen_fixjump(fs, list, l2);
                return;
            }
            list = next;
        }
    }
}

static void els_codegen_testgo(ElsFuncObj *fs, expdesc *v, int invert, ByteIR jump)
{
    int prevpos;
    Instruction *previous;
    int *golist, *exitlist;
    if (!invert)
    {
        golist = &v->u.l.f;
        exitlist = &v->u.l.t;
    }
    else
    {
        golist = &v->u.l.t;
        exitlist = &v->u.l.f;
    }
    discharge1(fs, v);
    prevpos = fs->pc - 1;
    previous = &fs->f->code[prevpos];
    if (!ISJUMP(GET_OPCODE(*previous)))
        prevpos = els_codegen_code_arg1(fs, jump, NO_JUMP);
    else
    {
        if (invert)
            SET_OPCODE(*previous, invertjump(GET_OPCODE(*previous)));
    }
    els_codegen_concat(fs, exitlist, prevpos);
    els_codegen_patchlist(fs, *golist, els_codegen_getlabel(fs));
    *golist = NO_JUMP;
}

void els_codegen_goiftrue(ElsFuncObj *fs, expdesc *v, int keepvalue)
{
    els_codegen_testgo(fs, v, 1, keepvalue ? BYTECODE_JMPONF : BYTECODE_JMPF);
}

static void els_codegen_goiffalse(ElsFuncObj *fs, expdesc *v, int keepvalue)
{
    els_codegen_testgo(fs, v, 0, keepvalue ? BYTECODE_JMPONT : BYTECODE_JMPT);
}

void els_codegen_tostack(LexObject *ls, expdesc *v, int onlyone)
{
    ElsFuncObj *fs = ls->fs;
    if (!discharge(fs, v))
    {
        ByteIR previous = GET_OPCODE(fs->f->code[fs->pc - 1]);
        if (!ISJUMP(previous) && v->u.l.f == NO_JUMP && v->u.l.t == NO_JUMP)
        {

            if (onlyone)
                els_codegen_setcallreturns(fs, 1);
        }
        else
        {
            int final;
            int j = NO_JUMP;
            int p_null = NO_JUMP;
            int p_1 = NO_JUMP;
            if (ISJUMP(previous) || need_value(fs, v->u.l.f, BYTECODE_JMPONF) || need_value(fs, v->u.l.t, BYTECODE_JMPONT))
            {

                if (ISJUMP(previous))
                    els_codegen_concat(fs, &v->u.l.t, fs->pc - 1);
                else
                {
                    j = code_label(fs, BYTECODE_JMP, NO_JUMP);

                    els_codegen_adjuststack(fs, 1);
                }
                p_null = code_label(fs, BYTECODE_PUSHNULLJMP, 0);
                p_1 = code_label(fs, BYTECODE_PUSHNUM, 1);
                els_codegen_patchlist(fs, j, els_codegen_getlabel(fs));
            }
            final = els_codegen_getlabel(fs);
            els_codegen_patchlistaux(fs, v->u.l.f, p_null, BYTECODE_JMPONF, final);
            els_codegen_patchlistaux(fs, v->u.l.t, p_1, BYTECODE_JMPONT, final);
            v->u.l.f = v->u.l.t = NO_JUMP;
        }
    }
}

void els_codegen_prefix(LexObject *ls, UnOpr op, expdesc *v)
{
    ElsFuncObj *fs = ls->fs;
    if (op == OPR_MINUS)
    {
        els_codegen_tostack(ls, v, 1);
        els_codegen_code_arg0(fs, BYTECODE_MINUS);
    }
    else
    {
        Instruction *previous;
        discharge1(fs, v);
        previous = &fs->f->code[fs->pc - 1];
        if (ISJUMP(GET_OPCODE(*previous)))
            SET_OPCODE(*previous, invertjump(GET_OPCODE(*previous)));
        else
            els_codegen_code_arg0(fs, BYTECODE_NOT);

        {
            int temp = v->u.l.f;
            v->u.l.f = v->u.l.t;
            v->u.l.t = temp;
        }
    }
}

void els_codegen_infix(LexObject *ls, BinOpr op, expdesc *v)
{
    ElsFuncObj *fs = ls->fs;
    switch (op)
    {
    case OPR_AND:
        els_codegen_goiftrue(fs, v, 1);
        break;
    case OPR_OR:
        els_codegen_goiffalse(fs, v, 1);
        break;
    default:
        els_codegen_tostack(ls, v, 1);
    }
}


void els_codegen_posfix(LexObject *ls, BinOpr op, expdesc *v1, expdesc *v2)
{
    ElsFuncObj *fs = ls->fs;
    switch (op)
    {
    case OPR_AND:
    {
        discharge1(fs, v2);
        v1->u.l.t = v2->u.l.t;
        els_codegen_concat(fs, &v1->u.l.f, v2->u.l.f);
        break;
    }
    case OPR_OR:
    {
        discharge1(fs, v2);
        v1->u.l.f = v2->u.l.f;
        els_codegen_concat(fs, &v1->u.l.t, v2->u.l.t);
        break;
    }
    default:
    {
        els_codegen_tostack(ls, v2, 1);
        els_codegen_code_arg1(fs, codes[op].opcode, codes[op].arg);
    }
    }
}

static void codelineinfo(ElsFuncObj *fs)
{
    els_func_code *f = fs->f;
    LexObject *ls = fs->ls;
    if (ls->lastline > fs->lastline)
    {
        els_Mem_growvector(fs->L, f->lineinfo, f->nlineinfo, 2, int,
                        "代码行数溢出", MAX_INT);
        if (ls->lastline > fs->lastline + 1)
            f->lineinfo[f->nlineinfo++] = -(ls->lastline - (fs->lastline + 1));
        f->lineinfo[f->nlineinfo++] = fs->pc;
        fs->lastline = ls->lastline;
    }
}

int els_codegen_code_arg2(ElsFuncObj *fs, ByteIR o, int arg1, int arg2)
{
    Instruction i = next_instruction(fs);
    int delta = els_codegen_bytecodesubframe[o].push - els_codegen_bytecodesubframe[o].pop;
    int optm = 0;
    switch (o)
    {
    case BYTECODE_PUSHFUNCTION:
    {
        delta = -arg2 + 1;
        break;
    }
    case BYTECODE_SETUNIT:
    {
        delta = -arg2;
        break;
    }
    case BYTECODE_SETLIST:
    {
        if (arg2 == 0)
            return NO_JUMP;
        delta = -arg2;
        break;
    }
    case BYTECODE_SETMAP:
    {
        if (arg1 == 0)
            return NO_JUMP;
        delta = -2 * arg1;
        break;
    }
    case BYTECODE_PUSHNULL:
    {
        if (arg1 == 0)
            return NO_JUMP;
        delta = arg1;
        switch (GET_OPCODE(i))
        {
        case BYTECODE_PUSHNULL:
            SETARG_U(i, GETARG_U(i) + arg1);
            optm = 1;
            break;
        default:
            break;
        }
        break;
    }
    case BYTECODE_POP:
    {
        if (arg1 == 0)
            return NO_JUMP;
        delta = -arg1;
        switch (GET_OPCODE(i))
        {
        case BYTECODE_SETUNIT:
            SETARG_B(i, GETARG_B(i) + arg1);
            optm = 1;
            break;
        default:
            break;
        }
        break;
    }
    case BYTECODE_JMPNE:
    {
        if (i == CREATE_U(BYTECODE_PUSHNULL, 1))
        {
            i = CREATE_S(BYTECODE_JMPT, NO_JUMP);
            optm = 1;
        }
        break;
    }
    case BYTECODE_JMPEQ:
    {
        if (i == CREATE_U(BYTECODE_PUSHNULL, 1))
        {
            i = CREATE_0(BYTECODE_NOT);
            delta = -1;
            optm = 1;
        }
        break;
    }
    case BYTECODE_JMPT:
    case BYTECODE_JMPONT:
    {
        switch (GET_OPCODE(i))
        {
        case BYTECODE_NOT:
        {
            i = CREATE_S(BYTECODE_JMPF, NO_JUMP);
            optm = 1;
            break;
        }
        case BYTECODE_PUSHNULL:
        {
            if (GETARG_U(i) == 1)
            {
                fs->pc--;
                els_codegen_deltastack(fs, -1);
                return NO_JUMP;
            }
            break;
        }
        default:
            break;
        }
        break;
    }
    case BYTECODE_JMPF:
    case BYTECODE_JMPONF:
    {
        switch (GET_OPCODE(i))
        {
        case BYTECODE_NOT:
        {
            i = CREATE_S(BYTECODE_JMPT, NO_JUMP);
            optm = 1;
            break;
        }
        case BYTECODE_PUSHNULL:
        {
            if (GETARG_U(i) == 1)
            {
                i = CREATE_S(BYTECODE_JMP, NO_JUMP);
                optm = 1;
            }
            break;
        }
        default:
            break;
        }
        break;
    }
    default:break;
    }
    els_codegen_deltastack(fs, delta);
    if (optm)
    {
        fs->f->code[fs->pc - 1] = i;
        return fs->pc - 1;
    }

    switch ((enum Mode)els_codegen_bytecodesubframe[o].mode)
    {
    case iO:
        i = CREATE_0(o);
        break;
    case iU:
        i = CREATE_U(o, arg1);
        break;
    case iS:
        i = CREATE_S(o, arg1);
        break;
    case iAB:
        i = CREATE_AB(o, arg1, arg2);
        break;
    }
    codelineinfo(fs);

    els_Mem_growvector(fs->L, fs->f->code, fs->pc, 1, Instruction,"字节码溢出", MAX_INT);
    fs->f->code[fs->pc] = i;
    return fs->pc++;
}
