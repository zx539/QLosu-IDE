#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "els.h"

#include "els_codegen.h"
#include "els_func.h"
#include "els_lex.h"
#include "els_mem.h"
#include "els_object.h"
#include "els_bytecode.h"
#include "els_parser.h"
#include "els_vmhost.h"
#include "els_string.h"

typedef struct Constdesc
{
    int n;
    int k;
} Constdesc;

typedef struct Breaklabel
{
    struct Breaklabel *previous;
    int breaklist;
    int stacklevel;
} Breaklabel;
static const struct
{
    char left;
    char right;
} sum_sign_exp[] = {
    {5, 5}, {5, 5}, {6, 6}, {6, 6}, {9, 8}, {4, 3}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {2, 2}, {1, 1}, {1, 1}};

#define UNARY_PRIORITY 7
#define code_string(ls, s) (els_codegen_kstr(ls, string_constant(ls->fs, s)))
#define lookahead(ls) (ls->lookahead.token = els_lexer_lex(ls, &(ls->lookahead.seminfo)))
#define new_localvarstr(ls, name, n) (new_localvar(ls, els_string_newfixed(ls->L, name), n))
#define expersion(ls, v) (subexpr(ls, v, -1))
#define condition(ls, v) (expersion(ls, v), els_codegen_goiftrue(ls->fs, v, 0))
#define optional(ls, c) (ls->t.token == c ? (next(ls), 1) : 0)

static void els_parser_error_expected(LexObject *ls, int token);
static void check(LexObject *ls, int c);
static void next(LexObject *ls);
static void check_condition(LexObject *ls, int c, const char *msg);
static void check_match(LexObject *ls, int l, int r, int line);
static int string_constant(ElsFuncObj *fs, TString *s);
static int checkname(LexObject *ls);
static TString *checkstrname(LexObject *ls);
static int els_parser_registerlocalvar(LexObject *ls, TString *varname);
static void new_localvar(LexObject *ls, TString *name, int n);
static void adjustlocalvars(LexObject *ls, int nvars);
static void removelocalvars(LexObject *ls, int nvars);
static int check_iflocal(LexObject *ls, TString *n, expdesc *var);
static void singlevar(LexObject *ls, TString *n, expdesc *var);
static void adjust_mult_assign(LexObject *ls, int nvars, int nexps);
static void enterbreak(ElsFuncObj *fs, Breaklabel *bl);
static void leavebreak(ElsFuncObj *fs, Breaklabel *bl);
static void push_csfunciotn(LexObject *ls, ElsFuncObj *func);
static void open_func(LexObject *ls, ElsFuncObj *fs);
static void close_func(LexObject *ls);
els_func_code *els_parser_parse(els_VmObj *L, vm_iobuff *z);
static int explist1(LexObject *ls);
static void funcargs(LexObject *ls, int slf);
static void var_or_func(LexObject *ls, expdesc *v);
static int recfield(LexObject *ls);
static int listfields(LexObject *ls);
static void constructor(LexObject *ls);
static void simpleexp(LexObject *ls, expdesc *v);
static void exp1(LexObject *ls);
static int getunopr(int op);
static int getbinopr(int op);
static int subexpr(LexObject *ls, expdesc *v, int limit);
static void block(LexObject *ls);
static int check_block(int token);
static int assignment(LexObject *ls, expdesc *v, int nvars);
static void forbody(LexObject *ls, int nvar, ByteIR prepfor, ByteIR loopfor);
static void fornum(LexObject *ls, TString *varname);
static void forlist(LexObject *ls, TString *indexname);
static void test_then_block(LexObject *ls, expdesc *v);
static void parlist(LexObject *ls);
static void body(LexObject *ls, int needself, int line);

static int els_parser_stat(LexObject *ls);
static void els_parser_stat_with(LexObject *ls, int line);
static void els_parser_stat_loop(LexObject *ls, int line);
static void els_parser_stat_for(LexObject *ls, int line);
static void els_parser_stat_if(LexObject *ls, int line);
static void els_parser_stat_func(LexObject *ls, int line);
static void els_parser_stat_name(LexObject *ls);
static void els_parser_stat_return(LexObject *ls);
static void els_parser_stat_break(LexObject *ls);

