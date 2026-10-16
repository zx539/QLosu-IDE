# re中文文档

re是一个正则表达式模块。该模块基于C++的regex模块。

# 例程

```lua
var s = "aabbccaa"
var res = re.replace(s, "aa", "yyy")
print(res)
```

# 函数

## re.match(s:str, pattern:str)
检查字符串s是否符合pattern指定的模式。是则返回1,否则返回null。

## re.search(s:str, pattern:str)
搜索字符串s中所有符合pattern的子字符串,并储存在一个unit中返回。

返回值的索引从1开始,如果没有找到任何子字符串,则返回空unit。

## re.repalce(a:str, pattern:str, b:str)
将字符串a中所有符合pattern的子字符串都替换为b。返回替换后的字符串。
