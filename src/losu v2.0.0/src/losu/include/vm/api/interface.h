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

#ifndef LOSU_INC_VM_API_INTERFACE_H
#define LOSU_INC_VM_API_INTERFACE_H

struct losu_vm_t {
  /* io */
  void *losu_stddef_notnull stdout, *losu_stddef_notnull stderr;
  /* string pool */
  losu_stddef_hash_t strpool_size, strpool_nsize;
  /* pool segment */
  losu_obj_string_t losu_stddef_notnull *losu_stddef_nullable strpool_strobj;
  losu_obj_unit_t losu_stddef_nullable unitpool;
  losu_obj_func_t losu_stddef_nullable funcpool;
  losu_obj_func_t_script_t losu_stddef_nullable scriptpool;
  losu_obj_resource_t losu_stddef_nullable respool;
  losu_obj_thread_t losu_stddef_nullable threadpool;
  losu_obj_context_t losu_stddef_nullable ctxpool;
  losu_vm_t_fiber_t losu_stddef_nullable fiberpool;

  losu_vm_t_fiber_t losu_stddef_nullable *losu_stddef_nullable fiber_ptr;

  /* global */
  losu_obj_unit_t losu_stddef_notnull global;
  /* char buff */
  char *losu_stddef_nullable buff;
  size_t nbuff;
  /* gc segment */
  size_t gcBlocks, gcLimit, gcMaxHook;

  /* ---- reg ---- */
  losu_obj_thread_t losu_stddef_nullable thread_ptr;
  losu_object_t losu_stddef_nullable stack;
  losu_object_t losu_stddef_nullable top;
  losu_object_t losu_stddef_nullable stackmax;
  void *losu_stddef_nullable errjmp;
};

/**
 * @brief create and init new vm
 * @param stdout stdout
 * @param stderr stderr
 * @param memlim mem when gc start work
 * @return losu_vm_t
 * @retval new vm handle
 */
losu_stddef_extern losu_vm_t losu_stddef_nullable
losu_vm_init(void *losu_stddef_notnull stdout, void *losu_stddef_notnull stderr,
             size_t memlim);

/**
 * @brief free a losu vm
 * @param vm
 * @return
 */
losu_stddef_extern void losu_vm_deinit(losu_vm_t losu_stddef_notnull vm);

/**
 * @brief throw error message, and signal.
 * @param vm
 * @param msg
 * @param
 * @return
 */
losu_stddef_extern void losu_vm_error(losu_vm_t losu_stddef_notnull vm,
                                      const char *losu_stddef_notnull msg, ...);

/**
 * @brief throw warning message
 * @param vm
 * @param msg
 * @param
 * @return
 */
losu_stddef_extern void losu_vm_warning(losu_vm_t losu_stddef_notnull vm,
                                        const char *losu_stddef_notnull msg,
                                        ...);

#endif
