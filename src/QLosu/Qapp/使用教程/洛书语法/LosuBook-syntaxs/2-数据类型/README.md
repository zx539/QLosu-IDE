# 数据类型

洛书语言拥有丰富的数据类型系统，支持动态类型。本章将详细介绍洛书的所有数据类型及其使用方法。

## 本章内容

### [逻辑类型](逻辑类型.md)

true 和 false 类型，包括：

- 逻辑值的独特类型设计
- 逻辑运算符（and、or、not）
- 短路求值
- 真值判断规则

### [数值类型](数值类型.md)

统一的 Float 类型，包括：

- 整数和浮点数字面量
- 算术运算和比较
- 数值转换和特殊值

### [字符串类型](字符串类型.md)

string 类型和文本处理，包括：

- 字符串字面量
- 转义字符
- 字符串操作

### [函数类型](函数类型.md)

func 类型和函数编程，包括：

- 函数定义和调用
- 高阶函数和闭包
- 递归函数

### [特殊类型](特殊类型.md)

- 暂缺（Resource 类型）

### [容器类型](容器类型.md)

Unit 类型基础，包括：

- Unit 类型概述
- 成员访问和修改
- 嵌套结构
- 实际应用场景

### [元类型](元类型.md)

Meta 类型基础，包括：

- 元类型基本定义
- 设置变量的元类型
- 使用元类型进行运算符重载
- 数据绑定元

## 洛书类型系统概览

洛书是一门动态类型语言，变量的类型在运行时确定。

### 基本类型

洛书的基本数据类型包括：

- **真类型**：`true`（非`nil`的所有变量）
- **假类型**：`false`（`nil`）
- **数值类型**：`Float`（统一的数值类型）
- **字符串类型**：`String`（UTF-8 文本）
- **函数类型**：`Func`（一等公民）
- **容器类型**: `Unit` （支持数组格式）
- **资源类型**：`Resource` （可自动回收的指针，不直接暴露给用户）
- **元类型**：`Meta` （用于描述数据信息的特殊字符串类型）
- **线程类型**：`Thread` （使用 `async` 创建）

### 类型转换

使用 `->&String` 或 `->&Float` 将数据转换为字符串或浮点数

```losu
var print import print

// 转换为数字
var NumFromString = "3.14"->&Float
var number = NumFromString + 1.86   // Float 类型的加法运算

// 转换为字符串
print(number->&String)
```

### 类型断言

使用 `->` 或 `->*` 可以断言变量的类型（原始类型或元类型），如果类型不满足会报错，满足则没有影响：

```losu
var print import print
var async import async

var flag1 = true
var flag2 = false
var num = 42
var text = "Hello"
var func = {=> "hello, losu"}
var unit = {x: 10, y: 20, 'type': 'point'}
var meta = 'This is a meta'
var thread = async { print("This is a async thread") }


// 类型满足没有影响
print("真类型:" .. (flag1->Float)->&String)      // true 可以是非 nil 的所有数据，默认为 Float 类型的 1
print("假类型:" .. (flag2->`nil`)->&String)      // false 为 nil
print("数字类型:" .. (num->Float)->&String)      // Float
print("字符串类型:" .. (text->String)->&String)    // String
print("函数类型:" .. (func->Func)->&String)   // Function
print("容器类型:" .. (unit->Unit)->&String)       // Unit
print("点类型（元类型）:" .. (unit->*point)->&String)       // Unit 类型的元类型断言
print("元类型:" .. (meta->Meta)->&String)    // Meta
print("线程类型" .. (thread->Thread)->&String)

// 类型不满足报错
//print("不满足:" .. (flag1->String)->&String)
```

### 类型检查

使用 `->?` 或 `->?*` 可以检查变量的类型（原始类型或元类型），如果不满足会返回 `nil` ，满足则没有影响：

```losu
var print import print
var async import async

var flag1 = true
var flag2 = false
var num = 42
var text = "Hello"
var func = {=> "hello, losu"}
var unit = {x: 10, y: 20, 'type': 'point'}
var meta = 'This is a meta'
var thread = async { print("This is a async thread") }

// 类型满足没有影响
print("真类型:" .. (flag1->?Float)->&String)      // true 可以是非 nil 的所有数据，默认为 Float 类型的 1
print("假类型:" .. (flag2->?`nil`)->&String)      // false 为 nil
print("数字类型:" .. (num->?Float)->&String)      // Float
print("字符串类型:" .. (text->?String)->&String)    // String
print("函数类型:" .. (func->?Func)->&String)   // Function
print("容器类型:" .. (unit->?Unit)->&String)       // Unit
print("点类型（元类型）:" .. (unit->?*point)->&String)       // Unit 类型的元类型断言
print("元类型:" .. (meta->?Meta)->&String)    // Meta
print("线程类型" .. (thread->?Thread)->&String)

// 类型不满足返回 nil
print("不满足:" .. (flag1->?String)->&String)
```

## 运算符与类型

不同类型支持不同的运算符：

| 运算符 | 数值类型 | 字符串类型 | 逻辑类型 | 容器类型 | 函数类型 | 资源类型 | 元类型 | 线程类型 |
| :--------: | :----------: | :------------: | :----------: | :----------: | :----------: | :----------: | :----------: | :----------: |
| `+` `-` `*` `/` `%` `/%` `**` | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ |
| `&` `\|` `^` `~` `<<` `>>` `>>>` | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ |
| `and` `or` `not` | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| `==` `!=` | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| `<` `<=` `>` `>=` | ✅ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ |
| `..` | ❌ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ |
| `~>` `:>` | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |

## 学习建议

建议按以下顺序学习本章内容：

1. **类型注释** - 理解洛书的类型系统基础
2. **基本类型** - 掌握逻辑、数值、字符串、函数类型
3. **容器类型** - 学习复杂数据结构的使用
4. **实际应用** - 通过练习巩固理解

每个类型都有其特定的用法和最佳实践，通过实际编程练习可以更好地掌握这些概念。

## 关键字表

洛书语言中与数据类型相关的关键字：

| 关键字 | 用途 | 示例 |
| :--------: | :------: | :------: |
| `var` | 声明变量 | `var count = 0` |
| `$` | 数组结构赋值 | `$[a,b,c] = [1,2,3]` |
| `return` | 函数返回值 | `return result` |
| `yield` | 让步 | `yield` |
| `_` | 通配符/自身引用 | `_(a,b)` |

## 运算符表

### 类型相关运算符

| 运算符 | 用途 | 示例 |
| :--------: | :------: | :------: |
| `.` | 成员访问（自动传self） | `obj.method()` |
| `[]` | 索引访问 | `arr[0]`, `obj['key']` |
| `()` | 函数调用 | `func(args)` |

---

**上一章**：[← 基本概念](../1-基本概念/README.md) | **下一章**：[逻辑类型 →](逻辑类型.md)