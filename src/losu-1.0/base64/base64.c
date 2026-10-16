#include "base64.h"
#include "base.h"

#include <stdint.h>

/*
    base64维护须知：
        底层函数原型如下

        uint8_t *Base64Encode(uint8_t *content, int length);
        uint8_t *Base64Encode(uint8_t *content, int length);

        1. 这两个函数的content参数都是要编码或解码的文本，为避免编译器警告，可以强制转换为(void)型指针来传递const char*类型的指针
        length参数则是content的字节数，注意是字节数！
        2. 两个函数的返回值均使用stdlib中的malloc来动态分配内存，一定要记得free！！！！！！
*/

int ELSAPI_base64_capi_b64encode(els_VmObj* vm)
{
    const char *content;
    if (arg_gettype(vm, 1)==ELS_API_TYPE_STRING || arg_gettype(vm, 1)==ELS_API_TYPE_NUMBER)
    {
        content = arg_getstr(vm, 1);
        void *result = Base64Encode((void*)content, sizeof(char)*strlen(content));
        arg_returnstr(vm, result);
        free(result);
    }
    else
        arg_returnnull(vm);
    return 1;
}

int ELSAPI_base64_capi_b64decode(els_VmObj* vm)
{
    const char *content;
    if (arg_gettype(vm, 1)==ELS_API_TYPE_STRING || arg_gettype(vm, 1)==ELS_API_TYPE_NUMBER)
    {
        content = arg_getstr(vm, 1);
        void *result = Base64Decode((void*)content, sizeof(char)*strlen(content));
        arg_returnstr(vm, result);
        free(result);
    }
    else
        arg_returnnull(vm);
    return 1;
}

int ELSAPI_base64_capi_b64encodep(els_VmObj* vm) // capi_b64encodep(p:ptr, len:int)
{
    void *ptr;
    uint64_t len;
    if (arg_gettype(vm, 1)==ELS_API_TYPE_PTR || arg_gettype(vm, 1)==ELS_API_TYPE_NUMBER)
    {
        ptr = arg_getptr(vm, 1);
        len = arg_getnum(vm, 2);
        void *result = Base64Encode(ptr, len);
        arg_returnstr(vm, result);
        free(result);
    }
    else
        arg_returnnull(vm);
    return 1;
}
