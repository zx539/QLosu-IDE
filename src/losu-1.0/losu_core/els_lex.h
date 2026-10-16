

#ifndef els_lex_h
#define els_lex_h

#include "els_object.h"
#include "els_vmcore.h"

#define FIRST_TOKENID 256

#define TOKEN_LEN 3 * 12
enum RESERVED
{

    TOKEN_AND = 256,
    TOKEN_BREAK,
    TOKEN_BLOCK,
    TOKEN_ELSE,
    TOKEN_ELSEIF,
    TOKEN_END,
    TOKEN_FOR,
    TOKEN_FUNCTION,
    TOKEN_IF,
    TOKEN_VAR,
    TOKEN_NULL,
    TOKEN_NOT,
    TOKEN_OR,
    TOKEN_LOOP,
    TOKEN_RETURN,
    TOKEN_THEN,
    TOKEN_TO,
    TOKEN_WITH,
    TOKEN_ARG,
    TOKEN_VARARG,
    /*18个*/

    TOKEN_NAME,
    TOKEN_CONCAT,
    TOKEN_BLOCKTS,
    TOKEN_EQ,
    TOKEN_GE,
    TOKEN_LE,
    TOKEN_NE,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_EOS,
    TOKEN_THIS,
    TOKEN_FOR_MAX,
    TOKEN_FOR_STEP,
    TOKEN_FOR_THIS,
    TOKEN_CONST,
};

typedef union
{
    Number r;
    TString *ts;
} SemInfo;

typedef struct Token
{
    int token;
    SemInfo seminfo;
} Token;

typedef struct LexObject
{
    int current;
    int deepth;
    Token t;
    Token lookahead;
    struct ElsFuncObj *fs;
    struct els_VmObj *L;
    struct vm_iobuff *z;
    int linenumber;
    int lastline;
    TString *source;
} LexObject;

void els_lexer_init(els_VmObj *);
void els_lexer_setinput(els_VmObj *L, LexObject *LS, vm_iobuff *z, TString *source);
int els_lexer_lex(LexObject *LS, SemInfo *seminfo);
void els_lexer_checklimit(LexObject *ls, int val, int limit, const char *msg);
void els_lexer_token2str(int token, char *s);
void els_compiler_error(LexObject *ls, const char *buff);

#endif
