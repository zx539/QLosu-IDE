# 洛书量子编程语言语法手册

## 1. 基本语法

### 1.1 变量声明

```losu
var x = 10;              // 整数
var y = 3.14;            // 浮点数
var name = "Alice";      // 字符串
var flag = true;         // 布尔值
```

### 1.2 导入语句

```losu
// 导入单个函数
var { println } import qufu.IO;

// 导入类型
var QuReg import qufu.QuReg;

// 导入多个符号
var { Hadamard: H, PauliX: X } import qufu.Gates;
```

### 1.3 控制流

#### 条件语句

```losu
if (x > 0) {
  println("正数");
} else if (x < 0) {
  println("负数");
} else {
  println("零");
}
```

#### 循环语句

```losu
// for 循环
for (var i = 0; i < 10; i = i + 1) {
  println(i);
}

// while 循环
var i = 0;
while (i < 10) {
  println(i);
  i = i + 1;
}
```

---

## 2. 量子编程

### 2.1 量子寄存器

```losu
// 创建 n 比特量子寄存器
var reg = QuReg(2);

// 访问单个量子比特
var q0 = reg[[0]];
var q1 = reg[[1]];
```

### 2.2 量子门操作

#### 基本单比特门

```losu
// Hadamard 门 - 叠加态
q0 :> Hadamard();

// Pauli X 门 - 量子非门
q0 :> PauliX();

// Pauli Y 门
q0 :> PauliY();

// Pauli Z 门
q0 :> PauliZ();

// 相位门
q0 :> PhaseShift(π / 4);

// 旋转门
q0 :> RotateX(π / 2);
q0 :> RotateY(π / 2);
q0 :> RotateZ(π / 2);
```

#### 双比特门

```losu
// CNOT 门 - 受控非门
// 语法：目标 :> ControlledPauliX(控制)
q1 :> ControlledPauliX(q0);

// SWAP 门 - 交换两比特
q0 :> Swap(q1);

// 受控相位门
q1 :> ControlledPhaseShift(q0, π / 4);
```

### 2.3 测量操作

```losu
// 测量单个比特
var result = Measure(q0);  // 结果为 0 或 1

// 获取测得某个值的概率
var p0 = q0(0);  // 测得 0 的概率
var p1 = q1(1);  // 测得 1 的概率
```

---

## 3. 经典算法示例

### 3.1 贝尔态制备

制备最大纠缠态 $|\Phi^+\rangle = \frac{1}{\sqrt{2}}(|00\rangle + |11\rangle)$

```losu
var { println } import qufu.IO;
var QuReg import qufu.QuReg;
var { Hadamard, ControlledPauliX } import qufu.Gates;
var { Measure } import qufu;

var reg = QuReg(2);
var q0 = reg[[0]];
var q1 = reg[[1]];

// 第一步：q0 应用 Hadamard 门
q0 :> Hadamard();

// 第二步：以 q0 为控制，q1 为目标，应用 CNOT
q1 :> ControlledPauliX(q0);

// 测量结果必然相同（00 或 11）
println("测量结果: ", Measure(q0), " ", Measure(q1));
```

### 3.2 量子隐形传态

Alice 将未知量子态传送给 Bob，利用纠缠和经典通信。

```losu
var { println } import qufu.IO;
var QuReg import qufu.QuReg;
var { Hadamard, ControlledPauliX, PauliX, PauliZ } import qufu.Gates;
var { Measure } import qufu;

// 待传送的量子态（Alice 持有）
var alice = QuReg(1);
var q_send = alice[[0]];
q_send :> RotateY(π / 3);  // 任意态

// Alice 和 Bob 共享的纠缠对
var shared = QuReg(2);
var q_alice = shared[[0]];
var q_bob = shared[[1]];

// 制备贝尔态
q_alice :> Hadamard();
q_bob :> ControlledPauliX(q_alice);

// Alice 的贝尔测量
q_send :> ControlledPauliX(q_alice);
q_send :> Hadamard();
var m1 = Measure(q_send);
var m2 = Measure(q_alice);

// Bob 根据测量结果修正
if (m2 == 1) {
  q_bob :> PauliX();
}
if (m1 == 1) {
  q_bob :> PauliZ();
}

// Bob 现在拥有原始态
println("传态完成");
```

### 3.3 Deutsch-Jozsa 算法

判断函数 $f: \{0,1\}^n \to \{0,1\}$ 是常数还是平衡。

