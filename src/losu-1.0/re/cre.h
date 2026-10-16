#ifndef __XKIT_RE_H__
#define __XKIT_RE_H__

#include "cxkit.h"

XKIT_API int re_match(const char *targert, const char *p);
XKIT_API LsUnit re_search(const char *targert, const char *p, els_VmObj *vm);
XKIT_API LsString re_replace(const char *targert, const char *p, const char *replace_string, els_VmObj *vm);

#endif