#ifndef __els_string_h__
#define __els_string_h__


#include "els_object.h"
#include "els_vmhost.h"
#include<string.h>
#define FIXMARK		2
#define RESERVEDMARK	3
#define sizestring(l)	((long)sizeof(TString) + ((long)(l+1)-TSPACK)*(long)sizeof(char))


void els_string_init (els_VmObj *);
void els_string_resize (els_VmObj *, stringunit *, int );
void els_string_freeall (els_VmObj *L);


TString *els_string_newlstr (els_VmObj *, const char *, size_t );
TString *els_string_newfixed (els_VmObj *, const char *);
TString *els_string_new(els_VmObj *, const char *);


#endif