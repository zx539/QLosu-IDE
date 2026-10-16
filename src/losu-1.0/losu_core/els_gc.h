

#ifndef __els_gc_h__
#define __els_gc_h__


#include "els_object.h"


void els_Gc_collect (els_VmObj *, int );
void els_Gc_checkGC (els_VmObj *);


#endif
