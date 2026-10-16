#ifndef ELS_RANOM_H_
#define ELS_RANOM_H_

#include "els.h"
#include "crandom.h"
#include "lc.h"

#include <stdio.h>

int ELSAPI_random_randseed(els_VmObj *vm);
int ELSAPI_random_random(els_VmObj *vm);
int ELSAPI_random_randint(els_VmObj *vm);
int ELSAPI_random_randfloat(els_VmObj *vm);
int ELSAPI_random_normal(els_VmObj *vm);
int ELSAPI_random_srand(els_VmObj *vm);
int ELSAPI_random_randstr(els_VmObj *vm);
int ELSAPI_random_randbytes(els_VmObj *vm);
int ELSAPI_random_percent(els_VmObj *vm);

#endif