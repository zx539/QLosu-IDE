#!/bin/owls.wiki.losu.tech
# wiki for lssys

## 简介
lssys是洛书stdsys的扩展，加强了其功能。

## 函数
### sys.getos(): str / 系统.操作系统(): 字符串
+ 参数
    无
+ 返回值
    1. 返回当前操作系统版本，值为"Windows Branch"或"Linux Branch"

### sys.getenv(name:str): str  / 系统.环境变量(名称:字符串): 字符串
+ 参数
    1. name 要获取的环境变量的名称

+ 返回值
    1. 对应环境变量的值的字符串

### sys.popen(cmd:str, mod:str): unit / 系统.新进程(命令:字符串, 模式:字符串): 单元
+ 参数
    1. cmd 要执行的命令
    2. mod 执行的模式

+ 返回值
    1. 返回创建的管道文件的类，详情见下文

### sys.system(cmd:str): int / 系统.调用(命令:字符串): 数字
+ 参数
    1. cmd 要执行的命令

+ 返回值
    1. 命令执行后的状态码

### sys.getcwd(): str / 系统.工作目录(): 字符串
+ 参数
    无

+ 返回值
    1. 当前工作目录的路径的字符串

### sys.exsists(path:str): [int/null] / 系统.存在(路径:字符串): [数字/空]
+ 参数
    1. path 要检查的文件路径

+ 返回值
    1. 路径存在时返回1
    2. 路径不存在时返回null

### sys.mkdir(path:str): [int/null] / 系统.创建目录(路径:字符串): [数字/空]
+ 参数
    1. path 要创建的目录路径

+ 返回值
    1. 创建成功时返回1
    2. 创建失败时返回null

### sys.rename(oldname:str, newname:str): [int/null] / 系统.重命名(旧名:字符串, 新名:字符串): [数字/空]
+ 参数
    1. oldname 要重命名的文件名
    2. newname 新文件名

+ 返回值
    1. 重命名成功时返回1
    2. 重命名失败时返回null

### sys.remove(name:str): [int/null] / 系统.删除(路径:字符串): [数字/空]
+ 参数
    1. name 要删除的文件路径

+ 返回值
    1. 删除成功时返回1
    2. 删除失败时返回null

### sys.getpid(): int / 系统.获取进程id(): 整数
+ 参数
    1. 无

+ 返回值
    1. 返回当前进程的id

### sys.lpcount(): int / 系统.逻辑处理器数(): 整数
+ 参数
    无
+ 返回值
    1. 当前处理器的逻辑处理器个数

### sys.architecture(): str / 系统.处理器架构()
+ 参数
    无
+ 返回值
    1. 当前处理器的架构。会是“X86”、“ARM”的其中一个
    2. 当架构未知时，返回null

### sys.memtotal(): int / 系统.总内存(): 整数
+ 参数
    无
+ 返回值
    1. 当前操作系统的总物理内存

### sys.memfreep(): int / 系统.线程可用内存(): 整数
+ 参数
    无
+ 返回值
    1. 当前线程可用的最大内存