#!/bin/owls.wiki.losu.tech
# wiki for socket

# 简介
socket是一个跨平台的网络编程模块，用于向洛书提供套接字服务。

# 常量
## socket.AF_INET
AF_INET指的是IPv4地址族

## socket.SOCK_STREAM
SOCK_STREAM指的是流式套接字

# 函数
## socket.gethostbyname(name:str): str
### 参数
1. name 要解析的主机名
### 返回值
1. 一个单元，结构如下
```python
{
    name:str,        #查询的主机的正式名称
    aliases:unit,    #查询的主机的别称，是一个都是字符串的单元，索引从0开始
    addrtype:int,    #对应ip的地址族
    length:int,      #对应ip的长度
    addr_list:unit   #对应的ip，是一个都是字符串的单元，索引从0开始
}
```
### 注意事项
1. 该函数在错误输入除字符串外的一切数据时不会出错，但在输入某些错误字符串时会造成未知原因的程序无响应，请确保输入的字符串大致符合主机名的格式

## socket.class(family, type, protocol)
### 参数
1. family指要创建的套接字的地址族，目前可选的有socket.AF_INET
2. type指要创建的套接字的类型，目前可选的有socket.SOCK_STREAM
3. protocol指要创建的套接字的协议，目前只能留空
### 返回值
返回一个套接字类

# 类
## socket
### 创建方式
+ 使用new(socket.Socket, ...)创建
### 成员变量
#### sockfd
+ 保存一个指向socket_t的指针
+ 此变量不应被非开发者访问

### 成员函数

#### sendtimeout(sec:int)
设置发送的超时时间

#### recvtimeout(sec:int)
设置接收的超时时间

#### bind(ip:string, port:int): [int/null]
将套接字绑定到指定ip和端口
##### 参数
1. ip 要绑定的ip地址，默认为本机地址
2. port要绑定的端口，默认为80

___

#### listen(amount:int): [int/null]
开始监听
##### 参数
1. amount 设置最大连接数

___

#### accept(): socket
接受客户端套接字
##### 返回值
1. 返回连接到的客户端套接字的socket类

___

#### connect(ip:string, port:int): [int/null]
连接到指定ip和端口
##### 参数
1. ip 要连接的服务端的ip
2. port 要连接的服务端的端口
##### 返回值
连接成功时返回1，失败时返回null

___

#### send(data:string): [int/null]
向套接字另一端发送数据
##### 参数
1. data 要发送的数据

___

#### recv(amount:int): string
从套接字另一端接受数据
##### 参数
1. amount 预计接受的字符数，应大于0而小于1024

___

#### close(): null
关闭套接字