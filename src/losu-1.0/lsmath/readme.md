#!/bin/owls.wiki.losu.tech
# wiki for lsmath
## 简介
+ lsmath是对洛书stdmath的拓展

## 包含的函数

### PI() :float
+ 中文版本名为“圆周率”
+ 返回π的值
### E() :float
+ 中文版本名为“自然常数”
+ 返回e的值
### GAMMA() :float
+ 中文版本名为“欧拉常数”
+ 返回γ（欧拉常数）的值
### abs( x:number ) :number
+ 中文版本名为“绝对值”
+ 返回x的绝对值。当x大于等于0时，返回其本身，否则返回其相反数。
### mod( a:int, b:int ): int
+ 中文版本名为“取模”
+ 返回a取模b的结果。即a除以b的余数。
### sqrt( num:number ): number
+ 中文版本名为“开平方”
+ 返回num的平方根
### anyroot( num:number, n:int ): number
+ 无中文版本
+ 参数
    1. num:number 被开方数
    2. n:int      要开方的级数
+ 返回值
    1. 返回num的n次方根的一个近似值
    2. 当类型不合法时，返回null
+ 注意
    1. num和n的类型必须为number型
### gcd( a:int, b:int ): int
+ 无中文版本
+ 参数
    1. a:int 一个任意的整数
    2. b:int 一个任意的整数
+ 返回值
    1. 返回a和b的最大公约数
    2. 当类型不合法时返回null
+ 注意
    1. a和b的类型必须为number
    2. a和b会被转化为long long类型，即只取整数部分，抛去小数部分
### acos( x )
+ 中文版本名为“反余弦”
+ 返回以弧度表示的x的反余弦
### asin( x )
+ 中文版本名为“反正弦”
+ 返回以弧度表示的x的反正弦
### atan( x )
+ 中文版本名为“反正切”
+ 返回以弧度表示的x的反正切
### cos( x )
+ 中文版本名为“余弦”
+ 返回x的余弦值
### sin( x )
+ 中文版本名为“正弦”
+ 返回x的正弦值
### tan( x )
+ 中文版本名为“正切”
+ 返回x的正切值
### log( a, N )
+ 中文版本名为“对数”
+ 返回以a为底N的对数
### exp( x )
+ 暂无中文版本
+ 返回e的x次幂的值
### ln( N )
+ 暂无中文版本
+ 返回以e为底，N的对数
### lg( N )
+ 暂无中文版本
+ 返回以10为底，N的对数
### ceil( num )
+ 中文版本名为“向上取整”
+ 对num向上取整
### floor( num )
+ 中文版本名为“向下取整”
+ 对num向下取整
### round( num )
+ 中文版本名为“四舍五入”
+ 将num四舍五入成整数
### cut( num, x )
+ 中文版本名为“保留位数”
+ 截取num到第x位小数
### move_left( num:int, n:int ): int
+ 参数
    1. num:int 被左移数
    2. n:int 要左移的位数
+ 返回值
    1. 左移之后的结果
    2. 如果类型检查不合格则返回null
+ 注意
    1. num和n的类型必须为number型
    2. num和n会被转化为long long型
    3. 请自行防止溢出，本函数不做类似检查
### move_right( num:int, n:int ): int
+ 参数
    1. num:int 被右移数
    2. n:int 要右移的位数
+ 返回值
    1. 右移之后的结果
    2. 如果类型检查不合格则返回null
+ 注意
    1. num和n的类型必须为number型
    2. num和n会被转化为long long型
    3. 请自行防止溢出，本函数不做类似检查
### bitwise_and( a:int, b:int ): int
+ 返回对a进行b的按位与运算结果
### bitwise_or( a:int, b:int ): int
+ 返回对a进行b的按位或运算结果
### bitwise_not( a:int ): int
+ 返回对a按位取反运算结果
### bitwise_xor( a:int, b:int ): int
+ 返回对a进行b的按位异或运算结果
### sum( a:unit ): number
+ 返回a中所有数字的和
### even( a:unit ): number
+ 返回a中所有数字的平均数
### degree( x:number ): number
+ 参数
    1. x:number 弧度制下的数字
+ 返回值
    1. 转化为角度制后的数据
### radian( x:number ): number
+ 参数
    1. x:number 角度制制下的数字
+ 返回值
    1. 转化为弧度制后的数据