void els_compiler_error(LexObject *ls, const char *buff)
{
    

#ifdef ELS_CONF_CHAR_GBK
    char tmp[256];
    char* t;
    sprintf(tmp,"编译错误: \t%s\n影响到第 \t%d\t行\n", buff, ls->linenumber);
    t = vm_win_togbk(tmp);
    printf("%s",t);
    free(t);
#else
    printf("编译错误: \t%s\n影响到第 \t%d\t行\n", buff, ls->linenumber);
#endif
    els_Heap_breakrun(ls->L, ELS_ERRORBACK_SYNTAX);
}
els_func_code *els_parser_parse(els_VmObj *L, vm_iobuff *z)
{
    struct LexObject lexstate;
    struct ElsFuncObj els_parser_stat_funce;
    lexstate.deepth = 0;
    int islast = 0;
    els_lexer_setinput(L, &lexstate, z, els_string_new(L, els_vmio_name(z)));
    open_func(&lexstate, &els_parser_stat_funce);
    next(&lexstate);
    /* this UB will remain in comments，to warn me
    * if you use this code and Compile with Compiler likes(gcc version 11.3.0 (Ubuntu 11.3.0-1ubuntu1~22.04) )
    *
    * using script to test ' for i=1,100 :;',you will own an error
    *
    while (!els_parser_stat(&lexstate) && !check_block(((LexObject*)(&lexstate))->t.token));
    */
    while (!islast && !check_block(((LexObject *)(&lexstate))->t.token))
        islast = els_parser_stat(((LexObject *)(&lexstate)));
    check_condition(&lexstate, (lexstate.t.token == TOKEN_EOS), "缺少结束符号");
    close_func(&lexstate);
    return els_parser_stat_funce.f;
}
static void open_func(LexObject *ls, ElsFuncObj *fs)
{
    els_func_code *f = els_ScriptFunc_newcodeir(ls->L);
    fs->prev = ls->fs;
    fs->ls = ls;
    fs->L = ls->L;
    ls->fs = fs;
    fs->stacklevel = 0;
    fs->nactloc = 0;
    fs->nupvalues = 0;
    fs->bl = NULL;
    fs->f = f;
    f->source = ls->source;
    fs->pc = 0;
    fs->lasttarget = 0;
    fs->lastline = 0;
    fs->jlt = NO_JUMP;
    f->code = NULL;
    f->maxstacksize = 0;
    f->numparams = 0;
    f->is_vararg = 0;
}
static void close_func(LexObject *ls)
{
    els_VmObj *L = ls->L;
    ElsFuncObj *fs = ls->fs;
    els_func_code *f = fs->f;
    els_codegen_code_arg0(fs, BYTECODE_END);
    els_codegen_getlabel(fs);
    els_Mem_reallocvector(L, f->code, fs->pc, Instruction);
    els_Mem_reallocvector(L, f->kstr, f->nkstr, TString *);
    els_Mem_reallocvector(L, f->knum, f->nknum, Number);
    els_Mem_reallocvector(L, f->kcodeir, f->nkcodeir, els_func_code *);
    removelocalvars(ls, fs->nactloc);
    els_Mem_reallocvector(L, f->locvars, f->nlocvars, LocVar);
    els_Mem_reallocvector(L, f->lineinfo, f->nlineinfo + 1, int);
    f->lineinfo[f->nlineinfo++] = MAX_INT;
    els_ScriptFunc_end(L, f, fs->pc);
    ls->fs = fs->prev;
}
static void els_parser_error_expected(LexObject *ls, int token)
{
    char buff[128], t[9];
    els_lexer_token2str(token, t);
    sprintf(buff, "在 %d 行，缺少 `%s' ", ls->linenumber, t);
    els_compiler_error(ls, buff);
}
static void check(LexObject *ls, int c)
{
    if (ls->t.token != c)
        els_parser_error_expected(ls, c);
    next(ls);
}
static void next(LexObject *ls)
{
    ls->lastline = ls->linenumber;
    if (ls->lookahead.token != TOKEN_EOS)
    {
        ls->t = ls->lookahead;
        ls->lookahead.token = TOKEN_EOS;
    }
    else
        ls->t.token = els_lexer_lex(ls, &(ls->t.seminfo));
}
static void check_condition(LexObject *ls, int c, const char *msg)
{
    char err[128];
    sprintf(err, "%s,在第 %d 行", msg, ls->lastline);
    if (!c)
        els_codegen_error(ls, err);
}
static void check_match(LexObject *ls, int l, int r, int line)
{
    if (ls->t.token != l)
    {
        if (line == ls->linenumber)
            els_parser_error_expected(ls, l);
        else
        {
            char buff[128], tl[8], tr[8];
            els_lexer_token2str(l, tl);
            els_lexer_token2str(r, tr);
            sprintf(buff, "在第 %d 行，缺少 `%.20s' 以匹配 `%.20s' ", line, tl, tr);
            els_codegen_error(ls, buff);
        }
    }
    next(ls);
}
static int string_constant(ElsFuncObj *fs, TString *s)
{
    els_func_code *f = fs->f;
    int c = s->u.s.constindex;
    if (c >= f->nkstr || f->kstr[c] != s)
    {
        els_Mem_growvector(fs->L, f->kstr, f->nkstr, 1, TString *, "系统内置索引数组溢出", MAX_ARG_U);
        c = f->nkstr++;
        f->kstr[c] = s;
        s->u.s.constindex = c;
    }
    return c;
}
static int checkname(LexObject *ls)
{
    return string_constant(ls->fs, checkstrname(ls));
}
static TString *checkstrname(LexObject *ls)
{
    TString *ts;
    check_condition(ls, (ls->t.token == TOKEN_NAME), "缺少变量名");
    ts = ls->t.seminfo.ts;
    next(ls);
    return ts;
}
static int els_parser_registerlocalvar(LexObject *ls, TString *varname)
{
    els_func_code *f = ls->fs->f;
    els_Mem_growvector(ls->L, f->locvars, f->nlocvars, 1, LocVar, "", MAX_INT);
    f->locvars[f->nlocvars].varname = varname;
    return f->nlocvars++;
}
static void new_localvar(LexObject *ls, TString *name, int n)
{
    ElsFuncObj *fs = ls->fs;
    fs->actloc[fs->nactloc + n] = els_parser_registerlocalvar(ls, name);
}
static void adjustlocalvars(LexObject *ls, int nvars)
{
    ElsFuncObj *fs = ls->fs;
    while (nvars--)
        fs->f->locvars[fs->actloc[fs->nactloc++]].startpc = fs->pc;
}
static void removelocalvars(LexObject *ls, int nvars)
{
    ElsFuncObj *fs = ls->fs;
    while (nvars--)
        fs->f->locvars[fs->actloc[--fs->nactloc]].endpc = fs->pc;
}
static int check_iflocal(LexObject *ls, TString *n, expdesc *var)
{
    ElsFuncObj *fs;
    int level = 0;
    for (fs = ls->fs; fs; fs = fs->prev)
    {
        int i;
        for (i = fs->nactloc - 1; i >= 0; i--)
        {
            if (n == fs->f->locvars[fs->actloc[i]].varname)
            {
                var->k = VLOCAL;
                var->u.index = i;
                return level;
            }
        }
        level++;
    }
    var->k = VGLOBAL;
    return -1;
}

