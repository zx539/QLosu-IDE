#include "hash.h"
#include "MD5.h"
#include "SHA256.h"
#include "SHA384.h"
#include "SHA512.h"

int ELSAPI_hash_md5(els_VmObj* vm)
{
    /*
        md5算法接口

        # 参数
            1.content    要摘要的字符串
        
        # 返回值
            1.result     对应的MD5值
    */
    MD5_t task;
    task.content = (void*)arg_getstr(vm, 1);
    MD5(&task);
    arg_returnstr(vm, task.result);
    return 1;
}

int ELSAPI_hash_sha256(els_VmObj* vm)
{
    /*
        sha256算法接口

        # 参数
            1.content    要摘要的字符串
        
        # 返回值
            1.result     对应的sha256值
    */
    SHA256_t task;
    task.content = (void*)arg_getstr(vm, 1);
    SHA256(&task);
    arg_returnstr(vm, (void*)task.result);
    return 1;
}

int ELSAPI_hash_sha384(els_VmObj* vm)
{
    /*
        sha384算法接口

        # 参数
            1.content    要摘要的字符串
        
        # 返回值
            1.result     对应的sha384值
    */
    SHA384_t task;
    task.content = (void*)arg_getstr(vm, 1);
    SHA384(&task);
    arg_returnstr(vm, (void*)task.result);
    return 1;
}

int ELSAPI_hash_sha512(els_VmObj* vm)
{
    SHA512_t task;
    task.content = (void*)arg_getstr(vm, 1);
    SHA512(&task);
    arg_returnstr(vm, (void*)task.result);
    return 1;
}