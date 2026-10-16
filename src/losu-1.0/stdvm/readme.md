#!/bin/owls.wiki.losu.tech
# stdlib (stdvm)
```python
'''
V 1.7.0 + 版本开始支持的模块，主要用以获取与设定虚拟机的状态
'''
#!declare
stdvm_get_version(): str    '获取版本信息'
stdvm_get_build():   str    '获取构建信息'
stdvm_get_copy():    str    '获取版权信息'
stdvm_get_branch():  str    '获取分支信息'
stdvm_set_gc(o:object,size: int,p:str): null   '设置GC上限,p可以为 B,K(默认),M'
stdvm_get_mem(o: obj,p:str): int        '获取内存占用大小,单位 B(默认),K,M'
stdvm_get_gc(o: obj,p:str): int         '获取GC上限，单位 B(默认),K,M' 
stdvm_gc()                  '立即进行GC'
#!end
#!script
vm = {
    version = stdvm_get_version,
    build   = stdvm_get_build,
    copy    = stdvm_get_copy,
    branch  = stdvm_get_branch,
    setgc   = stdvm_set_gc,
    getmem  = stdvm_get_mem,
    getgc   = stdvm_get_gc,
    gc      = stdvm_gc,
}
#!end
#!script-cn
解释器 = {
    版本 = stdvm_get_version,
    构建   = stdvm_get_build,
    版权    = stdvm_get_copy,
    分支  = stdvm_get_branch,
    设定GC   = stdvm_set_gc,
    获取内存  = stdvm_get_mem,
    获取GC   = stdvm_get_gc,
    回收垃圾 = stdvm_gc,
}
#!end
```