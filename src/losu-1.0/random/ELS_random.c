#include "ELS_random.h"
#include "crandom.h"

#include <stdlib.h>

int ELSAPI_random_randseed(els_VmObj *vm)
{
    arg_returnnum(vm, randseed());
    return 1;
}

int ELSAPI_random_random(els_VmObj *vm)
{
    arg_returnnum(vm, random());
    return 1;
}

int ELSAPI_random_randint(els_VmObj *vm)
{
    uint64_t start, end;
    start = arg_getnum(vm, 2);
    end = arg_getnum(vm, 3);
    arg_returnnum(vm, randint(start, end));
    return 1;
}

int ELSAPI_random_randfloat(els_VmObj *vm)
{
    double start, end;
    start = arg_getnum(vm, 2);
    end = arg_getnum(vm, 3);
    arg_returnnum(vm, randfloat(start, end));
    return 1;
}

int ELSAPI_random_normal(els_VmObj *vm)
{
    double nu, sigma;
    nu = arg_getnum(vm, 2);
    sigma = arg_getnum(vm, 3);
    arg_returnnum(vm, normal(nu, sigma));
    return 1;
}

int ELSAPI_random_srand(els_VmObj *vm)
{
    uint64_t seed = arg_getnum(vm, 2);
    lc_srand(seed);
    return 1;
}

int ELSAPI_random_randstr(els_VmObj *vm)
{
    size_t len = arg_getnum(vm, 2);
    char *result = malloc(len + 1);
    randstr(result, len);
    result[len + 1] = '\0';
    arg_returnstr(vm, result);
    free(result);
    return 1;
}

int ELSAPI_random_randbytes(els_VmObj *vm)
{
    size_t len = arg_getnum(vm, 2);
    uint8_t *result = malloc(len + 1);
    randbytes(result, len);
    result[len + 1] = '\0';
    arg_returnstr(vm, (void*)result);
    free(result);
    return 1;
}

int ELSAPI_random_percent(els_VmObj *vm)
{
    double rate = arg_getnum(vm, 2);
    if (percent(rate))
        arg_returnnum(vm, 1);
    else
        arg_returnnull(vm);
    return 1;
}

int ELSAPI_random_random_init(els_VmObj *vm)
{
    LosuObj lib = obj_newunit(vm);

    obj_setunit(vm, lib, obj_newstr(vm, "randseed"), obj_newfunction(vm, ELSAPI_random_randseed));
    obj_setunit(vm, lib, obj_newstr(vm, "random"), obj_newfunction(vm, ELSAPI_random_random));
    obj_setunit(vm, lib, obj_newstr(vm, "randint"), obj_newfunction(vm, ELSAPI_random_randint));
    obj_setunit(vm, lib, obj_newstr(vm, "randfloat"), obj_newfunction(vm, ELSAPI_random_randfloat));
    obj_setunit(vm, lib, obj_newstr(vm, "normal"), obj_newfunction(vm, ELSAPI_random_normal));
    obj_setunit(vm, lib, obj_newstr(vm, "srand"), obj_newfunction(vm, ELSAPI_random_srand));
    obj_setunit(vm, lib, obj_newstr(vm, "randstr"), obj_newfunction(vm, ELSAPI_random_randstr));
    obj_setunit(vm, lib, obj_newstr(vm, "randbytes"), obj_newfunction(vm, ELSAPI_random_randbytes));
    obj_setunit(vm, lib, obj_newstr(vm, "percent"), obj_newfunction(vm, ELSAPI_random_percent));

    vm_setval(vm, "random", lib);
    return 0;
}
