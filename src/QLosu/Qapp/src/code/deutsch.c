#include "quest/include/quest.h"
#include <math.h>
#include <stdio.h>

// 定义 Oracle 类型
typedef enum
{
  CONSTANT_0,
  CONSTANT_1,
  BALANCED_IDENTITY, // f(x) = x
  BALANCED_NOT       // f(x) = !x
} OracleType;

/**
 * @brief 应用 Deutsch 算法的 Oracle U_f
 * @param reg 量子寄存器
 * @param control 控制量子比特索引 (输入 x)
 * @param target 目标量子比特索引 (输出 f(x))
 * @param type Oracle 的类型
 */
void
applyDeutschOracle (Qureg reg, int control, int target, OracleType type)
{
  switch (type)
    {
    case CONSTANT_0:
      // f(x) = 0: 不做任何操作 (Identity)
      break;

    case CONSTANT_1:
      // f(x) = 1: 对目标位应用 X 门
      applyPauliX (reg, target);
      break;

    case BALANCED_IDENTITY:
      // f(x) = x: 应用 CNOT (Control -> Target)
      // 修复：使用标准的 applyControlledPauliX
      applyControlledPauliX (reg, control, target);
      break;

    case BALANCED_NOT:
      // f(x) = !x:
      // 逻辑：y XOR !x = y XOR x XOR 1
      // 实现：先 CNOT(control, target)，再 X(target)
      // 修复：使用标准的 applyControlledPauliX
      applyControlledPauliX (reg, control, target);
      applyPauliX (reg, target);
      break;
    }
}

/**
 * @brief 运行 Deutsch 算法
 * @param oracleType 要测试的 Oracle 类型
 */
void
runDeutschAlgorithm (OracleType oracleType)
{

  // 创建2个量子比特的寄存器
  Qureg reg = createQureg (2);

  // 1. 初始化状态
  // q[0] (输入) -> |0>
  // q[1] (辅助) -> |1> (通过 X 门)
  applyPauliX (reg, 1);

  // 2. 叠加态
  // H(q[0]) 和 H(q[1])
  applyHadamard (reg, 1);
  applyHadamard (reg, 0);

  // 3. 应用 Oracle
  applyDeutschOracle (reg, 0, 1, oracleType);

  // 4. 对输入位再次应用 H 门进行干涉
  applyHadamard (reg, 0);

  // 5. 测量输入位 (q[0])
  int result = applyQubitMeasurement (reg, 0);
  // 输出结果
  printf ("Oracle Type: ");
  switch (oracleType)
    {
    case CONSTANT_0:
      printf ("CONSTANT_0");
      break;
    case CONSTANT_1:
      printf ("CONSTANT_1");
      break;
    case BALANCED_IDENTITY:
      printf ("BALANCED_IDENTITY");
      break;
    case BALANCED_NOT:
      printf ("BALANCED_NOT");
      break;
    }
  printf ("\nMeasurement Result (q[0]): %d\n", result);

  if (result == 0)
    {
      printf ("Conclusion: The function is CONSTANT.\n");
    }
  else
    {
      printf ("Conclusion: The function is BALANCED.\n");
    }

  // 清理资源
  destroyQureg (reg);
}

int
main ()
{
  initQuESTEnv ();

  printf ("--- Running Deutsch Algorithm ---\n\n");

  // 测试所有四种情况
  runDeutschAlgorithm (CONSTANT_0);
  printf ("\n");

  runDeutschAlgorithm (CONSTANT_1);
  printf ("\n");

  runDeutschAlgorithm (BALANCED_IDENTITY);
  printf ("\n");

  runDeutschAlgorithm (BALANCED_NOT);
  printf ("\n");
  finalizeQuESTEnv ();
  // double prob00 = calcProbOfQubitOutcome();
  return 0;
}