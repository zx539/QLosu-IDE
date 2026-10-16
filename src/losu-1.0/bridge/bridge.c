#include "els.h"
#include "bridge.h"
#include "lsffi.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define unit_arg(x) x+1

static double lsptr_to_c(els_VmObj *vm, LosuObj *p)
{
    double output;
    void *ptr = obj_toptr(vm, p);
    output = *((double*)&ptr);
    return output;
}

// int

int ELSAPI_bridge_bridge_c_cint8(els_VmObj* vm) // bridge_c_cint8()
{
    LosuObj output;
    int type = arg_gettype(vm, unit_arg(1));

    if (type == ELS_API_TYPE_NUMBER)
    {
        int64_t n = (int8_t)arg_getnum(vm, unit_arg(1));
        output = obj_newbyte(vm, (char*)&n, 8);
    }
    else if (type == ELS_API_TYPE_PTR)
    {
        output = obj_newnum(vm, lsptr_to_c(vm, arg_get(vm, unit_arg(1))));
    }
    else
    {
        output = obj_newnull(vm);
    }

    arg_return(vm, output);

    return 1;
}

int ELSAPI_bridge_bridge_c_cint16(els_VmObj* vm) // bridge_c_cint16()
{
    LosuObj output;
    int type = arg_gettype(vm, unit_arg(1));

    if (type == ELS_API_TYPE_NUMBER)
    {
        int64_t n = (int16_t)arg_getnum(vm, unit_arg(1));
        output = obj_newbyte(vm, (char*)&n, 8);
    }
    else if (type == ELS_API_TYPE_PTR)
    {
        output = obj_newnum(vm, lsptr_to_c(vm, arg_get(vm, unit_arg(1))));
    }
    else
    {
        output = obj_newnull(vm);
    }

    arg_return(vm, output);

    return 1;
}

int ELSAPI_bridge_bridge_c_cint32(els_VmObj* vm) // bridge_c_cint32()
{
    LosuObj output;
    int type = arg_gettype(vm, unit_arg(1));

    if (type == ELS_API_TYPE_NUMBER)
    {
        int64_t n = (int32_t)arg_getnum(vm, unit_arg(1));
        output = obj_newbyte(vm, (char*)&n, 8);
    }
    else if (type == ELS_API_TYPE_PTR)
    {
        output = obj_newnum(vm, lsptr_to_c(vm, arg_get(vm, unit_arg(1))));
    }
    else
    {
        output = obj_newnull(vm);
    }

    arg_return(vm, output);

    return 1;
}

int ELSAPI_bridge_bridge_c_cint64(els_VmObj* vm) // bridge_c_cint64()
{
    LosuObj output;
    int type = arg_gettype(vm, unit_arg(1));

    if (type == ELS_API_TYPE_NUMBER)
    {
        int64_t n = (int64_t)arg_getnum(vm, unit_arg(1));
        output = obj_newbyte(vm, (char*)&n, 8);
    }
    else if (type == ELS_API_TYPE_PTR)
    {
        output = obj_newnum(vm, lsptr_to_c(vm, arg_get(vm, unit_arg(1))));
    }
    else
    {
        output = obj_newnull(vm);
    }

    arg_return(vm, output);

    return 1;
}

// float

int ELSAPI_bridge_bridge_c_cfloat32(els_VmObj* vm) // bridge_c_cint8()
{
    LosuObj output;
    int type = arg_gettype(vm, unit_arg(1));

    if (type == ELS_API_TYPE_NUMBER)
    {
        float n = arg_getnum(vm, unit_arg(1));
        uint64_t tmp = *((uint64_t*)&n);
        output = obj_newbyte(vm, (char*)&n, 8);
    }
    else if (type == ELS_API_TYPE_PTR)
    {
        output = obj_newnum(vm, lsptr_to_c(vm, arg_get(vm, unit_arg(1))));
    }
    else
    {
        output = obj_newnull(vm);
    }

    arg_return(vm, output);

    return 1;
}

