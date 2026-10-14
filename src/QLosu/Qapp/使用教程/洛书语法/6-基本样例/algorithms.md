# 算法示例

本章提供了一系列使用洛书语言实现的经典算法示例，展示了洛书语言在算法实现方面的表达能力和简洁性。

## 排序算法

### 冒泡排序

```losu
import std

main {
    let numbers = [64, 34, 25, 12, 22, 11, 90]

    println('=== 冒泡排序 ===')
    println('原数组:', numbers)

    let n = sizeof(numbers)

    # 冒泡排序实现
    for (i in 0, n - 2) {
        for (j in 0, n - 2 - i) {
            if (numbers[j] > numbers[j + 1]) {
                # 交换元素
                let temp = numbers[j]
                numbers[j] = numbers[j + 1]
                numbers[j + 1] = temp
            }
        }
    }

    println('排序后:', numbers)
}
```

### 选择排序

```losu
import std

main {
    let numbers = [29, 10, 14, 37, 13]

    println('=== 选择排序 ===')
    println('原数组:', numbers)

    let n = sizeof(numbers)

    # 选择排序实现
    for (i in 0, n - 2) {
        let minIndex = i

        # 找到最小元素的索引
        for (j in i + 1, n - 1) {
            if (numbers[j] < numbers[minIndex]) {
                minIndex = j
            }
        }

        # 交换元素
        if (minIndex != i) {
            let temp = numbers[i]
            numbers[i] = numbers[minIndex]
            numbers[minIndex] = temp
        }
    }

    println('排序后:', numbers)
}
```

## 搜索算法

### 线性搜索

```losu
import std

main {
    let numbers = [2, 4, 6, 8, 10, 12, 14]
    let target = 8

    println('=== 线性搜索 ===')
    println('数组:', numbers)
    println('目标:', target)

    let found = false
    let position = -1

    for (i in 0, sizeof(numbers) - 1) {
        if (numbers[i] == target) {
            found = true
            position = i
            break
        }
    }

    if (found) {
        println('找到了！位置:', position)
    } else {
        println('未找到目标元素')
    }
}
```

### 二分搜索

```losu
import std

main {
    let numbers = [1, 3, 5, 7, 9, 11, 13, 15]
    let target = 7

    println('=== 二分搜索 ===')
    println('数组:', numbers)
    println('目标:', target)

    let left = 0
    let right = sizeof(numbers) - 1
    let found = false
    let position = -1

    while (left <= right and not found) {
        let mid = (left + right) / 2

        if (numbers[mid] == target) {
            found = true
            position = mid
        } elif (numbers[mid] < target) {
            left = mid + 1
        } else {
            right = mid - 1
        }
    }

    if (found) {
        println('找到了！位置:', position)
    } else {
        println('未找到目标元素')
    }
}
```

## 数学算法

### 斐波那契数列

```losu
import std

main {
    let n = 10

    println('=== 斐波那契数列 ===')
    println('计算前', n, '项')

    if (n <= 0) {
        println('请输入正整数')
    } elif (n == 1) {
        println('F(1) = 0')
    } elif (n == 2) {
        println('F(1) = 0')
        println('F(2) = 1')
    } else {
        let a = 0
        let b = 1

        println('F(1) =', a)
        println('F(2) =', b)

        for (i in 3, n) {
            let c = a + b
            println('F(' & str(i) & ') =', c)
            a = b
            b = c
        }
    }
}
```

### 最大公约数（欧几里得算法）

```losu
import std

main {
    let a = 48
    let b = 18

    println('=== 最大公约数 ===')
    println('数字1:', a)
    println('数字2:', b)

    # 保存原始值用于输出
    let originalA = a
    let originalB = b

    # 欧几里得算法
    while (b != 0) {
        let temp = b
        b = a % b
        a = temp
    }

    println('最大公约数:', a)
    println('验证:', originalA, '÷', a, '=', originalA / a)
    println('验证:', originalB, '÷', a, '=', originalB / a)
}
```

## 字符串算法

### 字符串反转

```losu
import std

main {
    let text = 'Hello World'

    println('=== 字符串反转 ===')
    println('原字符串:', text)

    # 将字符串转换为字符数组（简化处理）
    let chars = []
    for (i in 0, sizeof(text) - 1) {
        chars = chars + [text[i]]
    }

    println('字符数组:', chars)

    # 反转数组
    let n = sizeof(chars)
    for (i in 0, n / 2 - 1) {
        let temp = chars[i]
        chars[i] = chars[n - 1 - i]
        chars[n - 1 - i] = temp
    }

    println('反转后数组:', chars)
}
```

### 回文检查

```losu
import std

main {
    let words = ['level', 'hello', 'radar', 'world', 'madam']

    println('=== 回文检查 ===')

    for (word in words) {
        let isPalindrome = true
        let len = sizeof(word)

        # 检查是否为回文
        for (i in 0, len / 2 - 1) {
            if (word[i] != word[len - 1 - i]) {
                isPalindrome = false
                break
            }
        }

        if (isPalindrome) {
            println(word, '是回文')
        } else {
            println(word, '不是回文')
        }
    }
}
```

## 练习

### 练习 1：插入排序

实现插入排序算法。

<details>
<summary>查看答案</summary>

```losu
import std

main {
    let numbers = [5, 2, 4, 6, 1, 3]

    println('=== 插入排序 ===')
    println('原数组:', numbers)

    let n = sizeof(numbers)

    # 插入排序实现
    for (i in 1, n - 1) {
        let key = numbers[i]
        let j = i - 1

        # 将大于key的元素向后移动
        while (j >= 0 and numbers[j] > key) {
            numbers[j + 1] = numbers[j]
            j = j - 1
        }

        numbers[j + 1] = key
        println('第', i, '轮:', numbers)
    }

    println('最终结果:', numbers)
}
```

</details>

### 练习 2：质数判断

编写程序判断一个数是否为质数。

<details>
<summary>查看答案</summary>

```losu
import std

main {
    let numbers = [2, 3, 4, 17, 25, 29, 30]

    println('=== 质数判断 ===')

    for (num in numbers) {
        let isPrime = true

        if (num < 2) {
            isPrime = false
        } else {
            for (i in 2, num - 1) {
                if (num % i == 0) {
                    isPrime = false
                    break
                }
            }
        }

        if (isPrime) {
            println(num, '是质数')
        } else {
            println(num, '不是质数')
        }
    }
}
```

</details>