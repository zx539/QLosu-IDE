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

#ifndef LOSU_INC_OBJECT_API_TYPES_THREAD_H
#define LOSU_INC_OBJECT_API_TYPES_THREAD_H

#define sizeof_losu_obj_thread_t(stack)                                        \
  (sizeof(struct losu_obj_thread_t) + (stack) * sizeof(struct losu_object_t))

typedef enum losu_obj_thread_t_status_enum_t losu_obj_thread_t_status_enum_t;

enum losu_obj_thread_t_status_enum_t {
  losu_obj_thread_t_status_enum_t_ready,
  losu_obj_thread_t_status_enum_t_running,
  losu_obj_thread_t_status_enum_t_suspended,
  losu_obj_thread_t_status_enum_t_dead,
  losu_obj_thread_t_status_enum_t_init,
  losu_obj_thread_t_status_enum_t_invalid
};

struct losu_obj_thread_t {
  losu_obj_thread_t losu_stddef_nullable next;
  losu_object_t losu_stddef_nullable top;
  uint32_t nstack;
  losu_obj_thread_t_status_enum_t status;
  losu_stddef_bool_t inited;
  losu_stddef_bool_t detach;
  losu_stddef_bool_t marked;
  struct losu_object_t stack[1];
};

/**
 * @brief create new thread
 * @param vm
 * @param size
 * @return losu_obj_thread_t
 * @retval thread handle
 */
losu_stddef_extern losu_obj_thread_t losu_stddef_notnull
losu_object_thread_new(losu_vm_t losu_stddef_notnull vm, uint32_t size);

/**
 * @brief free thread
 * @param vm
 * @param t
 * @return
 */
losu_stddef_extern void
losu_object_thread_free(losu_vm_t losu_stddef_notnull vm,
                        losu_obj_thread_t losu_stddef_notnull t);

#endif
