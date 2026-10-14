// // quantum_ai_classifier.c
// #include "quest/include/deprecated.h"
// #include "quest/include/quest.h"

// #include <math.h>
// #include <stdio.h>

// // 鸢尾花数据子集：Setosa (label=0) vs Versicolor (label=1)
// // 每条数据：[sepal_length, sepal_width] → 已归一化到 [0, π]
// double dataset[][3]
//     = { { 0.1, 0.2, 0.0 }, { 0.3, 0.4, 0.0 }, { 0.5, 0.6, 1.0 },
//         { 0.7, 0.8, 1.0 }, { 0.2, 0.5, 0.0 }, { 0.6, 0.7, 1.0 } };
// #define N_DATA 6
// #define N_FEATURES 2
// #define N_QUBITS 2

// // 前向传播：执行量子电路并测量 ⟨Z1⟩
// qreal
// forward (Qureg qreg, double x1, double x2, double theta1, double theta2,
//          double theta3)
// {
//   // 初始化
//   initZeroState (qreg);

//   // === 1. 数据编码（Angle Embedding）===
//   rotateX (qreg, 0, x1);
//   rotateX (qreg, 1, x2);

//   // === 2. 变分层（VQC 层）===
//   hadamard (qreg, 0);
//   hadamard (qreg, 1);
//   controlledNot (qreg, 0, 1); // CNOT 缠结
//   rotateZ (qreg, 0, theta1);  // 参数化旋转
//   rotateY (qreg, 1, theta2);
//   controlledNot (qreg, 0, 1);
//   rotateZ (qreg, 1, theta3);

//   // === 3. 测量输出 qubit（qubit 1）的期望值 ⟨Z⟩ ===
//   qreal z_expect = 0;
//   const int shots = 500;
//   for (int i = 0; i < shots; i++)
//     {
//       int m = measure (qreg, 1);     // 测量结果为 0 或 1
//       z_expect += (m == 0 ? 1 : -1); // Z = +1 if |0>, -1 if |1>
//     }
//   return z_expect / shots; // ⟨Z⟩ ∈ [-1, 1]
// }

// // Sigmoid 激活函数（将量子输出转为概率）
// double
// sigmoid (double x)
// {
//   return 1.0 / (1.0 + exp (-x));
// }

// // 交叉熵损失
// double
// loss_function (qreal z, double label)
// {
//   double prob = sigmoid (z); // 映射到 [0,1]
//   double eps = 1e-7;
//   prob = fmax (eps, fmin (1 - eps, prob)); // 防止 log(0)
//   return -(label * log (prob) + (1 - label) * log (1 - prob));
// }

// int
// main ()
// {
//   printf ("🧠 量子AI分类器启动（基于变分量子电路 VQC）\n");
//   printf ("任务：鸢尾花 Setosa vs Versicolor 二分类\n");

//   // 初始化 QuEST
//   QuESTEnv env = createQuESTEnv ();
//   Qureg qreg = createQureg (N_QUBITS, env);

//   // 超参数
//   double params[3] = { 0.5, 0.5, 0.5 }; // 初始参数 θ1, θ2, θ3
//   double lr = 0.1;
//   int epochs = 100;

//   printf ("🚀 开始训练... 共 %d 轮\n", epochs);

//   for (int epoch = 0; epoch < epochs; epoch++)
//     {
//       double total_loss = 0;
//       double grad[3] = { 0 };

//       for (int i = 0; i < N_DATA; i++)
//         {
//           double x1 = dataset[i][0];
//           double x2 = dataset[i][1];
//           double label = dataset[i][2];

//           // 当前预测
//           qreal z_pred
//               = forward (qreg, x1, x2, params[0], params[1], params[2]);
//           double loss = loss_function (z_pred, label);
//           total_loss += loss;

//           // 数值梯度（中心差分）
//           for (int j = 0; j < 3; j++)
//             {
//               params[j] += 0.01;
//               qreal z_plus
//                   = forward (qreg, x1, x2, params[0], params[1], params[2]);
//               double l_plus = loss_function (z_plus, label);

//               params[j] -= 0.02;
//               qreal z_minus
//                   = forward (qreg, x1, x2, params[0], params[1], params[2]);
//               double l_minus = loss_function (z_minus, label);

//               grad[j] += (l_plus - l_minus) / 0.02;
//               params[j] += 0.01;
//             }
//         }

//       // 平均梯度 & 更新
//       for (int j = 0; j < 3; j++)
//         {
//           grad[j] /= N_DATA;
//           params[j] -= lr * grad[j];
//         }
//       total_loss /= N_DATA;

//       if (epoch % 20 == 0)
//         {
//           printf ("Epoch %3d | Loss: %.4f | Params: [%.3f, %.3f, %.3f]\n",
//                   epoch, total_loss, params[0], params[1], params[2]);
//         }
//     }

//   // === 测试准确率 ===
//   int correct = 0;
//   for (int i = 0; i < N_DATA; i++)
//     {
//       double x1 = dataset[i][0], x2 = dataset[i][1], label = dataset[i][2];
//       qreal z = forward (qreg, x1, x2, params[0], params[1], params[2]);
//       int pred = (sigmoid (z) > 0.5) ? 1 : 0;
//       if (pred == label)
//         correct++;
//     }
//   printf ("\n🎯 训练完成！最终准确率: %.2f%%\n", 100.0 * correct / N_DATA);

//   // 输出最优参数（可用于部署）
//   printf ("🔧 最优参数: θ1=%.3f, θ2=%.3f, θ3=%.3f\n", params[0], params[1],
//           params[2]);

//   // 清理
//   destroyQureg (qreg, env);
//   destroyQuESTEnv (env);

//   return 0;
// }