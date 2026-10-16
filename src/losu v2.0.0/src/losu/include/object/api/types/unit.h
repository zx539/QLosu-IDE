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

#ifndef LOSU_INC_OBJECT_API_TYPES_UNIT_H
#define LOSU_INC_OBJECT_API_TYPES_UNIT_H

#define sizeof_losu_obj_unit_t(items)                                          \
  (sizeof(struct losu_obj_unit_t) +                                            \
   (items) * sizeof(struct losu_obj_unit_t_node_t))

typedef struct losu_obj_unit_t_node_t *losu_obj_unit_t_node_t;

struct losu_obj_unit_t {
  losu_obj_unit_t losu_stddef_nullable mark;
  losu_obj_unit_t losu_stddef_nullable next;
  losu_obj_unit_t_node_t losu_stddef_nullable node;
  losu_obj_unit_t_node_t losu_stddef_nullable free;
  uint32_t size;
};

struct losu_obj_unit_t_node_t {
  struct losu_object_t key, value;
  losu_obj_unit_t_node_t losu_stddef_nullable next;
};

#pragma region "unit's new, free and helper-apis"

/**
 * @brief create a new unit
 * @param vm losu vm handle
 * @param size unit size (will be packed to ^2)
 * @retval new unit object handle
 */
losu_stddef_extern losu_obj_unit_t losu_stddef_notnull
losu_object_unit_new(losu_vm_t losu_stddef_notnull vm, uint32_t size);

/**
 * @brief free a unit
 * @param vm losu vm handle
 * @param unit unit object
 * @return void
 */
losu_stddef_extern void
losu_object_unit_free(losu_vm_t losu_stddef_notnull vm,
                      losu_obj_unit_t losu_stddef_notnull unit);

/**
 * @brief get next node for key
 * @param t unit object
 * @param key key
 * @retval next node handle
 */
losu_stddef_extern losu_obj_unit_t_node_t losu_stddef_nullable
losu_object_unit_iternext(losu_obj_unit_t losu_stddef_notnull t,
                          losu_object_t losu_stddef_notnull key);

/**
 * @brief get unit size
 * @param unit unit object
 * @retval unit size
 */
losu_stddef_extern uint32_t
losu_object_unit_size(losu_obj_unit_t losu_stddef_notnull unit);

/**
 * @brief get hash node of key
 * @param vm losu vm handle
 * @param unit unit object
 * @param key key
 * @retval hash node
 */
losu_stddef_extern losu_obj_unit_t_node_t losu_stddef_nullable
losu_object_unit_node_getmainkey(losu_obj_unit_t losu_stddef_notnull unit,
                                 losu_object_const_t losu_stddef_notnull key);

/**
 * @brief set vector size of unit
 * @param vm losu vm handle
 * @param unit unit object
 * @param size size
 * @return void
 */
losu_stddef_extern void
losu_object_unit_setvector(losu_vm_t losu_stddef_notnull vm,
                           losu_obj_unit_t losu_stddef_notnull unit,
                           uint32_t size);

/**
 * @brief rehash unit
 * @param vm losu vm handle
 * @param unit unit object
 * @return void
 */
losu_stddef_extern void
losu_object_unit_rehash(losu_vm_t losu_stddef_notnull vm,
                        losu_obj_unit_t losu_stddef_notnull unit);

#pragma endregion

#pragma region "unit set-apis"

/**
 * @brief create insert-node for key
 * @param vm vm
 * @param unit unit
 * @param key key
 * @retval NULL is Failed, else is insert-node
 */
losu_stddef_extern losu_object_t losu_stddef_nullable losu_object_unit_set(
    losu_vm_t losu_stddef_notnull vm, losu_obj_unit_t losu_stddef_notnull unit,
    losu_object_const_t losu_stddef_notnull key);

/**
 * @brief create insert-node for key
 * @param vm vm
 * @param unit unit
 * @param key key
 * @retval NULL is Failed, else is insert-node
 */
losu_stddef_extern losu_object_t losu_stddef_nullable losu_object_unit_setany(
    losu_vm_t losu_stddef_notnull vm, losu_obj_unit_t losu_stddef_notnull unit,
    losu_object_const_t losu_stddef_notnull key);

/**
 * @brief create insert-node for key
 * @param vm vm
 * @param unit unit
 * @param key key
 * @retval NULL is Failed, else is insert-node
 */
losu_stddef_extern losu_object_t losu_stddef_nullable losu_object_unit_setfloat(
    losu_vm_t losu_stddef_notnull vm, losu_obj_unit_t losu_stddef_notnull unit,
    losu_obj_float_t key);

/**
 * @brief create insert-node for key
 * @param vm vm
 * @param unit unit
 * @param key key
 * @retval NULL is Failed, else is insert-node
 */
losu_stddef_extern losu_object_t losu_stddef_nullable losu_object_unit_setstr(
    losu_vm_t losu_stddef_notnull vm, losu_obj_unit_t losu_stddef_notnull unit,
    losu_obj_string_t losu_stddef_notnull key);

/**
 * @brief create insert-node for key
 * @param vm vm
 * @param unit unit
 * @param key key
 * @retval NULL is Failed, else is insert-node
 */
losu_stddef_extern losu_object_t losu_stddef_nullable losu_object_unit_setmeta(
    losu_vm_t losu_stddef_notnull vm, losu_obj_unit_t losu_stddef_notnull unit,
    losu_obj_meta_t losu_stddef_notnull key);

#pragma endregion

#pragma region "unit get-apis"

/**
 * @brief get value-node for key
 * @param vm vm
 * @param unit unit
 * @param key key
 * @retval value-node
 */
losu_stddef_extern losu_object_const_t losu_stddef_notnull
losu_object_unit_get(losu_obj_unit_t losu_stddef_notnull unit,
                     losu_object_const_t losu_stddef_notnull key);

/**
 * @brief get value-node for key
 * @param vm vm
 * @param unit unit
 * @param key key
 * @retval value-node
 */
losu_stddef_extern losu_object_const_t losu_stddef_notnull
losu_object_unit_getany(losu_obj_unit_t losu_stddef_notnull unit,
                        losu_object_const_t losu_stddef_notnull key);

/**
 * @brief get value-node for key
 * @param vm vm
 * @param unit unit
 * @param key key
 * @retval value-node
 */
losu_stddef_extern losu_object_const_t losu_stddef_notnull
losu_object_unit_getfloat(losu_obj_unit_t losu_stddef_notnull unit,
                          losu_obj_float_t key);

/**
 * @brief get value-node for key
 * @param vm vm
 * @param unit unit
 * @param key key
 * @retval value-node
 */
losu_stddef_extern losu_object_const_t losu_stddef_notnull
losu_object_unit_getstr(losu_obj_unit_t losu_stddef_notnull unit,
                        losu_obj_string_t losu_stddef_notnull key);

/**
 * @brief get value-node for key
 * @param vm vm
 * @param unit unit
 * @param key key
 * @retval value-node
 */
losu_stddef_extern losu_object_const_t losu_stddef_notnull
losu_object_unit_getmeta(losu_obj_unit_t losu_stddef_notnull unit,
                         losu_obj_meta_t losu_stddef_notnull key);

#pragma endregion

#endif
