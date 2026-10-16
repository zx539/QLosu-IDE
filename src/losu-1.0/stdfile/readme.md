#!/bin/owls.wiki.losu.tech
# stdlib (stdfile)

```python
#!declare
fopen(fname: str,mod: str): ptr     '以指定模式打开一个文件,mod对应C语言,失败返回null'
fclose(fp: ptr): int                '关闭一个文件,成功返回0,失败返回null'
fputc(fp: ptr,c: [int]): int        'c 的字符值写入到 fp 所指向的输出流中。如果写入成功，它会返回写入的字符，如果发生错误，则会返回 null'
fputs(fp: ptr,s: str): int          's 写入到 fp 所指向的输出流中。如果写入成功，它会返回一个非负值，如果发生错误，则会返回 null'
fgetc(fp: ptr): int                 '函数从 fp 所指向的输入文件中读取一个字符。返回值是读取的字符，如果发生错误则返回 null' 
finput(fp:ptr): str                 '从 fp 所指向文件中读取一个字段，遇到空格等结束'
fgets(fp: ptr, int n): str          '从 fp 所指向的输入流中读取 n-1 个字符"如果n为null,则读取为内核最大字符缓冲区"如果这个函数在读取最后一个字符之前就遇到一个换行符或文件的末尾 null,则只会返回读取到的字符，包括换行符'
feof(fp: ptr): int                  'fp 是否到达结尾，否返回 null'
fseek(fp: ptr,i: int,sp: int):int   '设置流 fp 的文件位置为给定的偏移 i,参数i 意味着从给定的 sp 位置查找的字节数'
ftell(fp: ptr): int                 '返回文件当前位置，以文件头为相对位置' 
rewind(fp: prt): null               '返回文件头'                               
#!end
#!script
file = {}
def file.open(fn,mod):
    '''
        fn是文件名,mod 是打开模式
        mod 是与C函数fopen的第二个参数相同的模式参数,具体如下
        模式     描述
        "r"     打开一个用于读取的文件。该文件必须存在。
        "w"	    创建一个用于写入的空文件。如果文件名称与已存在的文件相同，则会删除已有文件的内容，文件被视为一个新的空文件。
        "a"	    追加到一个文件。写操作向文件末尾追加数据。如果文件不存在，则创建文件。
        "r+"	打开一个用于更新的文件，可读取也可写入。该文件必须存在。
        "w+"	创建一个用于读写的空文件。
        "a+"	打开一个用于读取和追加的文件。

    '''

    var this = {}
    
    this.FILE = fopen(fn,mod)
    if this.FILE == null : return null ;
    
    this.filename = fn
    this.mod = mod
    
    def this.close():
        return fclose(this.FILE)
    ;
    def this.putc(i):
        return fputc(this.FILE,i)
    ;
    def this.print(s):
        return fputs(this.FILE,s)
    ;

    def this.gets(i): # 读取i个字符，与fgets不一样
        return fgets(this.FILE,i)
    ;
    def this.getc():
        return fgetc(this.FILE)    
    ;
    def this.input():
        return finput(this.FILE)
    ;
    def this.lineinput():
        return fgets(this.FILE)
    ;
    def this.eof():
        return feof(this.FILE)
    ;
    def this.seek(i,sp):
        return fseek(this.FILE,i,sp)
    ;
    def this.tell():
        return ftell(this.FILE)    
    ;

    return this
;
#!end
#!script-cn
文件 = {}
def 文件.打开(fn,mod):
    
    var this = {}
    
    this.FILE = fopen(fn,mod)
    if this.FILE == null : return null ;
    
    this.文件名 = fn
    this.模式 = mod
    
    def this.关闭():
        return fclose(this.FILE)
    ;
    def this.写字节(i):
        return fputc(this.FILE,i)
    ;
    def this.打印(s):
        return fputs(this.FILE,s)
    ;

    def this.读取(i):
        return fgets(this.FILE,i)
    ;
    def this.读字节():
        return fgetc(this.FILE)    
    ;
    def this.输入():
        return finput(this.FILE)
    ;
    def this.输入一行():
        return fgets(this.FILE)
    ;
    def this.文件尾():
        return feof(this.FILE)
    ;
    def this.定位(i,sp):
        return fseek(this.FILE,i,sp)
    ;
    def this.当前位置():
        return ftell(this.FILE)    
    ;

    return this    
;
#!end

```