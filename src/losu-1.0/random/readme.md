#!/bin/owls.wiki.losu.tech
# random脚本层维基

random是一个高性能的随机数模块。整个模块分为脚本层、C层和文档三部分。每个部分都是相对独立的整体。

若要移植C层到其他模块，请参阅C.md。如果需要获取历史更改信息，请参阅WIKI目录下的“历史”目录。

下面介绍脚本层函数。

## random.randseed()
返回一个随机种子
+ 中文名:随机.获取随机种子
+ 返回值:C层uint64_t类型的一个随机种子

## random.srand(seed)
将修改储存在动态库中的种子
+ 中文名：随机.改种子
+ 参数 seed:要使用的随机种子

## random.random()
+ 中文名：随机.随机数
+ 返回值：返回介于[0, 1)的实数

## random.randint(start, end)
+ 中文名:随机.随机整数
+ 参数 start: 随机区间的开始
+ 参数 end: 随机区间的结束
+ 返回值：返回位于[start, end)的随机整数

## random.randfloat(start, end)
+ 中文名:随机.随机实数
+ 参数 start: 随机区间的开始
+ 参数 end: 随机区间的结束
+ 返回值：返回位于[start, end)的随机实数

## random.randstr(len)
生成指定长度的英文字符串
+ 中文名:随机.随机字符串
+ 参数 len:要生成的字符串的长度
+ 返回值:生成的字符串

## random.randbytes(len)
生成指定长度的随机字节
注意,目前以字符串形式返回
+ 中文名：随机.随机字节
+ 参数 len:要生成的字节数
+ 返回值:生成的字节

## random.percent(rate)
以指定概率返回真值
+ 中文名:随机.百分之
+ 参数 rate:指定的概率
+ 返回值:当为真时,返回1,否则返回null