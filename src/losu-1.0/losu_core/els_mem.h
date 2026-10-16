#ifndef __els_mem_h__
#define __els_mem_h__

#include <stddef.h>

#include "els.h"

void *els_Mem_realloc (els_VmObj *, void *, lint32 );
void *els_Mem_growaux (els_VmObj *, void *, size_t ,int , size_t , const char *,size_t );

#define els_Mem_free(L, b)		                    els_Mem_realloc(L, (b), 0)
#define els_Mem_malloc(L, t)	                    els_Mem_realloc(L, NULL, (t))
#define els_Mem_new(L, t)                           ((t *)els_Mem_malloc(L, sizeof(t)))
#define els_Mem_newvector(L, n,t)                   ((t *)els_Mem_malloc(L, (n)*(lint32)sizeof(t)))

#define els_Mem_growvector(L, v,issues_num,inc,t,e,l)   ((v)=(t *)els_Mem_growaux(L, v,issues_num,inc,sizeof(t),e,l))
#define els_Mem_reallocvector(L, v,n,t)             ((v)=(t *)els_Mem_realloc(L, v,(n)*(lint32)sizeof(t)))



#endif