#ifndef __els_unit_h__
#define __els_unit_h__

#include "els_object.h"


// 节点，下标，值
#define node(t,i)	(&(t)->node[i])
#define key(n)		(&(n)->key)
#define val(n)		(&(n)->val)


Hash *els_Unit_new (els_VmObj *, int );
void els_Unit_free (els_VmObj *, Hash *);
void els_Unit_remove (Hash *, StackObj *);

const StackObj *els_Unit_get (els_VmObj *, const Hash *, const StackObj *);
const StackObj *els_Unit_getnum (const Hash *, Number );
const StackObj *els_Unit_getstr (const Hash *, TString *);
StackObj *els_Unit_set (els_VmObj *, Hash *, const StackObj *);
StackObj *els_Unit_setint (els_VmObj *, Hash *, int );
void els_Unit_setstrnum (els_VmObj *, Hash *, TString *, Number );

Node * els_Unit_next (els_VmObj *, const Hash *, const StackObj *);

unsigned long els_Unit_hash (els_VmObj *, const StackObj *);
const StackObj *els_Unit_getglobal (els_VmObj *, const char *);

#endif