#include "quest/include/quest.h"
#include <stdio.h>

int
main ()
{
  // 初始化QuEST环境
  initQuESTEnv ();
  Qureg reg = createQureg (3); // 创建3个量子比特的寄存器

  // X(q[2]);
  applyPauliX (reg, 2);

  // H(q); - 对所有量子比特应用Hadamard门
  applyHadamard (reg, 0);
  applyHadamard (reg, 1);
  applyHadamard (reg, 2);

  // oracle - Toffoli(q[0], q[1], q[2]);
  int controls[] = { 0, 1 };
  applyMultiControlledPauliX (reg, controls, 2,
                              2); // Toffoli门: 控制位0,1，目标位2

  // H(q[0]); H(q[1]);
  applyHadamard (reg, 0);
  applyHadamard (reg, 1);

  // 条件相位偏移 2|00><00|-I (Diffusion operator)
  applyPauliX (reg, 0);              // X(q[0]);
  applyPauliX (reg, 1);              // X(q[1]);
  applyHadamard (reg, 1);            // H(q[1]);
  applyControlledPauliX (reg, 0, 1); // CNOT(q[0], q[1]);
  applyHadamard (reg, 1);            // H(q[1]);
  applyPauliX (reg, 0);              // X(q[0]);
  applyPauliX (reg, 1);              // X(q[1]);

  // H(q); - 对所有量子比特再次应用Hadamard门
  applyHadamard (reg, 0);
  applyHadamard (reg, 1);
  applyHadamard (reg, 2);

  // M(q[0]); M(q[1]); - 测量量子比特0和1
  int result0 = applyQubitMeasurement (reg, 0);
  int result1 = applyQubitMeasurement (reg, 1);

  printf ("Measurement results: q[0]=%d, q[1]=%d\n", result0, result1);

  // 清理资源
  destroyQureg (reg);
  finalizeQuESTEnv ();

  return 0;
}