static int indexupvalue(LexObject *ls, expdesc *v)
{
    ElsFuncObj *fs = ls->fs;
    int i;
    for (i = 0; i < fs->nupvalues; i++)
    {
        if (fs->upvalues[i].k == v->k && fs->upvalues[i].u.index == v->u.index)
            return i;
    }
    fs->upvalues[fs->nupvalues] = *v;
    return fs->nupvalues++;
}
static void pushupvalue(LexObject *ls, TString *n)
{
    ElsFuncObj *fs = ls->fs;
    expdesc v;
    int level = check_iflocal(ls, n, &v);
    if (level == -1) /* global */
        v.u.index = string_constant(fs->prev, n);
    else if (level != 1)
    {
        char tmp[128];
        sprintf(tmp, "闭包变量必须是全局变量 或 位于 上一级作用域中 ,变量 %s 不满足此条件，在第 %d 行", n->str, ls->linenumber - 1);
        els_compiler_error(ls, tmp);
    }
    els_codegen_code_arg1(fs, BYTECODE_PUSHUPVALUE, indexupvalue(ls, &v));
}
static void singlevar(LexObject *ls, TString *n, expdesc *var)
{

    int level = check_iflocal(ls, n, var);
    if (level >= 1)
    {
        pushupvalue(ls, n);
        var->k = VEXP;
        var->u.l.t = var->u.l.f = NO_JUMP;
    }
    else if (level == -1)
        var->u.index = string_constant(ls->fs, n);
}

