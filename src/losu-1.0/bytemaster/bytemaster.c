#include "bytemaster.h"

#include <stdlib.h>
#include <stdio.h>

#define unit_arg(x) x+1
#define unit_atype(vm, x) arg_gettype(vm, x+1)

int ELSAPI_bytemaster_bytemaster_alloc(els_VmObj* vm) // mem_alloc(length:int)
{
    int type = arg_gettype(vm, unit_arg(1));

    if (type == ELS_API_TYPE_NUMBER)
    {
        size_t l = arg_getnum(vm, unit_arg(1));
        void *m = calloc(l, sizeof(char));
        arg_returnbyte(vm, m, l);
        free(m);
    }
    else
        arg_returnnull(vm);
    return 1;
}

int ELSAPI_bytemaster_bytemaster_write(els_VmObj* vm) // mem_write(m:byte, index:int, data:[int/byte/str])
{
    if (unit_atype(vm, 1) == ELS_API_TYPE_BYTE && unit_atype(vm, 2) == ELS_API_TYPE_NUMBER && unit_atype(vm, 3) != ELS_API_TYPE_NULL)
    {
        char *m = (char*)arg_getbyte(vm, unit_arg(1));
        size_t index = arg_getnum(vm, unit_arg(2));

        if (unit_atype(vm, 3) == ELS_API_TYPE_NUMBER)
        {
            char ch = arg_getnum(vm, unit_arg(3));
            m[index] = ch;
        }
        else if (unit_atype(vm, 3) == ELS_API_TYPE_STRING)
        {
            const char *s = arg_getstr(vm, unit_arg(3));
            sprintf(m + index, "%s", s);
        }

        arg_return(vm, *arg_get(vm, unit_arg(1)));
    }
    else
        arg_returnnull(vm);
    return 1;
}

int ELSAPI_bytemaster_bytemaster_read(els_VmObj* vm) // bytemaster_read(m:byte, index:int, size:int)
{
    if (unit_atype(vm, 1) == ELS_API_TYPE_BYTE && unit_atype(vm, 2) == ELS_API_TYPE_NUMBER && unit_atype(vm, 3) == ELS_API_TYPE_NUMBER)
    {
        char *m = (char*)arg_getbyte(vm, unit_arg(1));
        size_t index = arg_getnum(vm, unit_arg(2));
        size_t msize = arg_getnum(vm, unit_arg(3));

        arg_returnbyte(vm, m + index, msize);
    }
    else
        arg_returnnull(vm);
    return 1;
}
