#ifdef NDEBUG
#undef NDEBUG
#endif

#include "losu_api.h"
#include "losu_base.h"
#include "losu_mem.h"
#include "losu_object.h"

#include "quest/include/quest.h"

#include <stdio.h>

static void
__release_QuESTEnv (void *vm, void *res)
{
  /* WASM 构建适配：QuEST 环境为进程级单例，仅初始化一次、不随 VM 销毁，
     保证多次运行脚本时量子环境始终可用（原 CLI 为单次运行不受影响） */
  (void)(vm);
  (void)(res);
  losu_mem_free ((losu_vm_t)vm, res);
}

// QuEnv
losu_base_ffi_declare (initQuESTEnv, vm, ctx, recall)
{
  (void)vm;
  (void)ctx;
  (void)recall;
  /* WASM 构建适配：QuEST 环境仅允许初始化一次，已初始化则直接复用 */
  if (!isQuESTEnvInit ())
    initQuESTEnv ();
  QuESTEnv *env = losu_mem_malloc (vm, sizeof (QuESTEnv));
  *env = getQuESTEnv ();
  return losu_api_obj_context_return (
      vm, ctx,
      losu_api_obj_resource_new (vm, "C::QuEST::QuESTEnv", env, NULL,
                                 __release_QuESTEnv));
}

// Quwire
losu_base_ffi_declare (calcProbOfQubitOutcome, vm, ctx, recall)
{
  (void)vm;
  (void)ctx;
  (void)recall;

  struct losu_object_t arg1 = losu_api_obj_context_args (vm, ctx, 1);
  struct losu_object_t arg2 = losu_api_obj_context_args (vm, ctx, 2);
  struct losu_object_t arg3 = losu_api_obj_context_args (vm, ctx, 3);
  assert (losu_api_obj_type (vm, arg1) == losu_types_enum_t_resource
          && losu_api_obj_type (vm, arg2) == losu_types_enum_t_float
          && losu_api_obj_type (vm, arg3) == losu_types_enum_t_float);

  Qureg qureg = *(Qureg *)losu_api_obj_resource_get (vm, arg1);
  int bits = (int)losu_api_obj_float_get (vm, arg2);
  int outcome = (int)losu_api_obj_float_get (vm, arg3);
  qreal real = calcProbOfQubitOutcome (qureg, bits, outcome);
  return losu_api_obj_context_return_float (vm, ctx, real);
}

// QuReg
static void
__release_QuReg (void *vm, void *res)
{
  (void)(vm);
  (void)(res);
  destroyQureg (*(Qureg *)res);
  losu_mem_free ((losu_vm_t)vm, res);
}

losu_base_ffi_declare (createQureg, vm, ctx, recall)
{
  (void)vm;
  (void)ctx;
  (void)recall;

  struct losu_object_t arg1 = losu_api_obj_context_args (vm, ctx, 1);
  assert (losu_api_obj_type (vm, arg1) == losu_types_enum_t_float);

  int nQubits = (int)losu_api_obj_float_get (vm, arg1);

  Qureg *qureg = losu_mem_malloc (vm, sizeof (Qureg));

  *qureg = createQureg (nQubits);

  printf ("<Qureg>\n");
  reportQureg (*qureg);

  printf ("</Qureg>\n");
  return losu_api_obj_context_return (
      vm, ctx,
      losu_api_obj_resource_new (vm, "C::QuEST::Qureg", qureg, NULL,
                                 __release_QuReg));
}

losu_base_ffi_declare (reportQureg, vm, ctx, recall)
{
  (void)vm;
  (void)ctx;
  (void)recall;
  struct losu_object_t arg1 = losu_api_obj_context_args (vm, ctx, 1);
  assert (losu_api_obj_type (vm, arg1) == losu_types_enum_t_resource);
  Qureg *qureg = losu_api_obj_resource_get (vm, arg1);
  reportQureg (*qureg);
  return losu_api_obj_context_return_float (vm, ctx, 1);
}

// Gates
losu_base_ffi_declare (applyHadamard, vm, ctx, recall)
{
  (void)vm;
  (void)ctx;
  (void)recall;
  struct losu_object_t arg1 = losu_api_obj_context_args (vm, ctx, 1);
  struct losu_object_t arg2 = losu_api_obj_context_args (vm, ctx, 2);
  assert (losu_api_obj_type (vm, arg1) == losu_types_enum_t_resource
          && losu_api_obj_type (vm, arg2) == losu_types_enum_t_float);
  Qureg *qureg = losu_api_obj_resource_get (vm, arg1);
  int i = losu_api_obj_float_get (vm, arg2);
  printf ("<Hadamard>\n");
  reportQureg (*qureg);
  applyHadamard (*qureg, i);
  reportQureg (*qureg);
  printf ("</Hadamard>\n");
  return losu_api_obj_context_return_float (vm, ctx, 1);
}