static void adjust_mult_assign(LexObject *ls, int nvars, int nexps)
{
    ElsFuncObj *fs = ls->fs;
    int diff = nexps - nvars;
    if (nexps > 0 && els_codegen_lastisopen(fs))
    {
        diff--;
        if (diff <= 0)
        {
            els_codegen_setcallreturns(fs, -diff);
            diff = 0;
        }
        else
            els_codegen_setcallreturns(fs, 0);
    }

    els_codegen_adjuststack(fs, diff);
}

static void enterbreak(ElsFuncObj *fs, Breaklabel *bl)
{
    bl->stacklevel = fs->stacklevel;
    bl->breaklist = NO_JUMP;
    bl->previous = fs->bl;
    fs->bl = bl;
}
static void leavebreak(ElsFuncObj *fs, Breaklabel *bl)
{
    fs->bl = bl->previous;

    els_codegen_patchlist(fs, bl->breaklist, els_codegen_getlabel(fs));
}
static void push_csfunciotn(LexObject *ls, ElsFuncObj *func)
{
    ElsFuncObj *fs = ls->fs;
    els_func_code *f = fs->f;
    int i;
    for (i = 0; i < func->nupvalues; i++)
        els_codegen_tostack(ls, &func->upvalues[i], 1);
    els_Mem_growvector(ls->L, f->kcodeir, f->nkcodeir, 1, els_func_code *,
                       "单位对象溢出", MAX_ARG_A);
    f->kcodeir[f->nkcodeir++] = func->f;
    els_codegen_code_arg2(fs, BYTECODE_PUSHFUNCTION, f->nkcodeir - 1, func->nupvalues);
}
static int explist1(LexObject *ls)
{

    int n = 1;
    expdesc v;
    expersion(ls, &v);
    while (ls->t.token == ',')
    {
        els_codegen_tostack(ls, &v, 1);
        next(ls);
        expersion(ls, &v);
        n++;
    }
    els_codegen_tostack(ls, &v, 0);
    return n;
}
static void funcargs(LexObject *ls, int slf)
{
    ElsFuncObj *fs = ls->fs;
    int slevel = fs->stacklevel - slf - 1;
    int line = ls->linenumber;
    switch (ls->t.token)
    {
    case '(':
    {

        next(ls);
        if (ls->t.token != ')')
            explist1(ls);
        check_match(ls, ')', '(', line);
        break;
    }
    case '{':
    {
        constructor(ls);
        break;
    }
    case TOKEN_STRING:
    {
        code_string(ls, ls->t.seminfo.ts);
        next(ls);
        break;
    }
    default:
    {
        els_codegen_error(ls, "缺少函数参数");
        break;
    }
    }
    fs->stacklevel = slevel;
    els_codegen_code_arg2(fs, BYTECODE_CALL, slevel, MULT_RET);
}
static void var_or_func(LexObject *ls, expdesc *v)
{
    singlevar(ls, checkstrname(ls), v);
    while (1)
    {
        switch (ls->t.token)
        {
        case '.':
        {
            next(ls);
            lookahead(ls);
            if (ls->lookahead.token == '(')
            {
                int name = checkname(ls);
                els_codegen_tostack(ls, v, 1);
                els_codegen_code_arg1(ls->fs, BYTECODE_PUSHSELF, name);
                funcargs(ls, 1);
                v->k = VEXP;
                v->u.l.t = v->u.l.f = NO_JUMP;
                break;
            }
            else
            {
                els_codegen_tostack(ls, v, 1);
                els_codegen_kstr(ls, checkname(ls));
                v->k = VINDEXED;
                break;
            }
        }
        case '[':
        {
            next(ls);
            els_codegen_tostack(ls, v, 1);
            v->k = VINDEXED;
            exp1(ls);
            check(ls, ']');
            break;
        }
        case '(':
        case TOKEN_STRING:
        case '{':
        {
            els_codegen_tostack(ls, v, 1);
            funcargs(ls, 0);
            v->k = VEXP;
            v->u.l.t = v->u.l.f = NO_JUMP;
            break;
        }
        default:
            return;
        }
    }
}
static int recfield(LexObject *ls)
{

    ElsFuncObj *fs = ls->fs;
    int n = 1;
    switch (ls->t.token)
    {
    case TOKEN_NAME:
    {
        els_codegen_kstr(ls, checkname(ls));
        break;
    }
    case '[':
    {
        next(ls);
        exp1(ls);
        check(ls, ']');
        break;
    }
    default:
        els_codegen_error(ls, "缺少 变量名 或 '['");
    }
    check(ls, '=');
    exp1(ls);
    while (ls->t.token == ',')
    {
        next(ls);
        if (ls->t.token == '}')
            break;
        switch (ls->t.token)
        {
        case TOKEN_NAME:
        {
            els_codegen_kstr(ls, checkname(ls));
            break;
        }
        case '[':
        {
            next(ls);
            exp1(ls);
            check(ls, ']');
            break;
        }
        default:
            els_codegen_error(ls, "缺少 变量名 或 '['");
        }
        check(ls, '=');
        exp1(ls);
        n++;
        if (n % RFIELDS_PER_FLUSH == 0)
            els_codegen_code_arg1(fs, BYTECODE_SETMAP, RFIELDS_PER_FLUSH);
    }
    els_codegen_code_arg1(fs, BYTECODE_SETMAP, n % RFIELDS_PER_FLUSH);
    return n;
}
static int listfields(LexObject *ls)
{

    ElsFuncObj *fs = ls->fs;
    int n = 1;
    exp1(ls);
    while (ls->t.token == ',')
    {
        next(ls);
        if (ls->t.token == '}')
            break;
        exp1(ls);
        n++;
        if (n % LFIELDS_PER_FLUSH == 0)
            els_codegen_code_arg2(fs, BYTECODE_SETLIST, n / LFIELDS_PER_FLUSH - 1, LFIELDS_PER_FLUSH);
    }
    els_codegen_code_arg2(fs, BYTECODE_SETLIST, n / LFIELDS_PER_FLUSH, n % LFIELDS_PER_FLUSH);
    return n;
}
static void constructor(LexObject *ls)
{

    ElsFuncObj *fs = ls->fs;
    int line = ls->linenumber;
    int pc = els_codegen_code_arg1(fs, BYTECODE_CREATEUNIT, 0);
    int issues_num;
    Constdesc cd;
    check(ls, '{');
    switch (ls->t.token)
    {
    case '}':
    {
        cd.n = 0;
        cd.k = ls->t.token;
        break;
    }
    case TOKEN_NAME:
    {

        lookahead(ls);
        if (ls->lookahead.token != '=')
            goto case_default;
    }
    case '[':
    {
        cd.n = recfield(ls);
        cd.k = 1;
        break;
    }
    default:
    {
    case_default:
        cd.n = listfields(ls);
        cd.k = 0;
        break;
    }
    }
    issues_num = cd.n;
    check_match(ls, '}', '{', line);
    SETARG_U(fs->f->code[pc], issues_num);
}
static void simpleexp(LexObject *ls, expdesc *v)
{
    ElsFuncObj *fs = ls->fs;
    switch (ls->t.token)
    {
    case TOKEN_NUMBER:
    {
        Number r = ls->t.seminfo.r;
        next(ls);
        els_codegen_number(fs, r);
        break;
    }
    case TOKEN_STRING:
    {
        code_string(ls, ls->t.seminfo.ts);
        next(ls);
        break;
    }
    case TOKEN_NULL:
    {
        els_codegen_adjuststack(fs, -1);
        next(ls);
        break;
    }
    case '{':
    {
        constructor(ls);
        break;
    }
    case TOKEN_FUNCTION:
    {
        ls->deepth++;
        next(ls);
        body(ls, 0, ls->linenumber);
        ls->deepth--;
        break;
    }
    case '(':
    {
        next(ls);
        expersion(ls, v);
        check(ls, ')');
        return;
    }
    case TOKEN_NAME:
    {
        var_or_func(ls, v);
        return;
    }
    default:
    {
        char tmp[128];
        sprintf(tmp, "错误的表达式,在第 %d 行", ls->linenumber);
        els_codegen_error(ls, tmp);
        return;
    }
    }
    v->k = VEXP;
    v->u.l.t = v->u.l.f = NO_JUMP;
}
static void exp1(LexObject *ls)
{
    expdesc v;
    expersion(ls, &v);
    els_codegen_tostack(ls, &v, 1);
}
static int getunopr(int op)
{
    switch (op)
    {
    case TOKEN_NOT:
        return OPR_NOT;
    case '-':
        return OPR_MINUS;
    default:
        return OPR_NOUNOPR;
    }
}
static int getbinopr(int op)
{
    switch (op)
    {
    case '+':
        return OPR_ADD;
    case '-':
        return OPR_SUB;
    case '*':
        return OPR_MULT;
    case '/':
        return OPR_DIV;
    case '^':
        return OPR_POW;
    case TOKEN_CONCAT:
        return OPR_CONCAT;
    case TOKEN_NE:
        return OPR_NE;
    case TOKEN_EQ:
        return OPR_EQ;
    case '<':
        return OPR_LT;
    case TOKEN_LE:
        return OPR_LE;
    case '>':
        return OPR_GT;
    case TOKEN_GE:
        return OPR_GE;
    case TOKEN_AND:
        return OPR_AND;
    case TOKEN_OR:
        return OPR_OR;
    default:
        return OPR_NOBINOPR;
    }
}
static int subexpr(LexObject *ls, expdesc *v, int limit)
{
    int op;
    int uop = getunopr(ls->t.token);
    if (uop != OPR_NOUNOPR)
    {
        next(ls);
        subexpr(ls, v, UNARY_PRIORITY);
        els_codegen_prefix(ls, uop, v);
    }
    else
        simpleexp(ls, v);

    op = getbinopr(ls->t.token);
    while (op != OPR_NOBINOPR && sum_sign_exp[op].left > limit)
    {
        expdesc v2;
        int nextop;
        next(ls);
        els_codegen_infix(ls, op, v);

        nextop = subexpr(ls, &v2, sum_sign_exp[op].right);
        els_codegen_posfix(ls, op, v, &v2);
        op = nextop;
    }
    return op;
}
static void block(LexObject *ls)
{

    ElsFuncObj *fs = ls->fs;
    int nactloc = fs->nactloc;
    int islast = 0;
    while (!islast && !check_block(ls->t.token))
        islast = els_parser_stat(ls);
    els_codegen_adjuststack(fs, fs->nactloc - nactloc);
    removelocalvars(ls, fs->nactloc - nactloc);
}
static int check_block(int token)
{
    switch (token)
    {
    case TOKEN_ELSE:
    case TOKEN_ELSEIF:
    case TOKEN_END:
    case TOKEN_TO:
    case TOKEN_EOS:
        return 1;
    default:
        return 0;
    }
}
static int assignment(LexObject *ls, expdesc *v, int nvars)
{
    int left = 0;
    if (ls->t.token == ',')
    {
        expdesc nv;
        next(ls);
        var_or_func(ls, &nv);
        check_condition(ls, (nv.k != VEXP), "语法错误");
        left = assignment(ls, &nv, nvars + 1);
    }
    else
    {
        int nexps;
        if (ls->t.token == '=')
        {
            next(ls);
            nexps = explist1(ls);
        }
        else
        {
            nexps = 0;
        }

        adjust_mult_assign(ls, nvars, nexps);
    }
    if (v->k != VINDEXED)
        els_codegen_storevar(ls, v);
    else
    {
        els_codegen_code_arg2(ls->fs, BYTECODE_SETUNIT, left + nvars + 2, 1);
        left += 2;
    }
    return left;
}
static void els_parser_stat_with(LexObject *ls, int line)
{

    ElsFuncObj *fs = ls->fs;
    int while_init = els_codegen_getlabel(fs);
    expdesc v;
    Breaklabel bl;
    enterbreak(fs, &bl);
    next(ls);
    condition(ls, &v);
    check(ls, ':');
    block(ls);
    els_codegen_patchlist(fs, els_codegen_jump(fs), while_init);
    els_codegen_patchlist(fs, v.u.l.f, els_codegen_getlabel(fs));
    check_match(ls, TOKEN_END, TOKEN_WITH, line);
    leavebreak(fs, &bl);
}
static void els_parser_stat_loop(LexObject *ls, int line)
{

    ElsFuncObj *fs = ls->fs;
    int repeat_init = els_codegen_getlabel(fs);
    expdesc v;
    Breaklabel bl;
    enterbreak(fs, &bl);
    next(ls);
    block(ls);
    check_match(ls, TOKEN_TO, TOKEN_LOOP, line);
    condition(ls, &v);
    els_codegen_patchlist(fs, v.u.l.f, repeat_init);
    leavebreak(fs, &bl);
}
static void forbody(LexObject *ls, int nvar, ByteIR prepfor, ByteIR loopfor)
{

    ElsFuncObj *fs = ls->fs;
    int prep = els_codegen_code_arg1(fs, prepfor, NO_JUMP);
    int blockinit = els_codegen_getlabel(fs);
    check(ls, ':');
    adjustlocalvars(ls, nvar);
    block(ls);
    els_codegen_patchlist(fs, els_codegen_code_arg1(fs, loopfor, NO_JUMP), blockinit);
    els_codegen_patchlist(fs, prep, els_codegen_getlabel(fs));
    removelocalvars(ls, nvar);
}
static void fornum(LexObject *ls, TString *varname)
{
    check(ls, '=');
    exp1(ls);
    check(ls, ',');
    exp1(ls);
    if (optional(ls, ','))
        exp1(ls);
    else
        els_codegen_number(ls->fs, 1);
    new_localvar(ls, varname, 0);
    new_localvarstr(ls, "__MAX__", 1);
    new_localvarstr(ls, "__STEP__", 2);
    forbody(ls, 3, BYTECODE_FORPREP, BYTECODE_FORLOOP);
}
static void forlist(LexObject *ls, TString *indexname)
{
    TString *valname;
    check(ls, ',');
    valname = checkstrname(ls);
    check_condition(ls, ls->t.token == '=' || (ls->t.token == TOKEN_NAME && ls->t.seminfo.ts == els_string_new(ls->L, "in")), "`缺少 = 或 in");
    next(ls);
    exp1(ls);
    new_localvarstr(ls, "__THIS__", 0);
    new_localvar(ls, indexname, 1);
    new_localvar(ls, valname, 2);
    forbody(ls, 3, BYTECODE_LFORPREP, BYTECODE_LFORLOOP);
}
static int funcname(LexObject *ls, expdesc *v)
{
    int i = 0;
    singlevar(ls, checkstrname(ls), v);
    while (ls->t.token == '.')
    {
        next(ls);
        els_codegen_tostack(ls, v, 1);
        els_codegen_kstr(ls, checkname(ls));
        v->k = VINDEXED;
        i = 1;
    }
    return i;
}

