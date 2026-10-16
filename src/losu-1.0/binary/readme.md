#!/bin/owls.wiki.losu.tech
# wiki for binary

这是一个二进制数据储存及操作模块

使用示例
```python
import("binary")

var barray = binary.create(24) #创建一个24字节大小的储存区
barray.set(0) #将储存区数据全部设置为0
barray.insert(0, 1) #将储存区的0索引处设置为1
barray.insert(1, 78) #将储存区的1索引处设置为78
barray.print()

"""上面的输出
0x1  0x4E 0x0  0x0  0x0  0x0  0x0  0x0
0x0  0x0  0x0  0x0  0x0  0x0  0x0  0x0
0x0  0x0  0x0  0x0  0x0  0x0  0x0  0x0
"""

barray.free() #储存区要手动回收内存

```

# 函数
## binary.create(size:int): unit
创建一个大小为size字节的二进制储存区，并封装到一个洛书对象中。
该对象有如下成员函数

### this.free()

释放创建的二进制储存区，调用该成员函数后，对象会不可用。
### this.writef(fname:str, n:int)
向名为fname的文件中写入n字节

### this.writem(p:ptr, n:int): int
向p指针写入n字节

### this.tounit(): unit
将储存区中的数据按字节转化为洛书number类型数据并储存到unit中返回
返回的unit从1开始索引
#### 参数
    无
#### 返回值
    1. 转化而成的unit

### this.fromunit(u): unit
接受一个unit，从1处开始索引，直至索引到null。将索引范围内所有属于number类型的数据储存到储存区中。
对储存区中的数据每8字节转化一个number型数据

将会覆盖储存区原有的数据
#### 参数
    1. u:uint 要转化的unit
#### 返回值
    1. 对象本身

### this.print(): null
将储存区中的数据按字节以16进制大写的格式打印到控制台
#### 参数
    无
#### 返回值
    1. 打印的字节数

### this.printfile(fname): int
将储存区中的数据按字节以16进制大写的格式写入文件
#### 参数      
    1. fname:str 要写入的文件的名称
#### 返回值
    1. 写入的字节数

### this.prints(): str
将储存区中的数据按字节以16进制大写的格式转化成字符串并返回
#### 参数
    无
#### 返回值
    1. 转化后的字符串

### this.scans(s:str): unit
接受一个字符串，从里面扫描所有16进制数字并储存到储存区中。数字之间以空白字符分隔。

#### 参数
    1. s:str 要被扫描的字符串

#### 返回值
    调用函数的对象本身
#### 例子
```python
import('binary')

var hexstr = "0xE4 0xBD 0xA0 0xE5 0xA5 0xBD 0xEF 0xBC 0x8C 0xE6 0xB4 0x9B 0xE4 0xB9 0xA6 0xEF 0xBC 0x81"

var array = binary.create(1)
array.scans(hexstr)

array.print()

"""上面的输出
0xE4 0xBD 0xA0 0xE5 0xA5 0xBD 0xEF 0xBC
0x8C 0xE6 0xB4 0x9B 0xE4 0xB9 0xA6 0xEF
0xBC 0x81
"""

print(array.tostr())

"""上面的输出
你好，洛书！
"""

```

### this.set(n:int): int
将储存区内的所有数据设置为n
#### 参数
    1. n:int 要设置的数据
#### 返回值
    1. 实际设置的数据

### this.get(index:int, size:int): int
从储存区的指定位置读取size个字节
1<=size<=8

#### 参数
    1. i:int 索引
    2. size:int 要读取的字节数
        
#### 返回值
    1. 读取的字节值
    2. 当索引越界时，返回null

### this.insert(index:int, d:int, size:int): int
向储存区的指定位置插入size个字节
会替换掉原本的字节
1<=size<=8

#### 参数
    1. i:int 索引
    2. d:要插入的数据
    3. size:int 要读取的字节数
        
#### 返回值
    1. 实际插入的数据

