# 洛书量子编程语言语法

## 1. 基础语法

### 1.1 变量声明

使用 `var` 关键字声明变量：

```losu
var x = 10
var name = "量子计算"
var flag = true
```

### 1.2 数据类型

- **数字**: `42`, `3.14`, `1e-5`
- **字符串**: `"hello"`, `'world'`
- **布尔值**: `true`, `false`
- **空值**: `nil`

### 1.3 运算符

**算术运算符**: `+`, `-`, `*`, `/`, `%`, `^` (幂运算)

**比较运算符**: `==`, `!=`, `<`, `>`, `<=`, `>=`

**逻辑运算符**: `and`, `or`, `not`

---

## 2. 控制流

### 2.1 条件语句

```losu
if x > 0 {
  输出("正数")
} else if x < 0 {
  输出("负数")
} else {
  输出("零")
}
```

### 2.2 循环语句

**for 循环**:
```losu
for i in 范围(10) {
  输出(i)
}
```

**while 循环**:
```losu
var i = 0
while i < 10 {
  输出(i)
  i = i + 1
}
```

---

## 3. 量子编程

### 3.1 量子寄存器

创建量子寄存器：

```losu
var q = 量子(3)  // 创建3个量子比特的寄存器
```

量子态表示为：

$$|\psi\rangle = \alpha|0\rangle + \beta|1\rangle$$

其中归一化条件：$|\alpha|^2 + |\beta|^2 = 1$

### 3.2 量子门操作

**Hadamard 门** (创建叠加态):
```losu
H(q, 0)  // 对第0个量子比特应用H门
```

数学表示：

$$H = \frac{1}{\sqrt{2}}\begin{pmatrix}1 & 1\\1 & -1\end{pmatrix}$$

$$H|0\rangle = \frac{|0\rangle + |1\rangle}{\sqrt{2}}$$

**Pauli 门**:
```losu
X(q, 0)  // Pauli-X (量子NOT)
Y(q, 0)  // Pauli-Y
Z(q, 0)  // Pauli-Z
```

矩阵表示：

$$X = \begin{pmatrix}0 & 1\\1 & 0\end{pmatrix}, \quad Y = \begin{pmatrix}0 & -i\\i & 0\end{pmatrix}, \quad Z = \begin{pmatrix}1 & 0\\0 & -1\end{pmatrix}$$

**旋转门**:
```losu
Rx(q, 0, 3.14159/4)  // 绕X轴旋转π/4
Ry(q, 0, 3.14159/2)  // 绕Y轴旋转π/2
Rz(q, 0, 3.14159)    // 绕Z轴旋转π
```

旋转门定义：

$$R_x(\theta) = e^{-i\theta X/2} = \begin{pmatrix}\cos(\theta/2) & -i\sin(\theta/2)\\-i\sin(\theta/2) & \cos(\theta/2)\end{pmatrix}$$

**受控门**:
```losu
CNOT(q, 0, 1)  // 控制比特0，目标比特1
CZ(q, 0, 1)    // 受控Z门
```

CNOT门矩阵（4×4）：

$$\text{CNOT} = \begin{pmatrix}1&0&0&0\\0&1&0&0\\0&0&0&1\\0&0&1&0\end{pmatrix}$$

作用：$\text{CNOT}|c,t\rangle = |c, c \oplus t\rangle$

### 3.3 测量

```losu
var result = 测量(q, 0)  // 测量第0个量子比特
输出("测量结果:", result)
```

测量导致波函数坍缩，概率为：

$$P(|0\rangle) = |\alpha|^2, \quad P(|1\rangle) = |\beta|^2$$

---

## 4. 函数定义

### 4.1 普通函数

```losu
function 求和(a, b) {
  return a + b
}

var result = 求和(10, 20)
```

### 4.2 量子算法示例

**量子叠加**:
```losu
function 创建叠加态(n) {
  var q = 量子(n)
  for i in 范围(n) {
    H(q, i)
  }
  return q
}
```

**量子纠缠** (Bell态):
```losu
function 创建Bell态() {
  var q = 量子(2)
  H(q, 0)
  CNOT(q, 0, 1)
  return q
}
```

创建的态为：

$$|\Phi^+\rangle = \frac{|00\rangle + |11\rangle}{\sqrt{2}}$$

**量子相位估计** (简化):
```losu
function 相位估计(U, 特征态, 精度) {
  var 控制 = 量子(精度)
  var 目标 = 特征态
  
  for i in 范围(精度) {
    H(控制, i)
  }
  
  for i in 范围(精度) {
    // 受控U^(2^i)操作
    控制U(控制, i, 目标, 2^i)
  }
  
  // 逆量子傅里叶变换
  QFT逆(控制)
  
  return 测量全部(控制)
}
```

---

## 5. 内置函数

### 5.1 输入输出

- `输出(...)` - 打印输出
- `输入(prompt)` - 获取用户输入

### 5.2 数学函数

- `sin(x)`, `cos(x)`, `tan(x)` - 三角函数
- `sqrt(x)` - 平方根
- `abs(x)` - 绝对值
- `floor(x)`, `ceil(x)` - 向下/向上取整

