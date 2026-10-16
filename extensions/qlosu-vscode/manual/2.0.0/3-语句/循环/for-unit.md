# for 容器循环

for 容器循环是洛书语言中用于遍历容器元素的循环语句。洛书支持单变量遍历和双变量遍历两种语法。

## 基本语法

```losu
// 单变量遍历（获取值）
for(var i in unit){
  // 循环体代码
}

// 双变量遍历（获取键值对）
for (var key, value in unit) {
    // 循环体代码
}
```

> [!IMPORTANT]
> 容器循环中的"容器"指的是洛书的 unit 类型，包括[容器类型](../../2-数据类型/容器类型.md)中的所有格式与语法糖。
> 除数组格式外，其余格式的 unit 在遍历时均按照洛书底层哈希表存储结构顺序（随机顺序）遍历，而非按照字典顺序。

### 遍历 Unit

```losu
var print import print

print("数组格式：")
var numbers = [1, 2, 3, 4, 5]
for(var k,v in numbers) {
    print("k: " .. k->&String .. " v: " .. v->&String)
}

print("键值对格式")
var userInfo = {
  "name": "张三",
  // 键的双引号可以省略
  age: 25
}
for(var k,v in userInfo) {
    print("k: " .. k->&String .. " v: " .. v->&String)
}


print("array语法糖")
var MixedUnit = {
  name: "张三",
  age: 25,
  city: "北京";
  "a",false,3
}
for(var k,v in MixedUnit) {
    print("k: " .. k->&String .. " v: " .. v->&String)
}


print("export语法糖")
var a = "a"
var b = false
var c = 3
var MixedUnit = {
  ...
  a,b,c
}
for(var k,v in MixedUnit) {
    print("k: " .. k->&String .. " v: " .. v->&String)
}
```

## 练习

### 练习 1：统计字符出现次数

遍历一个字符数组，统计每个字符出现的次数。

<details>
<summary>查看答案</summary>

```losu
var print import print

var chars = ["a", "b", "a", "c", "b", "a"]
var counts = {}

print("=== 字符统计 ===")

for (var char in chars) {
    if (counts[char]) {
        counts[char] = counts[char] + 1
    } else {
        counts[char] = 1
    }
}

for (var char, count in counts) {
    print("字符" .. char .. "出现" .. count->&String .. "次")
}
```

</details>

### 练习 2：查找最大值和最小值

在一个数字数组中找出最大值和最小值。

<details>
<summary>查看答案</summary>

```losu
var print import print

var numbers = [45, 23, 67, 12, 89, 34, 56]
var max = numbers[0]
var min = numbers[0]

print("=== 查找最大值和最小值 ===")
print("原数组:", numbers)

for (var num in numbers) {
    if (num > max) {
        max = num
    }
    if (num < min) {
        min = num
    }
}

print("最大值:" .. max->&String)
print("最小值:" .. min->&String)
```

</details>

**上一章**：[← for-range循环](for-range.md) | **下一章**：[并发编程 →](../../4-并发编程/README.md)