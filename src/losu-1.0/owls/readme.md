#!/bin/owls.wiki.losu.tech
# owls · wiki服务黑魔法
## OWLS可以做什么？
+ OWLS是一个基于洛书实现的wiki建站服务工具，可以让您利用markdown文档生成精美的web网页
## OWLS有哪些特色？
+ 更轻量、快速的选择
    - 基于高效的 losu script，完全由其标准库书写，仅需数十KB的资源需求
    - 支持多种markdown引擎
    - 通过洛书模块工具一键部署
+ 满足个性化需求
    - 原生html模块支持
    - 支持工程自定义模板
+ 无中间文件
    - 无html输出生成
    - 适合git版本管理机制
    - 保持源码整洁度
## 如何使用？
## 安装与初始化
1. 安装 洛书 v1.6.8+ 版本
2. ```losu -r lpt install owls```


## 编写工程文档
```python
#!/bin/losu
import('owls')

# 创建一个名为 web_losu 的工程，其根目录为 /var/www/losu
web_losu = {
    wwwroot = "/var/www/losu/",
    wiki = {
        # 这里是绑定所有出现在侧边栏的文件
        [1] = "readme.md",
        ["test"] = {    # 这是指 test 目录下的 readm.md 文件，/var/www/losu/test/readme.md
            [1] = "readme.md",
        },
    }
}

# 可以创建多个工程

owls.start(web_losu,argv(1))

```

## 编写主题
+ 以 losu 工程为例，其主题目录为 wwwroot/owls_theme
+ 你可以访问[洛书官网](https://losu.tech)下载主题样例

## 编写文档
+ 您可以遵循markdown格式自由的编写
+ 创建链接，将 工程文档 链接到 /bin/owls_losu，
+ 在文档开头写一行声明```#!/bin/owls_losu```，这将另其可以被渲染

## 配置服务器
+ 将所.md的文件配置为可执行的cgi程序，__注意__ ，不要将md文件所在目录配置为cgi-bin，这样会引起图片等多媒体资源的不可访问
+ 检查，修改文件权限为 555 
+ 启动服务器，访问 xxx.md，观察效果