int ELSAPI_bridge_bridge_c_cfloat64(els_VmObj* vm) // bridge_c_cint8()
{
    LosuObj output;
    int type = arg_gettype(vm, unit_arg(1));

    if (type == ELS_API_TYPE_NUMBER)
    {
        double n = arg_getnum(vm, unit_arg(1));
        output = obj_newbyte(vm, (char*)&n, 8);
    }
    else if (type == ELS_API_TYPE_PTR)
    {
        output = obj_newnum(vm, lsptr_to_c(vm, arg_get(vm, unit_arg(1))));
    }
    else
    {
        output = obj_newnull(vm);
    }

    arg_return(vm, output);

    return 1;
}

// string

int ELSAPI_bridge_bridge_c_cstr(els_VmObj* vm) // bridge_c_cint8()
{
    LosuObj output;
    int type = arg_gettype(vm, unit_arg(1));

    if (type == ELS_API_TYPE_STRING)
    {
        char* p = (char*)arg_getstr(vm, unit_arg(1));
        output = obj_newbyte(vm, (char*)&p, 8);
    }
    else if (type == ELS_API_TYPE_PTR)
    {
        output = obj_newnum(vm, lsptr_to_c(vm, arg_get(vm, unit_arg(1))));
    }
    else
    {
        output = obj_newnull(vm);
    }

    arg_return(vm, output);

    return 1;
}

// ptr
int ELSAPI_bridge_bridge_c_cptr(els_VmObj* vm) // bridge_c_cint8()
{
    LosuObj output;
    int type = arg_gettype(vm, unit_arg(1));

    if (type == ELS_API_TYPE_STRING)
    {
        char* p = (char*)arg_getstr(vm, unit_arg(1));
        output = obj_newbyte(vm, (char*)&p, 8);
    }
    else if (type == ELS_API_TYPE_BYTE)
    {
        char* p = (char*)arg_getbyte(vm, unit_arg(1));
        output = obj_newbyte(vm, (char*)&p, 8);
    }
    else if (type == ELS_API_TYPE_PTR)
    {
        char* p = (char*)arg_getptr(vm, unit_arg(1));
        output = obj_newbyte(vm, (char*)&p, 8);
    }
    else
    {
        output = obj_newnull(vm);
    }

    arg_return(vm, output);

    return 1;
}

int ELSAPI_bridge_bridge_c_iif(els_VmObj* vm) // bridge_c_iaf() #int arg function
{
    /*
        多参函数
        第一个参数恒为ptr类型的c函数指针
        其余参数均为number类型的参数
    */
    uint32_t argn = arg_num(vm) - 1;
    char __tmp[8];
    LosuObj loutput = obj_newbyte(vm, (char*)__tmp, 8);
    uint64_t *output = (uint64_t*)obj_tobyte(vm, &loutput);
    arg_return(vm, loutput);

    switch (argn)
    {
        case 0:
        {
            arg_returnnull(vm);
            return 1;
        }
        case 1:
        {
            uint64_t args[6] = {0};
            void *fp = arg_getptr(vm, unit_arg(1));
            *output = lsffi_cdel_intargs6(args, fp);
            
            arg_return(vm, loutput);
            return 1;
        }
        default:
        {
            void *fp = arg_getptr(vm, unit_arg(1));

            argn--;
            
            //函数调用
            if (argn <= 6)
            {
                uint64_t args[6] = {0};

                //参数传递
                for (int i = 0; i < argn; i ++)
                {
                    if (arg_gettype(vm, i + 3) != ELS_API_TYPE_BYTE)
                    {
                        return 0;
                    }

                    void *p = (void*)arg_getbyte(vm, i + 3);
                    args[i] = *((uint64_t*)p);
                }

                *output = lsffi_cdel_intargs6(args, fp);
            }
            else
            {
                uint64_t *args = malloc(sizeof(uint64_t) * argn);
                //参数传递

                for (int i = 0; i < argn; i ++)
                {
                    if (arg_gettype(vm, i + 3) != ELS_API_TYPE_BYTE)
                    {
                        return 0;
                    }

                    void *p = (void*)arg_getbyte(vm, i + 3);
                    args[i] = *((uint64_t*)p);
                }

                *output = lsffi_cdel_intargsx(args, args + 6, fp, argn - 6);
                free(args);
            }

        }
    }

    return 1;
}
