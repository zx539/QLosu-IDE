# for 区间循环

for 区间循环是洛书语言中用于在指定数值范围内进行迭代的循环语句。它使用简洁的语法来遍历连续的整数序列，是处理计数循环的主要方式。

## 基本语法

洛书的 for 区间循环语法如下：

```losu
for (var 变量名1, 变量名2, 变量名3 = 起始值, 结束值, 步长) {
    # 循环体代码
}
```

> [!NOTE]
> 当循环条件不可能满足时（例如 for ( var i,m,s = 1, 10, -1 ) ）编译器会自动跳过该循环

其中，变量名1、变量名2、变量名3 以及 起始值、结束值、步长 可以任意省略，等号两边按顺序对应赋值，当变量名全部省略时 `var 变量名1, 变量名2, 变量名3 =` 部分可以全部省略：

```losu
var print import print;

// 起始值、结束值，步长默认为 1
for(1, 10) {
    print("循环中")
}

// 起始值、结束值、步长
for(1, 10, 2) {
    print("步长为 2")
}

// 使用变量
for(var i = 1, 10) {
    print(i)
}

// 三个变量分别对应迭代值、最大值、步长
for(var i, max, step = 1, 10, 2) {
    print("当前：" .. i->&String .. " 最大：" .. max->&String .. " 步长：" .. step->&String)
}

// 省略部分变量
for(var i, max = 1, 10, 2) {
    print(i)
}
```

> [!IMPORTANT]
> 区间循环是**包含结束值**的，即如果写 `for (var i = 1, 5)`，会执行 i = 1, 2, 3, 4, 5 共5次循环。

## 基本示例

```losu
var print import print

print("=== 基本区间循环 ===")

// 从1到5的循环
for (var i = 1, 5) {
    print("当前数字:" .. i->&String)
}

// 从6到10的循环
var i=6
for(6,10) {
    print("当前数字:" .. i->&String)
    i = i + 1
}

print("循环结束")
```

## 常见用法

### 数组索引遍历

```losu
var print import print;

var fruits = ["苹果", "香蕉", "橙子", "葡萄"]

print("=== 数组索引遍历 ===")
print("水果列表:")

// 注意：数组索引从0开始，所以是 0 到 sizeof(fruits)-1
for (var i = 0,3) {
    print("索引" .. i->&String .. ":" .. fruits[i]->&String)
}
```

### 数学计算

```losu
var print import print

print("=== 计算1到10的和 ===")

var sum = 0
for (var i = 1, 10) {
    sum = sum + i
}

print("1到10的和为:" .. sum->&String)
```

### 嵌套循环

```losu
var print import print

print("=== 乘法表 ===")

for (var i = 1, 3) {
    for (var j = 1, 3) {
        var result = i * j
        print(i->&String .. "×" .. j->&String .. "=" .. result->&String)
    }
    print("---")
}
```

## 循环控制

### break 语句

```losu
var print import print

print("=== 查找第一个大于15的平方数 ===")

for (var i = 1, 10) {
    var square = i * i
    print(i->&String .. "的平方是" .. square->&String)

    if (square > 15) {
        print("找到了！")
        break
    }
}
```

### continue 语句

```losu
var print import print

print("=== 打印1到10中的奇数 ===")

for (var i = 1, 10) {
    if (i % 2 == 0) {
        continue  // 跳过偶数
    }
    print("奇数:" .. i->&String)
}
```

## 练习

### 练习 1：计算阶乘

编写程序计算5的阶乘（5! = 5 × 4 × 3 × 2 × 1）。

<details>
<summary>查看答案</summary>

```losu
var print import print

var n = 5
var factorial = 1

print("计算" .. n->&String .. "的阶乘")

for (var i = 1, n) {
    factorial = factorial * i
    print("当前结果:" .. factorial->&String)
}

print(n->&String .. "! =" .. factorial->&String)
```

</details>

### 练习 2：找出1到20中的质数

编写程序找出1到20之间的所有质数。

<details>
<summary>查看答案</summary>

```losu
var print import print

print("1到20中的质数:")

for (var num = 2, 20) {
    var isPrime = true

    // 检查是否为质数
    for (var i = 2, num - 1) {
        if (num % i == 0) {
            isPrime = false
            break
        }
    }

    if (isPrime) {
        print(num->&String .. "是质数")
    }
}
```

</details>

**上一章**：[← while循环](while.md) | **下一章**：[for-unit循环 →](for-unit.md)