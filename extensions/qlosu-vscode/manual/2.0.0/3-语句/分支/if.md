# if 分支语句

if 分支语句是洛书语言中用于条件判断和分支执行的控制结构。它允许程序根据不同的条件执行不同的代码块。

## 基本语法

洛书的 if 语句支持以下几种形式：

```losu
// 基本if语句
if (条件) {
    // 代码块
}

// if-else语句
if (条件) {
    // 条件为真时执行
} else {
    // 条件为假时执行
}

// if-else if-else语句
if (条件1) {
    // 条件1为真时执行
} else if (条件2) {
    // 条件2为真时执行
} else {
    // 所有条件都为假时执行
}
```

## 基本示例

```losu
var print import print

var score = 85

print("学生分数:" .. score->&String)

if (score >= 90) {
    print("等级: 优秀")
} else if (score >= 80) {
    print("等级: 良好")
} else if (score >= 60) {
    print("等级: 及格")
} else {
    print("等级: 不及格")
}
```

## 条件表达式

### 比较运算符

```losu
var print import print

var a = 10
var b = 20

print("a =" .. a->&String .. ", b =" .. b->&String)

if (a == b) {
    print("a 等于 b")
}

if (a != b) {
    print("a 不等于 b")
}

if (a < b) {
    print("a 小于 b")
}

if (a <= b) {
    print("a 小于等于 b")
}
```

### 逻辑运算符

```losu
var print import print

var age = 25
var hasLicense = true
var hasExperience = false

print("年龄:" .. age->&String)
print("有驾照:" .. hasLicense->&String)
print("有经验:" .. hasExperience->&String)

// and 运算符
if (age >= 18 and hasLicense) {
    print("可以开车")
}

// or 运算符
if (hasLicense or hasExperience) {
    print("具备驾驶条件之一")
}

// not 运算符
if (not hasExperience) {
    print("需要培训")
}
```

## 嵌套if语句

```losu
var print import print

var weather = "sunny"
var temperature = 25

print("天气:" .. weather)
print("温度:" .. temperature->&String)

if (weather == "sunny") {
    print("天气晴朗")

    if (temperature > 30) {
        print("建议: 注意防晒")
    } else if (temperature > 20) {
        print("建议: 适合外出")
    } else {
        print("建议: 添加衣物")
    }
} else {
    print("天气不佳")

    if (weather == "rainy") {
        print("建议: 带伞出行")
    }
}
```

## 条件表达式

在洛书中，if语句也可以通过表达式替换：

```losu
var print import print

var score = 92

// 使用条件表达式
var grade = score >= 90 and "优秀" or score >= 80 and "良好" or "一般"

print("分数:" .. score->&String)
print("等级:" .. grade->&String)

var status = score >= 60 and "及格" or "不及格"
print("状态:" .. status)
```

## 练习

### 练习 1：数字分类

编写程序判断一个数字是正数、负数还是零。

<details>
<summary>查看答案</summary>

```losu
var print import print

var numbers = [5, -3, 0, 12, -8]

print("=== 数字分类 ===")

for (var num in numbers) {
    if (num > 0) {
        print(num, "是正数")
    } else if (num < 0) {
        print(num, "是负数")
    } else {
        print(num, "是零")
    }
}
```

</details>

### 练习 2：用户权限检查

根据用户角色和状态判断访问权限。

<details>
<summary>查看答案</summary>

```losu
var print import print

var user = {
    name: "张三",
    role: "admin",
    active: true,
    loginCount: 5
}

print("用户:" .. user.name)
print("角色:" .. user.role)
print("状态:" .. user.active->&String)

if (not user.active) {
    print("访问被拒绝: 账户已禁用")
} else if (user.role == "admin") {
    print("访问权限: 管理员权限")
} else if (user.role == "user" and user.loginCount > 0) {
    print("访问权限: 普通用户权限")
} else {
    print("访问被拒绝: 权限不足")
}
```

</details>

**上一章**：[← 语句概述](../README.md) | **下一章**：[循环语句 →](../循环/while.md)