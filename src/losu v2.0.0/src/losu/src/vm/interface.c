/**
 * Homepage: https://losu.tech
 * Losu - An open-source programming language project from China
 *
 * Created by chen-chaochen during his time at NEUQ,
 * and maintained by the university student innovation team "Neuq-Losu Group".
 *
 * Losu v2 is dual-licensed under the terms of:
 *   1. GNU Affero General Public License v3.0 (AGPL-3.0)
 *   2. Mulan Permissive Software License v2 (Mulan PSL v2)
 *
 * By default, this software is licensed under AGPL-3.0. The source code,
 * including all modifications, must be made available under the same license
 * when distributed.
 *
 * For commercial use without AGPL-3.0's copyleft obligations,
 * you may opt to use Mulan PSL v2 which has more permissive terms.
 * To obtain commercial licensing under Mulan PSL v2, please contact us
 * losulang@foxmail.com
 *
 * For AGPL-3.0 terms, see: https://www.gnu.org/licenses/agpl-3.0.html
 * For Mulan PSL v2 terms, see: http://license.coscl.org.cn/MulanPSL2
 *
 * SPDX-License-Identifier: AGPL-3.0-or-later OR MulanPSL-2.0
 */

/**
 * Copyright (C) 2019 陈朝臣, 2022 汤洛小组（东北大学）
 *     This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *     This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *     You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://gnu.ac.cn/licenses/>.
 */

#include "losu/include/losu.h"

#include "losu/src/vm/errno.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

// losu_stddef_extern losu_vm_t losu_stddef_nullable losu_vm_init(size_t memlim)
// {
//   jmp_buf jb;

//   losu_vm_t vm = losu_vm_mem_new(NULL, struct losu_vm_t);
//   if (NULL == vm)
//     return vm;
//   memset(vm, 0, sizeof(struct losu_vm_t));

//   vm->errjmp = &jb;
//   vm->gcMaxHook = vm->gcBlocks = sizeof(struct losu_vm_t);

//   if (setjmp(jb) != 0) {
//     losu_vm_deinit(vm);
//     return NULL;
//   }
//   losu_object_strpool_init(vm);
//   vm->global = losu_object_unit_new(vm, 0);

//   /* init global */
//   *losu_object_unit_setmeta(vm, vm->global,
//                             losu_object_meta_new(vm, "module_cache")) =
//       (struct losu_object_t){
//           .type = losu_object_types_enum_t_unit,
//           .value._unit = losu_object_unit_new(vm, 0),
//       };
//   *losu_object_unit_setmeta(vm, vm->global,
//                             losu_object_meta_new(vm, "module_cache_tag")) =
//       (struct losu_object_t){
//           .type = losu_object_types_enum_t_unit,
//           .value._unit = losu_object_unit_new(vm, 0),
//       };

//   vm->errjmp = NULL;

//   vm->gcLimit = memlim;
//   return vm;
// }

losu_stddef_extern void losu_vm_deinit(losu_vm_t losu_stddef_notnull vm) {
  assert(vm != NULL);
  losu_vm_gc_sweep(vm, 1);
  losu_object_strpool_free(vm);
  losu_vm_thread_fiber_sweep(vm);
  losu_vm_mem_free(NULL, vm);
}

losu_stddef_extern void losu_vm_error(losu_vm_t losu_stddef_notnull vm,
                                      const char *losu_stddef_notnull msg,
                                      ...) {
  assert(vm != NULL && msg != NULL);
  char tmp[LOSU_CONFIG_VM_IO_SIZE] = {0};
  va_list ap;
  va_start(ap, msg);
  vsnprintf(tmp, sizeof(tmp), msg, ap);
  va_end(ap);
  losu_base_io_errprintf("%s%s\n", ERRNO_VM_INTERFACE_ERROR, tmp);
  losu_vm_stack_trace(vm);
  losu_vm_signal_throw(vm, losu_vm_sig_enum_t_err);
}

losu_stddef_extern void losu_vm_warning(losu_vm_t losu_stddef_notnull vm,
                                        const char *losu_stddef_notnull msg,
                                        ...) {
  assert(vm != NULL && msg != NULL);
  char tmp[LOSU_CONFIG_VM_IO_SIZE] = {0};
  va_list ap;
  va_start(ap, msg);
  vsnprintf(tmp, sizeof(tmp), msg, ap);
  va_end(ap);
  losu_base_io_warnprintf("%s\n", tmp);
  losu_vm_stack_trace(vm);
}
