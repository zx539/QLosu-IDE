#ifndef __els_bytecode_h__
#define __els_bytecode_h__

#include "els.h"

#define SIZE_INSTRUCTION 32

#define SIZE_B 9
#define SIZE_OP 6
#define SIZE_A (SIZE_INSTRUCTION - (SIZE_OP + SIZE_B))
#define SIZE_U (SIZE_INSTRUCTION - SIZE_OP)

#define POS_U SIZE_OP
#define POS_B SIZE_OP
#define POS_A (SIZE_OP + SIZE_B)





#if SIZE_U < BITS_SIZE - 1
    #define MAX_ARG_U ((1 << SIZE_U) - 1)
    #define MAX_ARG_S (MAX_ARG_U >> 1) 
#else
    #define MAX_ARG_U MAX_INT
    #define MAX_ARG_S MAX_INT
#endif


#if SIZE_A < BITS_SIZE - 1
    #define MAX_ARG_A ((1 << SIZE_A) - 1)
#else
    #define MAX_ARG_A MAX_INT
#endif


#if SIZE_B < BITS_SIZE - 1
    #define MAX_ARG_B ((1 << SIZE_B) - 1)
#else
    #define MAX_ARG_B MAX_INT
#endif




#define MAXSTACK 256  //表达式栈上限
#if MAXSTACK > MAX_ARG_B
    #undef MAXSTACK
    #define MAXSTACK MAX_ARG_B
#endif


#define LFIELDS_PER_FLUSH 64
#if LFIELDS_PER_FLUSH > (MAXSTACK / 4)
    #undef LFIELDS_PER_FLUSH
    #define LFIELDS_PER_FLUSH (MAXSTACK / 4)
#endif

#define RFIELDS_PER_FLUSH (LFIELDS_PER_FLUSH / 2)
#define LOOKBACKNUMS 32


#define MASK1(n,p)	((~((~(Instruction)0)<<n))<<p)
#define MASK0(n,p)	(~MASK1(n,p))



#define CREATE_0(o)	 ((Instruction)(o))
#define GET_OPCODE(i)	((ByteIR)((i)&MASK1(SIZE_OP,0)))
#define SET_OPCODE(i,o)	((i) = (((i)&MASK0(SIZE_OP,0)) | (Instruction)(o)))

#define CREATE_U(o,u)	 ((Instruction)(o) | ((Instruction)(u)<<POS_U))
#define GETARG_U(i)	((int)((i)>>POS_U))
#define SETARG_U(i,u)	((i) = (((i)&MASK0(SIZE_U,POS_U)) | ((Instruction)(u)<<POS_U)))

#define CREATE_S(o,s)	CREATE_U((o),(s)+MAX_ARG_S)
#define GETARG_S(i)	(GETARG_U(i)-MAX_ARG_S)
#define SETARG_S(i,s)	SETARG_U((i),(s)+MAX_ARG_S)


#define CREATE_AB(o,a,b) ((Instruction)(o) | ((Instruction)(a)<<POS_A) |  ((Instruction)(b)<<POS_B))
#define GETARG_A(i)	((int)((i)>>POS_A))
#define SETARG_A(i,a)	((i) = (((i)&MASK0(SIZE_A,POS_A)) | ((Instruction)(a)<<POS_A)))
#define GETARG_B(i)	((int)(((i)>>POS_B) & MASK1(SIZE_B,0)))
#define SETARG_B(i,b)	((i) = (((i)&MASK0(SIZE_B,POS_B)) | ((Instruction)(b)<<POS_B)))

typedef enum {
BYTECODE_END,
BYTECODE_RETURN,

BYTECODE_CALL,

BYTECODE_PUSHNULL,
BYTECODE_POP,

BYTECODE_PUSHSTRING,
BYTECODE_PUSHNUM,

BYTECODE_PUSHUPVALUE,

BYTECODE_GETLOCAL,
BYTECODE_GETGLOBAL,

BYTECODE_GETUNIT,
BYTECODE_PUSHSELF,

BYTECODE_CREATEUNIT,

BYTECODE_SETLOCAL,
BYTECODE_SETGLOBAL,
BYTECODE_SETUNIT,

BYTECODE_SETLIST,
BYTECODE_SETMAP,

BYTECODE_ADD,
BYTECODE_SUB,
BYTECODE_MULT,
BYTECODE_DIV,
BYTECODE_POW,
BYTECODE_CONCAT,
BYTECODE_MINUS,
BYTECODE_NOT,

BYTECODE_JMPNE,
BYTECODE_JMPEQ,
BYTECODE_JMPLT,
BYTECODE_JMPLE,
BYTECODE_JMPGT,
BYTECODE_JMPGE,

BYTECODE_JMPT,
BYTECODE_JMPF,
BYTECODE_JMPONT,
BYTECODE_JMPONF,
BYTECODE_JMP,

BYTECODE_PUSHNULLJMP,

BYTECODE_FORPREP,
BYTECODE_FORLOOP,

BYTECODE_LFORPREP,
BYTECODE_LFORLOOP,

BYTECODE_PUSHFUNCTION

} ByteIR;
#define NUM_BYTECODES	((int)BYTECODE_PUSHFUNCTION+1)
#define ISJUMP(o)	(BYTECODE_JMPNE <= (o) && (o) <= BYTECODE_JMP)

#define MULT_RET        255	
#if MULT_RET>MAX_ARG_B
#undef MULT_RET
#define MULT_RET	MAX_ARG_B
#endif

#endif