static void els_parser_stat_for(LexObject *ls, int line)
{

    ElsFuncObj *fs = ls->fs;
    TString *varname;
    Breaklabel bl;
    enterbreak(fs, &bl);
    next(ls);
    varname = checkstrname(ls);
    switch (ls->t.token)
    {
    case '=':
        fornum(ls, varname);
        break;
    case ',':
        forlist(ls, varname);
        break;
    default:
        els_codegen_error(ls, "缺少 '=' 或 ','");
    }
    check_match(ls, TOKEN_END, TOKEN_FOR, line);
    leavebreak(fs, &bl);
}
static void test_then_block(LexObject *ls, expdesc *v)
{

    next(ls);
    condition(ls, v);
    check(ls, ':');
    block(ls);
}
static void els_parser_stat_if(LexObject *ls, int line)
{

    ElsFuncObj *fs = ls->fs;
    expdesc v;
    int escapelist = NO_JUMP;
    test_then_block(ls, &v);
    while (ls->t.token == TOKEN_ELSEIF)
    {
        els_codegen_concat(fs, &escapelist, els_codegen_jump(fs));
        els_codegen_patchlist(fs, v.u.l.f, els_codegen_getlabel(fs));
        test_then_block(ls, &v);
    }
    if (ls->t.token == TOKEN_ELSE)
    {
        els_codegen_concat(fs, &escapelist, els_codegen_jump(fs));
        els_codegen_patchlist(fs, v.u.l.f, els_codegen_getlabel(fs));
        next(ls);
        block(ls);
    }
    else
        els_codegen_concat(fs, &escapelist, v.u.l.f);
    els_codegen_patchlist(fs, escapelist, els_codegen_getlabel(fs));
    check_match(ls, TOKEN_END, TOKEN_IF, line);
}
static void els_parser_stat_var(LexObject *ls)
{
    if (ls->deepth == 0)
    {
        next(ls);
        els_parser_stat_name(ls);
        return;
    }
    int nvars = 0;
    int nexps;
    do
    {
        next(ls);
        new_localvar(ls, checkstrname(ls), nvars++);
    } while (ls->t.token == ',');
    if (optional(ls, '='))
        nexps = explist1(ls);
    else
        nexps = 0;
    adjust_mult_assign(ls, nvars, nexps);
    adjustlocalvars(ls, nvars);
}
static void els_parser_stat_func(LexObject *ls, int line)
{
    expdesc v;
    next(ls);
    int i = funcname(ls, &v);
    body(ls, i, line);
    els_codegen_storevar(ls, &v);
}