losu_base_ffi_declare (applyPauliX, vm, ctx, recall)
{
  (void)vm;
  (void)ctx;
  (void)recall;
  struct losu_object_t arg1 = losu_api_obj_context_args (vm, ctx, 1);
  struct losu_object_t arg2 = losu_api_obj_context_args (vm, ctx, 2);
  assert (losu_api_obj_type (vm, arg1) == losu_types_enum_t_resource
          && losu_api_obj_type (vm, arg2) == losu_types_enum_t_float);
  Qureg *qureg = losu_api_obj_resource_get (vm, arg1);
  int i = losu_api_obj_float_get (vm, arg2);
  printf ("<applyPauliX>\n");
  reportQureg (*qureg);
  applyPauliX (*qureg, i);
  reportQureg (*qureg);
  printf ("</applyPauliX>\n");
  return losu_api_obj_context_return_float (vm, ctx, 1);
}

losu_base_ffi_declare (applyPauliZ, vm, ctx, recall)
{
  (void)vm;
  (void)ctx;
  (void)recall;
  struct losu_object_t arg1 = losu_api_obj_context_args (vm, ctx, 1);
  struct losu_object_t arg2 = losu_api_obj_context_args (vm, ctx, 2);
  assert (losu_api_obj_type (vm, arg1) == losu_types_enum_t_resource
          && losu_api_obj_type (vm, arg2) == losu_types_enum_t_float);
  Qureg *qureg = losu_api_obj_resource_get (vm, arg1);
  int i = losu_api_obj_float_get (vm, arg2);
  printf ("<applyPauliZ>\n");
  reportQureg (*qureg);
  applyPauliZ (*qureg, i);
  reportQureg (*qureg);
  printf ("</applyPauliZ>\n");
  return losu_api_obj_context_return_float (vm, ctx, 1);
}

losu_base_ffi_declare (applyControlledPauliX, vm, ctx, recall)
{
  (void)vm;
  (void)ctx;
  (void)recall;
  struct losu_object_t arg1 = losu_api_obj_context_args (vm, ctx, 1);
  struct losu_object_t arg2 = losu_api_obj_context_args (vm, ctx, 2);
  struct losu_object_t arg3 = losu_api_obj_context_args (vm, ctx, 3);

  assert (losu_api_obj_type (vm, arg1) == losu_types_enum_t_resource
          && losu_api_obj_type (vm, arg2) == losu_types_enum_t_float
          && losu_api_obj_type (vm, arg3) == losu_types_enum_t_float);
  Qureg *qureg = losu_api_obj_resource_get (vm, arg1);
  int c = losu_api_obj_float_get (vm, arg2);
  int t = losu_api_obj_float_get (vm, arg3);
  printf ("<applyControlledPauliX>\n");
  reportQureg (*qureg);
  applyControlledPauliX (*qureg, c, t);
  reportQureg (*qureg);
  printf ("</applyControlledPauliX>\n");
  return losu_api_obj_context_return_float (vm, ctx, 1);
}

losu_base_ffi_declare (applyRotateX, vm, ctx, recall)
{
  (void)vm;
  (void)ctx;
  (void)recall;
  struct losu_object_t arg1 = losu_api_obj_context_args (vm, ctx, 1);
  struct losu_object_t arg2 = losu_api_obj_context_args (vm, ctx, 2);
  struct losu_object_t arg3 = losu_api_obj_context_args (vm, ctx, 3);

  assert (losu_api_obj_type (vm, arg1) == losu_types_enum_t_resource
          && losu_api_obj_type (vm, arg2) == losu_types_enum_t_float
          && losu_api_obj_type (vm, arg3) == losu_types_enum_t_float);
  Qureg *qureg = losu_api_obj_resource_get (vm, arg1);
  int c = losu_api_obj_float_get (vm, arg2);
  qreal theta = losu_api_obj_float_get (vm, arg3);
  printf ("<applyRotateX>\n");
  reportQureg (*qureg);
  applyRotateX (*qureg, c, theta);
  reportQureg (*qureg);
  printf ("</applyRotateX>\n");
  return losu_api_obj_context_return_float (vm, ctx, 1);
}

