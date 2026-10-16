#!/bin/owls.wiki.losu.tech
# lpkg 模块开发者工具 Wiki
## 简介
Losu Package，是一款运行在 Linux 环境下的，适合开发者的模块管理工具，支持提交、检出、校验与自动化构建等多种功能。
## 可用功能
+ 运行方法
```
losu -r lpkg [参数列表]
```
```sh
lpkg show               # 显示模块的详细信息

lpkg checkout [分支]    # 检出指定分支的源代码到工作区
lpkg commit   [分支]    # 提交工作区代码到指定分支
lpkg clean              # 清理工作区

lpkg md5sum             # 计算 md5 文件
lpkg md5check           # 校验 md5 ，判断文件是否被更改

lpkg make               # 构建模块二进制
```
## 使用方法
+ [视频教程]()
