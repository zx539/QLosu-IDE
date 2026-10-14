/**
 * QLosu FFI 函数注册表（QLosu 构建适配代码）
 *
 * 将 ffi.c 中声明的量子 FFI 函数注册进 Losu 虚拟机，
 * 使 losu 脚本可通过 `$(函数名)` 语法调用 QuEST 量子计算接口。
 */

#include "losu_base.h"

/** @note 内置 FFI 函数表（来自 ffi.c） */

losu_base_ffi_declare (initQuESTEnv, vm, ctx, recall);
losu_base_ffi_declare (calcProbOfQubitOutcome, vm, ctx, recall);
losu_base_ffi_declare (createQureg, vm, ctx, recall);
losu_base_ffi_declare (reportQureg, vm, ctx, recall);
losu_base_ffi_declare (applyHadamard, vm, ctx, recall);
losu_base_ffi_declare (applyPauliX, vm, ctx, recall);
losu_base_ffi_declare (applyPauliZ, vm, ctx, recall);
losu_base_ffi_declare (applyControlledPauliX, vm, ctx, recall);
losu_base_ffi_declare (applyRotateX, vm, ctx, recall);
losu_base_ffi_declare (applyToffoli, vm, ctx, recall);
losu_base_ffi_declare (applyQubitMeasurement, vm, ctx, recall);
losu_base_ffi_declare (println, vm, ctx, recall);
losu_base_ffi_declare (eprintln, vm, ctx, recall);
losu_base_ffi_declare (warnprintln, vm, ctx, recall);
losu_base_ffi_declare (PI, vm, ctx, recall);

struct losu_base_ffi_t losu_base_ffi_index[] = {
  losu_base_ffi_include (initQuESTEnv),
  losu_base_ffi_include (calcProbOfQubitOutcome),
  losu_base_ffi_include (createQureg),
  losu_base_ffi_include (reportQureg),
  losu_base_ffi_include (applyHadamard),
  losu_base_ffi_include (applyPauliX),
  losu_base_ffi_include (applyPauliZ),
  losu_base_ffi_include (applyControlledPauliX),
  losu_base_ffi_include (applyRotateX),
  losu_base_ffi_include (applyToffoli),
  losu_base_ffi_include (applyQubitMeasurement),
  losu_base_ffi_include (println),
  losu_base_ffi_include (eprintln),
  losu_base_ffi_include (warnprintln),
  losu_base_ffi_include (PI),
};
size_t losu_base_ffi_nindex
    = sizeof (losu_base_ffi_index) / sizeof (losu_base_ffi_index[0]);
