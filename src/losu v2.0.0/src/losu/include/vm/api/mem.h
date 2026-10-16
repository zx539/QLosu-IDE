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

#ifndef LOSU_INC_VM_API_MEM_H
#define LOSU_INC_VM_API_MEM_H

/**
 * @brief realloc memory of vm namespace
 * @param vm
 * @param ptr
 * @param size
 * @return void*
 * @retval pointer of new memory
 * @pre size > 0
 */
losu_stddef_extern void *losu_stddef_notnull
losu_vm_mem_allocer(losu_vm_t losu_stddef_nullable vm,
                    void *losu_stddef_nullable ptr, size_t size);

/**
 * @brief free memory of vm namespace
 * @param vm
 * @param ptr
 * @return
 */
losu_stddef_extern void losu_vm_mem_free(losu_vm_t losu_stddef_nullable vm,
                                         void *losu_stddef_notnull ptr);

/**
 * @brief grow items block with failed error message
 * @param vm
 * @param block
 * @param i_num
 * @param inc
 * @param size
 * @param limit
 * @param errmsg
 * @return void*
 * @retval pointer of new block
 */
losu_stddef_extern void *losu_stddef_notnull losu_vm_mem_grow(
    losu_vm_t losu_stddef_notnull vm, void *losu_stddef_nullable block,
    size_t i_num, int32_t inc, size_t size, size_t limit,
    const char *losu_stddef_notnull errmsg);

/* define marcos */
#define losu_vm_mem_malloc(vm, size) losu_vm_mem_allocer((vm), 0, (size))

#define losu_vm_mem_realloc(vm, ptr, size)                                     \
  losu_vm_mem_allocer((vm), (ptr), (size))

#define losu_vm_mem_free(vm, ptr) losu_vm_mem_allocer((vm), (ptr), 0)

#define losu_vm_mem_new(vm, T) (T *)losu_vm_mem_malloc((vm), sizeof(T))

#define losu_vm_mem_vector_new(vm, n, T)                                       \
  (T *)losu_vm_mem_malloc((vm), (n) * sizeof(T))

#define losu_vm_mem_vector_grow(vm, vec, nitem, inc, T, lim, errmsg)           \
  ((vec) = (T *)losu_vm_mem_grow((vm), (vec), (nitem), (inc), sizeof(T),       \
                                 (lim), (errmsg)))

#define losu_vm_mem_vector_realloc(vm, vec, size, T)                           \
  ((vec) = (T *)losu_vm_mem_realloc((vm), (vec), (size) * sizeof(T)))

#endif
