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

#define losu_oType(o) ((o)->type)
#define losu_oValue(o, name) ((o)->value._##name)

#include <string.h>

losu_stddef_extern losu_stddef_bool_t
losu_object_isequ(losu_object_const_t losu_stddef_notnull o1,
                  losu_object_const_t losu_stddef_notnull o2) {

  assert(o1 != NULL && o2 != NULL);

  if (o1->type != o2->type)
    return 0;
  switch (o1->type) {
  case losu_object_types_enum_t_nil:
    return 1;
  case losu_object_types_enum_t_float:
    return losu_oValue(o1, float) == losu_oValue(o2, float);
  case losu_object_types_enum_t_string:
    return losu_oValue(o1, string) == losu_oValue(o2, string);
  case losu_object_types_enum_t_unit:
    return losu_oValue(o1, unit) == losu_oValue(o2, unit);
  case losu_object_types_enum_t_func:
    return losu_oValue(o1, func) == losu_oValue(o2, func);
  case losu_object_types_enum_t_resource:
    return losu_oValue(o1, resource) == losu_oValue(o2, resource);
  case losu_object_types_enum_t_thread:
    return losu_oValue(o1, thread) == losu_oValue(o2, thread);
  case losu_object_types_enum_t_meta:
    return losu_oValue(o1, meta) == losu_oValue(o2, meta);
  case losu_object_types_enum_t_context:
    return losu_oValue(o1, context) == losu_oValue(o2, context);
  default:
    return 0;
  }

  return 0;
}

losu_stddef_extern losu_stddef_bool_t
losu_object_isless(losu_object_t losu_stddef_notnull o1,
                   losu_object_t losu_stddef_notnull o2) {
  assert(o1 != NULL && o2 != NULL);

  if (losu_oType(o1) != losu_oType(o2))
    return losu_oType(o1) < losu_oType(o2);
  if (losu_oType(o1) == losu_object_types_enum_t_float)
    return losu_oValue(o1, float) < losu_oValue(o2, float);
  else if (losu_oType(o1) == losu_object_types_enum_t_string) {
    losu_obj_string_t str1 = losu_oValue(o1, string);
    losu_obj_string_t str2 = losu_oValue(o2, string);
    int32_t cmp_result = memcmp(str1->data, str2->data,
                                str1->len < str2->len ? str1->len : str2->len);
    if (cmp_result == 0)
      return str1->len < str2->len;
    return cmp_result < 0;
  }

  return 0;
}
