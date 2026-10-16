#ifndef __els_vm_core_h__
#define __els_vm_core_h__


#include"els.h"
#include<stdio.h>
#include "els_heap.h"
#include "els_object.h"

#define EOZ	(-1)	
#define els_vmio_getc(z)	(((z)->n--)>0 ? ((int)*((z)->p++)): (z)->filbuf(z))
#define els_vmio_ugetc(z)	(++(z)->n,--(z)->p)
#define els_vmio_name(z)	((z)->name)
#define tonumber(o)   ((ttype(o) != ELS_TYPE_NUMBER) && (els_Vmcore_tonumber(o) != 0))
#define tostring(L,o) ((ttype(o) != ELS_TYPE_STRING) && (els_Vmcore_tostring(L, o) != 0))



typedef struct vm_iobuff vm_iobuff;
typedef struct vm_iobuff {
  size_t n;				
  const unsigned char* p;		
  int (*filbuf)(vm_iobuff* z);
  FILE* u;				
  const char *name;
  unsigned char buffer[ELS_VMIO_SIZE];		
}vm_iobuff  ;
vm_iobuff* els_vmio_Fopen (vm_iobuff* z, FILE* f, const char *name);	
vm_iobuff* els_vmio_sopen (vm_iobuff* z, const char* s, const char *name);	
vm_iobuff* els_vmio_mopen (vm_iobuff* z, const char* b, size_t size, const char *name); 

int els_Vmcore_tonumber (StackObj *obj);
int els_Vmcore_tostring (els_VmObj *L, StackObj *obj);
const StackObj *els_Vmcore_getunit (els_VmObj *L, StackObj* t);
void els_Vmcore_setunit (els_VmObj *L, StackObj* t, StackObj* key);
const StackObj *els_Vmcore_getglobal (els_VmObj *L, TString *s);
void els_Vmcore_setglobal (els_VmObj *L, TString *s);
StackObj* els_Vmcore_execute (els_VmObj *L, const ElsCfunc *cl, StackObj* base);
void els_Vmcore_C_csfunciotn (els_VmObj *L, els_C_API_function c, int issues_num);
void els_Vmcore_L_csfunciotn (els_VmObj *L, els_func_code *l, int issues_num);
int els_Vmcore_lessthan (els_VmObj *L, const StackObj *l, const StackObj *r, StackObj* top);
void els_Vmcore_strconc (els_VmObj *L,StackObj* top);


#endif