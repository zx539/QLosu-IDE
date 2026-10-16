
#ifndef els_code_h
#define els_code_h

#include "els_lex.h"
#include "els_object.h"
#include "els_bytecode.h"
#include "els_parser.h"
#include "els_vmcore.h"

#define NO_JUMP (-1)

typedef enum BinOpr {
  OPR_ADD, OPR_SUB, OPR_MULT, OPR_DIV, OPR_POW,
  OPR_CONCAT,
  OPR_NE, OPR_EQ, OPR_LT, OPR_LE, OPR_GT, OPR_GE,
  OPR_AND, OPR_OR,
  OPR_NOBINOPR
} BinOpr;

typedef enum UnOpr { OPR_MINUS, OPR_NOT, OPR_NOUNOPR } UnOpr;


enum Mode {iO, iU, iS, iAB};  

#define VD	100	

extern const struct Bytecodesubframe {
  char mode;
  unsigned char push;
  unsigned char pop;
} els_codegen_bytecodesubframe[];



int els_codegen_code_arg2 (ElsFuncObj *, ByteIR , int , int );
int els_codegen_jump (ElsFuncObj *);
void els_codegen_patchlist (ElsFuncObj *, int , int );
void els_codegen_concat (ElsFuncObj *, int *, int );
void els_codegen_goiftrue (ElsFuncObj *fs, expdesc *, int );
int els_codegen_getlabel (ElsFuncObj *);
void els_codegen_deltastack (ElsFuncObj *, int );
void els_codegen_number (ElsFuncObj *, Number );
void els_codegen_adjuststack (ElsFuncObj *, int );
int els_codegen_lastisopen (ElsFuncObj *);
void els_codegen_setcallreturns (ElsFuncObj *, int );
void els_codegen_tostack (LexObject *, expdesc *, int );
void els_codegen_storevar (LexObject *, const expdesc *);
void els_codegen_prefix (LexObject *, UnOpr , expdesc *);
void els_codegen_infix (LexObject *, BinOpr , expdesc *);
void els_codegen_posfix (LexObject *, BinOpr , expdesc *, expdesc *);

#define els_codegen_code_arg0(fs,o) (els_codegen_code_arg2(fs,o,0,0))
#define els_codegen_code_arg1(fs,o,arg) (els_codegen_code_arg2(fs,o,arg,0))
#define els_codegen_kstr(ls,c) (els_codegen_code_arg1(ls->fs, BYTECODE_PUSHSTRING, c))
#define els_codegen_error(ls,msg) els_compiler_error(ls,msg)
#define next_instruction(fs) ( (fs->pc > fs->lasttarget)?(fs->f->code[fs->pc-1]):((Instruction)(BYTECODE_END)) )
#define els_codegen_kstr(ls,c) (els_codegen_code_arg1(ls->fs, BYTECODE_PUSHSTRING, c))

#endif