static void els_parser_stat_name(LexObject *ls)
{

    ElsFuncObj *fs = ls->fs;
    expdesc v;
    var_or_func(ls, &v);
    if (v.k == VEXP)
    {
        check_condition(ls, els_codegen_lastisopen(fs), "语法错误");
        els_codegen_setcallreturns(fs, 0);
    }
    else
    {
        int left = assignment(ls, &v, 1);
        els_codegen_adjuststack(fs, left);
    }
}
static void els_parser_stat_return(LexObject *ls)
{

    ElsFuncObj *fs = ls->fs;
    next(ls);
    if (!check_block(ls->t.token))
        explist1(ls);
    els_codegen_code_arg1(fs, BYTECODE_RETURN, ls->fs->nactloc);
    fs->stacklevel = fs->nactloc;
}
static void els_parser_stat_break(LexObject *ls)
{

    ElsFuncObj *fs = ls->fs;
    int currentlevel = fs->stacklevel;
    Breaklabel *bl = fs->bl;
    next(ls);
    if (bl)
    {
        els_codegen_adjuststack(fs, currentlevel - bl->stacklevel);
        els_codegen_concat(fs, &bl->breaklist, els_codegen_jump(fs));
        els_codegen_adjuststack(fs, bl->stacklevel - currentlevel);
    }
}
static int els_parser_stat(LexObject *ls)
{
    char tmp[128];
    int line = ls->linenumber;
    switch (ls->t.token)
    {
    case TOKEN_IF:
    {
        els_parser_stat_if(ls, line);
        return 0;
    }
    case TOKEN_WITH:
    {
        els_parser_stat_with(ls, line);
        return 0;
    }
    case TOKEN_BLOCK:
    {
        ls->deepth++;
        next(ls);
        block(ls);
        check_match(ls, TOKEN_END, TOKEN_BLOCK, line);
        ls->deepth--;
        return 0;
    }
    case TOKEN_FOR:
    {
        els_parser_stat_for(ls, line);
        return 0;
    }
    case TOKEN_LOOP:
    {
        els_parser_stat_loop(ls, line);
        return 0;
    }
    case TOKEN_FUNCTION:
    {
        ls->deepth++;
        els_parser_stat_func(ls, line);
        ls->deepth--;
        return 0;
    }
    case TOKEN_VAR:
    {
        els_parser_stat_var(ls);
        return 0;
    }
    case TOKEN_NAME:
    case '%':
    {
        els_parser_stat_name(ls);
        return 0;
    }
    case TOKEN_RETURN:
    {
        els_parser_stat_return(ls);
        return 1;
    }
    case TOKEN_BREAK:
    {
        els_parser_stat_break(ls);
        return 1;
    }
    default:
    {
        sprintf(tmp, "错误的表达式,在第 %d 行", ls->linenumber);
        els_codegen_error(ls, tmp);
        return 0;
    }
    }
}