losu_base_ffi_declare (applyToffoli, vm, ctx, recall)
{
  (void)vm;
  (void)ctx;
  (void)recall;
  struct losu_object_t arg1 = losu_api_obj_context_args (vm, ctx, 1);
  struct losu_object_t arg2 = losu_api_obj_context_args (vm, ctx, 2);
  struct losu_object_t arg3 = losu_api_obj_context_args (vm, ctx, 3);
  struct losu_object_t arg4 = losu_api_obj_context_args (vm, ctx, 4);

  assert (losu_api_obj_type (vm, arg1) == losu_types_enum_t_resource
          && losu_api_obj_type (vm, arg2) == losu_types_enum_t_float
          && losu_api_obj_type (vm, arg3) == losu_types_enum_t_float
          && losu_api_obj_type (vm, arg4) == losu_types_enum_t_float);
  Qureg *qureg = losu_api_obj_resource_get (vm, arg1);
  int c1 = losu_api_obj_float_get (vm, arg2);
  int c2 = losu_api_obj_float_get (vm, arg3);
  int t = losu_api_obj_float_get (vm, arg4);
  int ctrl[2] = { c1, c2 };
  printf ("<applyToffoli>\n");
  reportQureg (*qureg);
  applyMultiControlledPauliX (*qureg, ctrl, 2, t);
  reportQureg (*qureg);
  printf ("</applyToffoli>\n");
  return losu_api_obj_context_return_float (vm, ctx, 1);
}

// Measure
losu_base_ffi_declare (applyQubitMeasurement, vm, ctx, recall)
{
  (void)vm;
  (void)ctx;
  (void)recall;
  struct losu_object_t arg1 = losu_api_obj_context_args (vm, ctx, 1);
  struct losu_object_t arg2 = losu_api_obj_context_args (vm, ctx, 2);
  assert (losu_api_obj_type (vm, arg1) == losu_types_enum_t_resource
          && losu_api_obj_type (vm, arg2) == losu_types_enum_t_float);
  Qureg *qureg = losu_api_obj_resource_get (vm, arg1);
  int i = losu_api_obj_float_get (vm, arg2);
  printf ("<Measure %d>\n", i);
  reportQureg (*qureg);
  int m = applyQubitMeasurement (*qureg, i);
  reportQureg (*qureg);
  printf ("</Measure %d = %d>\n", i, m);

  return losu_api_obj_context_return_float (vm, ctx, m);
}

// io
losu_base_ffi_declare (println, vm, ctx, recall)
{
  (void)vm;
  (void)ctx;
  (void)recall;
  uint16_t argc = losu_api_obj_context_narg (vm, ctx);
  for (uint16_t i = 0; i < argc; i++)
    {
      struct losu_object_t s = losu_api_obj_context_args (vm, ctx, i + 1);
      losu_base_io_printf ("%s", losu_api_obj_string_to (vm, &s));
      if (i + 1 < argc)
        losu_base_io_printf (" ");
    }
  losu_base_io_printf ("\n");
  return losu_api_obj_context_return_float (vm, ctx, 1);
}
losu_base_ffi_declare (eprintln, vm, ctx, recall)
{
  (void)vm;
  (void)ctx;
  (void)recall;
  uint16_t argc = losu_api_obj_context_narg (vm, ctx);
  for (uint16_t i = 0; i < argc; i++)
    {
      struct losu_object_t s = losu_api_obj_context_args (vm, ctx, i + 1);
      losu_base_io_errprintf ("%s", losu_api_obj_string_to (vm, &s));
      if (i + 1 < argc)
        losu_base_io_errprintf (" ");
    }
  losu_base_io_errprintf ("\n");
  return losu_api_obj_context_return_float (vm, ctx, 1);
}

losu_base_ffi_declare (warnprintln, vm, ctx, recall)
{
  (void)vm;
  (void)ctx;
  (void)recall;
  uint16_t argc = losu_api_obj_context_narg (vm, ctx);
  for (uint16_t i = 0; i < argc; i++)
    {
      struct losu_object_t s = losu_api_obj_context_args (vm, ctx, i + 1);
      losu_base_io_warnprintf ("%s", losu_api_obj_string_to (vm, &s));
      if (i + 1 < argc)
        losu_base_io_warnprintf (" ");
    }
  losu_base_io_warnprintf ("\n");
  return losu_api_obj_context_return_float (vm, ctx, 1);
}
#define M_PI 3.14159265358979323846

losu_base_ffi_declare (PI, vm, ctx, recall)
{
  (void)vm;
  (void)ctx;
  (void)recall;
  return losu_api_obj_context_return_float (vm, ctx, M_PI);
}