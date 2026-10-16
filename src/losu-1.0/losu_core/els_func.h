#ifndef __els_func_h__
#define __els_func_h__

#include "els_object.h"


els_func_code *els_ScriptFunc_newcodeir (els_VmObj *);
void els_ScriptFunc_end (els_VmObj *, els_func_code *, int );
ElsCfunc *els_ScriptFunc_new_csfunciotn (els_VmObj *, int );
void els_ScriptFunc_freecodeir (els_VmObj *, els_func_code *);
void els_ScriptFunc_free_csfunciotn (els_VmObj *, ElsCfunc *);

const char *els_ScriptFunc_getlocalname (const els_func_code *, int , int );


#endif