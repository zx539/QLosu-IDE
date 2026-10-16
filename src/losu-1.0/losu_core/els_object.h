#ifndef __els_object_h__
#define __els_object_h__

#include "els.h"

#define ttype(o) ((o)->ttype)
#define nvalue(o) ((o)->value.n)
#define tsvalue(o) ((o)->value.ts)
#define clvalue(o) ((o)->value.cl)
#define hvalue(o) ((o)->value.a)
#define infovalue(o) ((o)->value.i)
#define svalue(o) (tsvalue(o)->str)

#define iscfunction(o) (ttype(o) == ELS_TYPE_FUNCTION && clvalue(o)->isC)
#define ismarked(x) ((x)->mark != (x))

extern const StackObj els_Object_nullobject;

char *els_Object_openspace(els_VmObj* , size_t);
int els_Object_equalObj(const StackObj *, const StackObj *);
int els_Object_str2d(const char *, Number *);
lint32 els_Object_power2(lint32 );

#endif