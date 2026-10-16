#ifndef __els_heap_h__
#define __els_heap_h__

#include "els.h"
#include "els_object.h"

#define incr_top(L) {if (L->top == L->stack_last) els_Heap_checkstack(L, 1); L->top++;}

void els_Heap_init (els_VmObj *, int );
void els_Heap_adjusttop (els_VmObj *, StackObj* , int );
int els_Heap_call (els_VmObj *, StackObj* , int);

void els_Heap_checkstack (els_VmObj *, int );

void els_Heap_breakrun (els_VmObj *, int );




#endif