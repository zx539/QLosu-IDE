#include <stdlib.h>
#include <stdio.h>
#include "els.h"


#include "els_heap.h"
#include "els_mem.h"
#include "els_object.h"
#include "els_vmhost.h"


void *els_Mem_growaux(els_VmObj *vm, void *block, size_t issues_num, int inc, size_t size, const char *errormsg, size_t limit)
{
    size_t newn = issues_num + inc;
    if (issues_num >= limit - inc)
        vm_error(vm, errormsg,ELS_ERRORBACK_RUN);
    if ((newn ^ issues_num) <= issues_num || (issues_num > 0 && newn < 4))
        return block;
    else
        return els_Mem_realloc(vm, block, els_Object_power2(newn) * size);
}


// 全局的内存管理函数，所有内存的申请与回收都从这里走
void *els_Mem_realloc(els_VmObj *vm, void *block, lint32 size)
{
    if (size == 0)
    {
        if(block!=NULL)
            ELS_HEAPMEM_FREE(block);    // 回收指针空间
        return NULL;
    }
    
    block = ELS_HEAPMEM_REALLOC(block, size);   //重新分配内存块
    if (block == NULL){
        if (vm)
            vm_error(vm, "内存管理器出错:无法获取足够的内存",ELS_ERRORBACK_MEM);
        else
            return NULL;
    }
    return block;
}