```losu
var { println } import qufu.IO;
var QuReg import qufu.QuReg;
var { Hadamard, PauliX, ControlledPauliX } import qufu.Gates;
var { Measure } import qufu;

var n = 3;  // 输入比特数
var reg = QuReg(n + 1);

// 最后一位作为辅助
var aux = reg[[n]];
aux :> PauliX();
aux :> Hadamard();

// 所有输入比特应用 Hadamard
for (var i = 0; i < n; i = i + 1) {
  reg[[i]] :> Hadamard();
}

// Oracle（这里假设常数函数 f(x) = 0）
// 平衡函数会翻转一半的输入

// 再次应用 Hadamard
for (var i = 0; i < n; i = i + 1) {
  reg[[i]] :> Hadamard();
}

// 测量：全 0 = 常数，否则 = 平衡
var result = 0;
for (var i = 0; i < n; i = i + 1) {
  result = result + Measure(reg[[i]]);
}

if (result == 0) {
  println("常数函数");
} else {
  println("平衡函数");
}
```

### 3.4 Grover 搜索算法

在 $N = 2^n$ 个未排序项中查找标记项，量子加速至 $O(\sqrt{N})$。

```losu
var { println } import qufu.IO;
var QuReg import qufu.QuReg;
var { Hadamard, PauliX, PauliZ, ControlledPauliZ } import qufu.Gates;
var { Measure } import qufu;

var n = 3;  // 比特数
var reg = QuReg(n);

// 初始化：所有比特叠加态
for (var i = 0; i < n; i = i + 1) {
  reg[[i]] :> Hadamard();
}

// 迭代次数：约 π/4 * sqrt(N)
var iterations = 2;  // 对于 n=3, N=8

for (var iter = 0; iter < iterations; iter = iter + 1) {
  // Oracle：标记目标态（假设搜索 |101⟩）
  reg[[0]] :> PauliX();
  reg[[2]] :> PauliX();
  // 多控制 Z 门
  reg[[n - 1]] :> ControlledPauliZ(reg[[0]], reg[[1]], ...);
  reg[[0]] :> PauliX();
  reg[[2]] :> PauliX();

  // Diffusion 算子
  for (var i = 0; i < n; i = i + 1) {
    reg[[i]] :> Hadamard();
    reg[[i]] :> PauliX();
  }
  reg[[n - 1]] :> ControlledPauliZ(reg[[0]], reg[[1]], ...);
  for (var i = 0; i < n; i = i + 1) {
    reg[[i]] :> PauliX();
    reg[[i]] :> Hadamard();
  }
}

// 测量
println("搜索结果: ");
for (var i = 0; i < n; i = i + 1) {
  println(Measure(reg[[i]]));
}
```

---

## 4. 数学函数

### 4.1 常量

```losu
var pi = π;      // 圆周率
var e = 2.71828; // 自然对数底
```

### 4.2 三角函数

```losu
var s = sin(π / 2);    // 正弦
var c = cos(0);        // 余弦
var t = tan(π / 4);    // 正切
```

### 4.3 其他函数

```losu
var a = abs(-5);       // 绝对值
var sq = sqrt(16);     // 平方根
var p = pow(2, 10);    // 幂运算
var l = log(e);        // 自然对数
```

---

## 5. 输入输出

### 5.1 打印

```losu
var { println, print } import qufu.IO;

println("Hello, World!");        // 打印并换行
print("Hello, ");                // 打印不换行
println("World!");

// 打印多个值
println("x =", x, "y =", y);
```

### 5.2 格式化

```losu
// 拼接字符串
var msg = "Result: " + result;
println(msg);

// 数字转字符串
var s = toString(123);
```

---

## 6. 高级特性

### 6.1 函数定义

```losu
function greet(name) {
  println("你好, " + name);
}

greet("Alice");
```

### 6.2 递归

```losu
function factorial(n) {
  if (n <= 1) {
    return 1;
  } else {
    return n * factorial(n - 1);
  }
}

println(factorial(5));  // 120
```

---

## 7. 常见错误与调试

### 7.1 量子测量后态坍缩

```losu
var reg = QuReg(1);
var q = reg[[0]];
q :> Hadamard();  // 叠加态

var m = Measure(q);  // 测量后坍缩为 |0⟩ 或 |1⟩
// 此后 q 不再处于叠加态
```

### 7.2 控制门语法

```losu
// ✅ 正确：目标 :> 门(控制)
q1 :> ControlledPauliX(q0);

// ❌ 错误
q0 :> ControlledPauliX(q1);  // q0 应为控制而非目标
```

### 7.3 比特索引越界

```losu
var reg = QuReg(2);  // 仅 q0, q1
var q2 = reg[[2]];   // ❌ 越界错误
```

---

## 8. 学习资源

- **量子计算基础**：Nielsen & Chuang《量子计算与量子信息》
- **QuEST 文档**：https://quest.qtechtheory.org/
- **Losu 示例**：IDE 左侧"示例"标签

---

**快捷键**  
- `Ctrl+Enter`：运行代码  
- `Ctrl+S`：保存文件  
- `Ctrl+O`：打开文件  

**可视化**  
- 运行后自动生成量子线路图  
- 步进调试：观察每步量子态变化  
- 程序流程图：显示算法结构