### this.insertp(index:int, p:ptr, size:int): int
读取p指针的size个字节，插入储存区的index处
会替换掉原本的字节
size>=1

#### 参数
    1. i:int 索引
    2. p:要插入的指针
    3. size:int 要读取的字节数
        
#### 返回值
    1. 被读取的指针

### this.cat(b:unit): unit
在该储存区的末尾附加上b中储存区的所有数据

#### 参数
    1. b 用于附加的对象

#### 返回值
    1. 调用该方法的对象本身

### this.replace(b:unit, start:int, len:int): unit
将b中的数据从start处开始赋盖a中的数据，会覆盖len个字节
覆盖到a的结尾处后会停止覆盖
    
#### 参数
    1. b:ptr 用于覆盖的储存区指针
    2. start 覆盖的起始位置
    3. len 覆盖的字节数
        
#### 返回值
    1. 调用函数的对象本身

### this.writem(p:ptr, len:int): int
将储存区的数据以指定字节数写入内存中

#### 参数
    1. m:ptr 要写入的内存的指针
    2. len:int 要写入的字节数
        
#### 返回值
    1. 写入的实际字节数
    2. 当len<0时,返回null

### this.readm(p:ptr, len:int): int
从内存中读取指定字节的数据

#### 参数
    1. m:ptr 要读取的内存的指针
    2. len:int 要读取的字节数
        
#### 返回值
    1. 调用函数的对象本身
    2. 当len的值<=0、m为0或NULL时,返回null

### this.writef(fname:str, len:int): int
将二进制储存区的数据写入指定的文件中

#### 参数
    1. fname:str 要写入的文件的名称
    2. len:int   要写入的字节数，缺省时为储存的数据的大小

#### 返回值
    1. 写入的实际字节数

### this.readf(fname:str, len): unit
从文件中读取指定字节数
原本的数据会被释放

#### 参数
    1. fname:str 要读取的文件名
    2. len:int 要读取的字节数

    当len缺省时,会被设置为文件大小
        
#### 返回值
    1. 新的储存区的指针
    2. 当打开文件失败时返回null

### this.length(): int
返回对应储存区的大小

### this.copy()
将储存区复制
不会改动原储存区的数据

#### 参数
    无
        
#### 返回值
    1. 新创建的对象

### this.change(len:int): int
改变储存区的大小
在扩大后，数据会被全部复制
在缩小后，数据会被截断

#### 参数
    1. len:int 指定的大小
        
#### 返回值
    1. 新的储存区指针

### this.slice(start:int, end:int): unit
将储存区切片
start和end处的数据都将被包括

#### 参数
    1. start:int 切片起始处
    2. end:int 切片终止处
        
#### 返回值
    1. 新创建的储存区对象

### this.tostr(): str
将储存区的数据直接以字符串形式返回
会在末尾附加'\0'

#### 参数
    无
        
#### 返回值
    1. 转化后的字符串

### this.tonum(): unit
将储存区数据转化为洛书number类型，结果储存在unit中，从1开始索引

#### 参数
    无

#### 返回值
    1. 返回储存结果的unit

### this.toptr(): unit
将储存区数据转化为洛书pointer类型，结果储存在unit中，从1开始索引

#### 参数
    无

#### 返回值
    1. 返回储存结果的unit

### this.fromstr(s:unit): unit
将字符串直接转化为储存区数据
会去除末尾附加的'\0'

#### 参数
    1. s:str 被转化的字符串
        
#### 返回值
    1. 新的储存区指针

### this.fromnum(u:unit): unit
接受一个unit，从1处开始索引，直至索引到null。将索引范围内所有属于number类型的数据储存到储存区中。

对储存区中的数据每8字节转化一个number型数据

将会覆盖储存区原有的数据

#### 参数
    1. u:unit 要转化的unit
        
#### 返回值
    1. 新的储存区指针

## binary.hexval(s:str): number
将一个16进制的字符串转化为number类型

例
```python
import('binary')

var n = binary.valhex("0xFF") #输出为255
```