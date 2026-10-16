# sharedlib文档

## 概述

sharedlib是一个动态库操作模块，提供了动态库的加载、卸载和指针地址获取功能。

## 函数列表

### sharedlib.load

sharedlib.load(...:str) -> ptr / unit

接受str类型的参数作为动态库路径。当参数个数为1时，返回加载的动态库的指针。当参数个数大于1时，将指针装在unit中返回。

单个参数，当加载失败时，返回null。多个参数，当加载失败时，对应的索引，其值为null。

### sharedlib.unload

sharedlib.unload(...:ptr)

卸载对应的指针的动态库。该函数始终返回null。

### sharedlib.sys

sharedlib.sys(libp:ptr, sys:str)

在libp中查找sys标准，当查找到时，返回对应的指针，否则返回null。
