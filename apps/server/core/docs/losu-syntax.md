# 洛书量子编程语言语法

洛书（Losu）是一种专为量子计算设计的高级编程语言，提供直观的语法来操作量子态和构建量子算法。

---

## 基本结构

### 程序入口

```losu
func main() {
  // 主程序代码
}
```

每个洛书程序必须包含一个 `main` 函数作为程序入口。

---

## 数据类型

### 量子比特寄存器

```losu
qreg q[3];  // 声明一个包含3个量子比特的寄存器
```

量子比特初始化为 $|0\rangle$ 态。

### 经典变量

```losu
int n = 5;
float theta = 3.14159;
```

---

## 量子门操作

### 单量子比特门

#### Hadamard 门（H）

创建叠加态：

$$H|0\rangle = \frac{1}{\sqrt{2}}(|0\rangle + |1\rangle)$$

```losu
H q[0];  // 对第0个量子比特应用 Hadamard 门
```

#### Pauli 门（X, Y, Z）

- **X 门**：量子比特翻转 $X|0\rangle = |1\rangle$
- **Y 门**：相位翻转 + 比特翻转
- **Z 门**：相位翻转 $Z|1\rangle = -|1\rangle$

```losu
X q[1];  // 比特翻转
Y q[2];  // Y 门
Z q[0];  // 相位翻转
```

#### 旋转门（Rx, Ry, Rz）

绕 X/Y/Z 轴旋转指定角度：

$$R_x(\theta) = e^{-i\theta X/2}$$

```losu
Rx(1.57, q[0]);  // 绕 X 轴旋转 π/2
Ry(3.14, q[1]);  // 绕 Y 轴旋转 π
Rz(0.785, q[2]); // 绕 Z 轴旋转 π/4
```

### 双量子比特门

#### CNOT 门（受控非门）

```losu
CNOT q[0], q[1];  // q[0] 为控制位，q[1] 为目标位
```

当控制位为 $|1\rangle$ 时，对目标位应用 X 门。

#### CZ 门（受控 Z 门）

```losu
CZ q[0], q[1];  // 受控相位翻转
```

---

## 测量

### 单比特测量

```losu
measure q[0] -> c[0];  // 测量 q[0] 并存储到经典位 c[0]
```

测量会使量子态坍缩到基态 $|0\rangle$ 或 $|1\rangle$，概率由振幅平方决定：

$$P(|0\rangle) = |\alpha|^2, \quad P(|1\rangle) = |\beta|^2$$

其中 $|\psi\rangle = \alpha|0\rangle + \beta|1\rangle$

### 全寄存器测量

```losu
measure q;  // 测量寄存器 q 的所有量子比特
```

---

## 控制流

### 条件语句

```losu
if (condition) {
  // 真分支
} else {
  // 假分支
}
```

### 循环

```losu
for (int i = 0; i < 5; i = i + 1) {
  H q[i];
}

while (i < 10) {
  i = i + 1;
}
```

---

## 函数定义

```losu
func oracle(qreg q[2]) {
  X q[0];
  CNOT q[0], q[1];
  X q[0];
}

func main() {
  qreg q[2];
  oracle(q);
}
```

---

## 量子算法示例

### Bell 态制备

创建最大纠缠态：

$$|\Phi^+\rangle = \frac{1}{\sqrt{2}}(|00\rangle + |11\rangle)$$

```losu
func main() {
  qreg q[2];
  
  H q[0];         // 创建叠加态
  CNOT q[0], q[1]; // 纠缠两个量子比特
  
  measure q;
  print("Bell 态已创建");
}
```

### Deutsch 算法

判断函数是常数还是平衡的：

```losu
func oracle_balanced(qreg q[2]) {
  CNOT q[0], q[1];
}

func deutsch() {
  qreg q[2];
  
  X q[1];         // 初始化辅助比特为 |1⟩
  H q[0];
  H q[1];         // Hadamard 变换
  
  oracle_balanced(q);
  
  H q[0];         // 最终 Hadamard
  measure q[0];
}
```

### Grover 搜索

从 N 个元素中搜索目标，复杂度 $O(\sqrt{N})$：

```losu
func oracle(qreg q[2]) {
  // 标记 |11⟩ 态
  CZ q[0], q[1];
}

func diffusion(qreg q[2]) {
  H q[0];
  H q[1];
  X q[0];
  X q[1];
  CZ q[0], q[1];
  X q[0];
  X q[1];
  H q[0];
  H q[1];
}

func grover() {
  qreg q[2];
  
  H q[0];
  H q[1];         // 初始叠加态
  
  for (int i = 0; i < 1; i = i + 1) {
    oracle(q);
    diffusion(q);
  }
  
  measure q;
}
```

---

## 高级特性

### 参数化量子电路

```losu
func rotate_circuit(qreg q[1], float angle) {
  Rx(angle, q[0]);
  Ry(angle * 2, q[0]);
}
```

### 注释

```losu
// 单行注释

/*
 * 多行注释
 */
```

---

## 内置函数

- `print(string)` - 输出文本
- `sin(float)` - 正弦函数
- `cos(float)` - 余弦函数
- `sqrt(float)` - 平方根
- `pow(float, float)` - 幂运算

---

## 量子态表示

洛书运行时使用状态向量表示量子态。对于 n 个量子比特，状态向量有 $2^n$ 个复数分量：

$$|\psi\rangle = \sum_{i=0}^{2^n-1} \alpha_i |i\rangle$$

其中 $\sum_i |\alpha_i|^2 = 1$（归一化条件）

---

## 错误处理

编译时会检查：

- 量子门参数类型
- 寄存器索引越界
- 未声明的变量引用
- 函数签名匹配

运行时会报告：

- 量子态模拟错误
- 测量失败
- 超时限制

---

## 最佳实践

1. **最小化测量**：测量会破坏叠加态，尽量推迟到算法末尾
2. **利用纠缠**：量子优势来自纠缠态的并行计算
3. **控制深度**：量子电路越深，噪声累积越严重
4. **重用量子比特**：实际量子计算机比特数有限
5. **添加注释**：量子算法通常不直观，注释帮助理解

---

更多示例请参考左侧"示例"面板中的演示代码。