### 5.3 集合操作

- `范围(n)` - 生成 0 到 n-1 的序列
- `长度(arr)` - 获取数组长度

---

## 6. 量子算法示例

### 6.1 Deutsch-Jozsa 算法

判断布尔函数是常数还是平衡的：

```losu
function DeutschJozsa(oracle, n) {
  var q = 量子(n + 1)
  
  // 初始化
  X(q, n)  // 辅助比特设为|1⟩
  for i in 范围(n + 1) {
    H(q, i)
  }
  
  // 应用oracle
  oracle(q)
  
  // 再次应用H门
  for i in 范围(n) {
    H(q, i)
  }
  
  // 测量
  var result = 测量全部(q, 范围(n))
  return result == 0  // 全为0则是常数
}
```

### 6.2 Grover 搜索算法

在未排序数据库中搜索：

```losu
function Grover搜索(oracle, n) {
  var q = 量子(n)
  var 迭代次数 = floor(3.14159 / 4 * sqrt(2^n))
  
  // 初始化叠加态
  for i in 范围(n) {
    H(q, i)
  }
  
  // Grover迭代
  for iter in 范围(迭代次数) {
    oracle(q)           // 标记目标态
    H全部(q)            // H门
    相位反转(q)         // 关于均值反转
    H全部(q)
  }
  
  return 测量全部(q)
}
```

Grover算法复杂度：$O(\sqrt{N})$

---

## 7. 高级特性

### 7.1 量子傅里叶变换 (QFT)

```losu
function QFT(q, n) {
  for j in 范围(n) {
    H(q, j)
    for k in 范围(j + 1, n) {
      var angle = 3.14159 / (2 ^ (k - j))
      控制Rz(q, k, j, angle)
    }
  }
  // 反转量子比特顺序
  for i in 范围(n / 2) {
    SWAP(q, i, n - 1 - i)
  }
}
```

QFT变换：

$$|j\rangle \xrightarrow{\text{QFT}} \frac{1}{\sqrt{N}}\sum_{k=0}^{N-1} e^{2\pi ijk/N}|k\rangle$$

### 7.2 量子纠错码

简单的3比特重复码：

```losu
function 编码3比特(q, data_qubit) {
  CNOT(q, data_qubit, data_qubit + 1)
  CNOT(q, data_qubit, data_qubit + 2)
}

function 纠错3比特(q, data_qubit) {
  // 测量校验子
  var s1 = 测量校验(q, data_qubit, data_qubit + 1)
  var s2 = 测量校验(q, data_qubit, data_qubit + 2)
  
  if s1 and s2 {
    X(q, data_qubit)
  } else if s1 {
    X(q, data_qubit + 1)
  } else if s2 {
    X(q, data_qubit + 2)
  }
}
```

---

## 8. 最佳实践

### 8.1 量子态准备

总是从 $|0\rangle$ 态开始，然后应用门操作构造目标态。

### 8.2 测量时机

只在算法末尾测量，中间测量会破坏叠加态。

### 8.3 资源管理

量子比特是稀缺资源，尽量复用。

### 8.4 错误处理

量子计算易受噪声影响，需要多次运行取统计结果。

---

## 9. 参考公式

**贝尔不等式**:

$$|E(a,b) - E(a,c)| \leq 1 + E(b,c)$$

**薛定谔方程**:

$$i\hbar\frac{\partial}{\partial t}|\psi(t)\rangle = \hat{H}|\psi(t)\rangle$$

**酉演化**:

$$|\psi(t)\rangle = e^{-i\hat{H}t/\hbar}|\psi(0)\rangle$$

**密度矩阵**:

$$\rho = \sum_i p_i |\psi_i\rangle\langle\psi_i|$$

**冯诺依曼熵**:

$$S(\rho) = -\text{Tr}(\rho \log \rho)$$

---

## 10. 常见错误

1. **忘记归一化**: 量子态必须满足 $\langle\psi|\psi\rangle = 1$
2. **测量后继续操作**: 测量导致坍缩，不能再期望叠加态
3. **受控门顺序错误**: CNOT(q, c, t) 中 c 是控制位，t 是目标位
4. **角度单位**: 旋转门使用弧度制，不是角度

---

## 附录：常用量子门速查

| 门 | 符号 | 矩阵 | 用途 |
|---|---|---|---|
| H | `H(q,i)` | $\frac{1}{\sqrt{2}}\begin{pmatrix}1&1\\1&-1\end{pmatrix}$ | 创建叠加态 |
| X | `X(q,i)` | $\begin{pmatrix}0&1\\1&0\end{pmatrix}$ | 量子NOT |
| Z | `Z(q,i)` | $\begin{pmatrix}1&0\\0&-1\end{pmatrix}$ | 相位翻转 |
| CNOT | `CNOT(q,c,t)` | 见上文 | 创建纠缠 |
| T | `T(q,i)` | $\begin{pmatrix}1&0\\0&e^{i\pi/4}\end{pmatrix}$ | π/8 相位 |

**使用提示**: 任何量子算法都可以分解为H、T、CNOT门的组合（通用门集）。
