# bridge模块文档

```python
bridge.c = {
    #从洛书数据类型转换到C数据类型
    int = bridge_c_cint32,
    int8 = bridge_c_cint8,
    int16 = bridge_c_cint16,
    int32 = bridge_c_cint32,
    int64 = bridge_c_cint64,
    char = bridge_c_cint8,
    short = bridge_c_cint16,
    float32 = bridge_c_cfloat32,
    float64 = bridge_c_cfloat64,
    float = bridge_c_cfloat32,
    double = bridge_c_cfloat64,
    str = bridge_c_cstr,
    ptr = bridge_c_cptr,

    #接口函数
    iif = bridge_c_iif, #int返回值、int参数的函数(int int function)
}
```


例程(Windows):

```python
"""
    使用printf进行格式化输出
"""

import "bridge"
import "sharedlib"

var lib = sharedlib.load("C:\\Windows\\System32\\msvcrt.dll") #加载动态库
var function = sharedlib.sys(lib, "printf") #获取printf函数

#将洛书类型转换为C类型
var pattern = bridge.c.str("str:\%s, number:\%d\n")
var s = bridge.c.str("Hello losu!")
var n = bridge.c.int(1024)

#函数调用
bridge.c.iif(function, pattern, s, n)


```

最后的输出

```shell
str:Hello losu!, number:1024
```