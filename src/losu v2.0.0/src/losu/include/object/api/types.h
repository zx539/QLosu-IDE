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

#ifndef LOSU_INC_OBJECT_API_TYPES_H
#define LOSU_INC_OBJECT_API_TYPES_H

enum losu_object_types_enum_t {
  losu_object_types_enum_t_nil,
  losu_object_types_enum_t_float,
  losu_object_types_enum_t_string,
  losu_object_types_enum_t_unit,
  losu_object_types_enum_t_func,
  losu_object_types_enum_t_resource,
  losu_object_types_enum_t_thread,
  losu_object_types_enum_t_meta,
  losu_object_types_enum_t_context,
  losu_object_types_enum_t_invalid
};

/**
 * @brief convert c-string to float
 * @param str c-string
 * @param n float
 * @return 0 if success, 1 if not
 */
losu_stddef_extern losu_stddef_bool_t
losu_object_types_str2float(const char *losu_stddef_notnull str,
                            losu_stddef_float_t *losu_stddef_notnull n);

/**
 * @brief get object type with meta;
 * @param vm losu vm handle
 * @param obj object
 * @param meta meta handle
 * @retval type-name, *meta is type-meta
 */
losu_stddef_extern const char *losu_stddef_notnull losu_object_types_gettype(
    losu_vm_t losu_stddef_notnull vm, losu_object_t losu_stddef_notnull obj,
    losu_stddef_string_const_t losu_stddef_nullable *losu_stddef_nullable meta);

#endif
