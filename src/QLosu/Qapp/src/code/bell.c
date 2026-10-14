#include "quest/include/quest.h"
#include <stdio.h>
int
main ()
{
  // 1. 初始化 QuEST 环境
  initQuESTEnv ();

  // 2. 创建一个包含 2 个量子比特的寄存器
  // 默认初始化为 |00> 态
  Qureg qubits = createQureg (2);

  printf ("--- 开始量子线路 ---\n");
  printf ("初始状态: |00>\n");

  // 3. 构建量子线路

  // 步骤 A: 在 qubit[0] 上应用 Hadamard 门
  // 作用: |0> -> (|0> + |1>)/sqrt(2)
  applyHadamard (qubits, 0);

  // 步骤 B: 应用 CNOT 门
  // 控制位: qubit[0], 目标位: qubit[1]
  // 作用: 如果 qubit[0] 是 |1>，则翻转 qubit[1]
  // 结果产生贝尔态: (|00> + |11>)/sqrt(2)
  applyControlledPauliX (qubits, 0, 1);

  // 4. 测量与输出
  // 我们使用 reportState 来打印整个波函数，查看概率幅
  reportQureg (qubits);

  // 5. 获取特定基态的概率
  // 计算测量得到 |00> 的概率
  double prob00 = calcProbOfQubitOutcome (qubits, 1, 0);

  printf ("The probability of |00> is %f\n", prob00);
  printf ("%d\n", applyQubitMeasurement (qubits, 1));

  return 0;
}