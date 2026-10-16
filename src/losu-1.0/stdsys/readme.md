#!/bin/owls.wiki.losu.tech
# stdlib (stdsys)

```python
#!declare
sys_getos(): str '从内核中获取所在操作系统信息，一般为 Windwos/Linux'
sys_getenv(n: str) '获取名称为n的环境变量'
sys_system(cmd: str): int '调用一个命令'
sys_popen(cmd: str, mod:str): ptr '创建一个管道，mod可取为 r,w'
sys_pwrite(p: ptr,s: str): int '向指定管道 p 写入 s'
sys_pread(p: ptr, n: int): int '从指定管道 p 读取 n 个字符'
sys_pclose(p: ptr): int '关闭一个管道'
#!end
#!script
sys={}
sys.getos  = sys_getos
def sys.getenv(name):
    return sys_getenv(name)
;
def sys.popen(cmd,mod):
    var this = {}

    this.P = sys_popen(cmd,mod)

    if !this.P : return null ;

    def this.write(s):
        return sys_pwrite(this.P,s)
    ;  
    def this.read(i):
        return sys_pread(this.P,i)
    ;
    def this.close():
        return sys_pclose(this.P)
    ;

    return this
; 

def sys.system(cmd):
    return sys_system(cmd)
;
#!end

#!script-cn
系统={}
系统.操作系统  = sys_getos
def 系统.环境变量(name):
    return sys_getenv(name)
;
def 系统.新进程(cmd,mod):
    var this = {}

    this.P = sys_popen(cmd,mod)

    if !this.P : return null ;

    def this.写(s):
        return sys_pwrite(this.P,s)
    ;  
    def this.读(i):
        return sys_pread(this.P,i)
    ;
    def this.关闭():
        return sys_pclose(this.P)
    ;

    return this
; 

def 系统.调用(cmd):
    return sys_system(cmd)
;


#!end

```