static void parlist(LexObject *ls)
{
    short is_vararg = 0;
    char tmp[128];
    int nparams = 0;
    if (ls->t.token != ')')
    {
        do
        {
            switch (ls->t.token)
            {
            case TOKEN_ARG:
                next(ls);
                is_vararg = 1;
                break;
            case TOKEN_NAME:
                new_localvar(ls, checkstrname(ls), nparams++);
                break;
            default:
            {
                sprintf(tmp, "缺少变量名,在第 %d 行", ls->linenumber);
                els_codegen_error(ls, tmp);
            }
            }
        } while (optional(ls, ','));
    }
    ElsFuncObj *fs = ls->fs;
    adjustlocalvars(ls, nparams);
    fs->f->numparams = fs->nactloc;
    fs->f->is_vararg = is_vararg;
    if (is_vararg)
    {
        new_localvarstr(ls, "arg", 0);
        adjustlocalvars(ls, 1);
    }
    els_codegen_deltastack(fs, fs->nactloc);
}

static void body(LexObject *ls, int needself, int line)
{
    ElsFuncObj new_fs;
    open_func(ls, &new_fs);
    new_fs.f->lineDefined = line;
    check(ls, '(');
    if (needself)
    {
        new_localvarstr(ls, "this", 0);
        adjustlocalvars(ls, 1);
    }
    parlist(ls);
    check(ls, ')');
    check(ls, ':');
    while (!check_block(ls->t.token))
        if (els_parser_stat(ls))
            break;
    check_match(ls, TOKEN_END, TOKEN_FUNCTION, line);
    close_func(ls);
    push_csfunciotn(ls, &new_fs);
}
