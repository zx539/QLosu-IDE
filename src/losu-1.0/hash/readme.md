#!/bin/owls.wiki.losu.tech
# wiki for hash

## 模块简介
这是一个哈希模块，目前实现了md5、sha256、sha384、sha512摘要算法

## 函数详解
### md5( content:string ): string
+ 该函数无中文版本
+ 参数
1. content 要进行md5摘要的内容，因洛书内部统一采用utf-8，所以编码一致
+ 返回值
1. 返回32字符的大写md5值

### sha256( content:string ):string
+ 该函数无中文版本
+ 参数
1. content 要进行sha256摘要的内容
+ 返回值
1. 返回64字符的大写sha256值

### sha384( content:string ):string
+ 该函数无中文版本
+ 参数
1. content 要进行sha256摘要的内容
+ 返回值
1. 返回96字符的大写sha384值

### sha512( content:string ):string
+ 该函数无中文版本
+ 参数
1. content 要进行sha256摘要的内容
+ 返回值
1. 返回128字符的大写sha512值