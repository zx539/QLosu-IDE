# while 循环

while 循环是洛书语言中用于重复执行代码块的控制结构。它会在指定条件为真时持续执行循环体，直到条件变为假为止。

## 基本语法

洛书的 while 循环语法如下：

```losu
while (条件表达式) {
    # 循环体代码
}
```

> [!IMPORTANT]
> while 循环会先检查条件，如果条件为假，循环体一次都不会执行。

## 基本示例

```losu
var print import print

print("=== 基本while循环 ===")

var count = 1
while (count <= 5) {
    print("计数:" .. count->&String)
    count = count + 1
}

print("循环结束，最终count:" .. count->&String)
```

## 常见用法

### 累加计算

```losu
var print import print

print("=== 计算1到10的和 ===")

var sum = 0
var i = 1

while (i <= 10) {
    sum = sum + i
    i = i + 1
}

print("1到10的和为:" .. sum->&String)
```

### 条件查找

```losu
var print import print

var numbers = [2, 4, 7, 8, 12, 15]
var target = 8
var index = 0
var found = false

print("=== 查找数字" .. target->&String .. "===")

while (index < 6 and not found) {
    if (numbers[index] == target) {
        found = true
        print("找到了！位置:" .. index->&String)
    } else {
        print("检查位置" .. index->&String .. ":" .. numbers[index]->&String)
        index = index + 1
    }
}

if (not found) {
    print("未找到目标数字")
}
```

## 循环控制

### break 语句

```losu
var print import print

print("=== 使用break退出循环 ===")

var count = 1
while (true) {  // 无限循环
    print("当前计数:" .. count->&String)

    if (count >= 3) {
        print("达到目标，退出循环")
        break
    }

    count = count + 1
}

print("循环结束")
```

### continue 语句

```losu
var print import print

print("=== 使用continue跳过偶数 ===")

var i = 0
while (i < 10) {
    i = i + 1

    if (i % 2 == 0) {
        continue  // 跳过偶数
    }

    print("奇数:" .. i->&String)
}
```

## 嵌套循环

```losu
var print import print

print("=== 嵌套while循环 ===")

var i = 1
while (i <= 3) {
    print("外层循环:" .. i->&String)

    var j = 1
    while (j <= 2) {
        print("  内层循环:" .. j->&String)
        j = j + 1
    }

    i = i + 1
}
```

## 练习

### 练习 1：计算阶乘

使用 `while` 循环计算一个数的阶乘。

<details>
<summary>查看答案</summary>

```losu
var print import print

var n = 5
var factorial = 1
var i = 1

print("计算" .. n->&String .. "的阶乘")

while (i <= n) {
    factorial = factorial * i
    print(i->&String .. "! =" .. factorial->&String)
    i = i + 1
}

print("最终结果:" .. n->&String .. "! =" .. factorial->&String)
```

</details>

### 练习 2：猜数字游戏

实现一个简单的猜数字游戏逻辑。

<details>
<summary>查看答案</summary>

```losu
var print import print

var secretNumber = 7
var guesses = [5, 8, 6, 7]  // 模拟用户猜测
var guessIndex = 0
var found = false

print("=== 猜数字游戏 ===")
print("我想了一个1-10之间的数字")

while (guessIndex < 4 and not found) {
    var guess = guesses[guessIndex]
    print("你猜:" .. guess->&String)

    if (guess == secretNumber) {
        found = true
        print("恭喜！猜对了！")
    } else if (guess < secretNumber) {
        print("太小了")
    } else {
        print("太大了")
    }

    guessIndex = guessIndex + 1
}

if (not found) {
    print("游戏结束，答案是:" .. secretNumber->&String)
}
```

</details>

**上一章**：[if 语句 ←](if.md) | **下一章**：[for区间循环 →](for-range.md)