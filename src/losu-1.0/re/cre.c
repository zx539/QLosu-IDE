#include "cre.h"
#include "re.h"

int ELSAPI_re_re_match(els_VmObj *vm)
{
    const char *target = arg_getstr(vm, unit_arg(1));
    const char *p = arg_getstr(vm, unit_arg(2));
    int output = re_match(target, p);
    if (output == 1)
        arg_returntrue(vm);
    else
        arg_returnfalse(vm);
    return 1;
}

int ELSAPI_re_re_search(els_VmObj *vm)
{
    const char *target = arg_getstr(vm, unit_arg(1));
    const char *p = arg_getstr(vm, unit_arg(2));
    arg_return(vm, re_search(target, p, vm));
    return 1;
}

int ELSAPI_re_re_replace(els_VmObj *vm)
{
    const char *target = arg_getstr(vm, unit_arg(1));
    const char *p = arg_getstr(vm, unit_arg(2));
    const char *r = arg_getstr(vm, unit_arg(3));
    LsString output = re_replace(target, p, r, vm);
    arg_return(vm, output);
    return 1;
}
