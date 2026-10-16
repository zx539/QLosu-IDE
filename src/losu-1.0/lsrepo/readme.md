#!/bin/owls.wiki.losu.tech
# lsrepo 洛书持续部署 wiki
## 简介
lsrepo (Losu Repo )，洛书持续部署工具，是一款运行在 Linux 平台下的 CI 工具，支持多种 版本管理工具与构建环境，支持导入第三方源
## 可用功能
+ 运行方法
```
losu -r lsrepo [参数列表]
```
```sh
lsrepo add [name]  # 添加 追踪
lsrepo rm  [name]  # 删除 追踪
lsrepo list        # 列出所有追踪信息
lsrepo check [name] # 检测是否发生更改
lsrepo make         # 执行更新

```
## 使用方法
+ [视频教程]()