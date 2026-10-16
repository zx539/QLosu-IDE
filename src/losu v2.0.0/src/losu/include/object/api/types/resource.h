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

#ifndef LOSU_INC_OBJECT_API_TYPES_RESOURCE_H
#define LOSU_INC_OBJECT_API_TYPES_RESOURCE_H

typedef void (*losu_stddef_nullable losu_obj_resource_t_gc_hook_t)(
    void *losu_stddef_notnull handle, void *losu_stddef_notnull p);

struct losu_obj_resource_t {
  losu_obj_string_t losu_stddef_notnull meta;
  void *losu_stddef_notnull res_t;
  losu_obj_resource_t_gc_hook_t losu_stddef_nullable mark_t;
  losu_obj_resource_t_gc_hook_t losu_stddef_nullable sweep_t;
  losu_obj_unit_t losu_stddef_nullable aluhook;
  losu_obj_resource_t losu_stddef_nullable next;
  losu_stddef_bool_t marked;
};

/**
 * @brief create a new resource
 * @param vm
 * @param meta
 * @param res_t
 * @param mark_t
 * @param sweep_t
 * @return
 */
losu_stddef_extern losu_obj_resource_t losu_stddef_notnull
losu_object_resource_new(
    losu_vm_t losu_stddef_notnull vm, const char *losu_stddef_notnull meta,
    void *losu_stddef_notnull res_t,
    losu_obj_resource_t_gc_hook_t losu_stddef_nullable mark_t,
    losu_obj_resource_t_gc_hook_t losu_stddef_nullable sweep_t);

/**
 * @brief close resource
 * @param vm
 * @param resource
 * @return
 */
losu_stddef_extern void
losu_object_resource_close(losu_vm_t losu_stddef_notnull vm,
                           losu_obj_resource_t losu_stddef_notnull resource);

/**
 * @brief free a resource object
 * @param vm
 * @param r
 * @return
 */
losu_stddef_extern void
losu_object_resource_free(losu_vm_t losu_stddef_notnull vm,
                          losu_obj_resource_t losu_stddef_notnull r);

/**
 * @brief resource default hook mark
 * @param gc
 * @param p
 * @return
 */
losu_stddef_extern void
losu_object_resource_defaulthook_mark(void *losu_stddef_notnull gc,
                                      void *losu_stddef_notnull p);

/**
 * @brief resource default hook sweep
 * @param vm
 * @param p
 * @return
 */
losu_stddef_extern void
losu_object_resource_gc_defaulthook_sweep(void *losu_stddef_notnull vm,
                                          void *losu_stddef_notnull p);

#endif
