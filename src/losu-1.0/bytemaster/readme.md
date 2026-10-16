# bytemaster模块文档

bytemaster是一个byte操作库，用于在脚本层面对byte进行类似c的操作。

## 函数列表

### bytemaster.alloc

bytemaster.alloc(length:int)

分配一块长为length的内存，以byte形式返回。

### bytemaster.write

bytemaster.write(m:byte, index:int, data:[int/byte/str])

对byte类型数据m，从第index号偏移起，写入data。

当data类型为int时，被转换为char。当data类型为byte时，当前版本不做处理。当data类型为str时，将字符串整个写入。

### bytemaster.read

bytemaster.read(m:byte, index:int, size:int)

对byte类型数据m，从第index号偏移起，读取size个字节，以byte形式返回。

## 贡献者名单

2024-4-4 成天宇 创始模块
2024-4-4 张耀源 优化代码逻辑