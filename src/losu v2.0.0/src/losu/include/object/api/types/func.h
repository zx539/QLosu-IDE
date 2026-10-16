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

#ifndef LOSU_INC_OBJECT_API_TYPES_FUNC_H
#define LOSU_INC_OBJECT_API_TYPES_FUNC_H

#define sizeof_losu_obj_func_t(nclosure)                                       \
  (sizeof(struct losu_obj_func_t) + sizeof(struct losu_object_t) * (nclosure))

#define sizeof_losu_obj_func_t_script_t(spt)                                   \
  (sizeof(struct losu_obj_func_t_script_t) +                                   \
   spt->nlcfloat * sizeof(losu_stddef_float_t) +                               \
   spt->nlcstr * sizeof(losu_obj_string_t) +                                   \
   spt->nlcscript * sizeof(losu_obj_func_t_script_t) +                         \
   spt->nlineinfo * sizeof(int32_t) +                                          \
   spt->ncode * sizeof(losu_stddef_vmins_t))

typedef struct losu_obj_func_t_script_t *losu_obj_func_t_script_t;
typedef struct losu_obj_func_t_script_t_locvar_t
    *losu_obj_func_t_script_t_locvar_t;

struct losu_obj_func_t {
  losu_obj_func_t losu_stddef_nullable next;
  losu_obj_func_t losu_stddef_nullable mark;
  union losu_obj_func_t_func {
    losu_vm_t_capi_t losu_stddef_nullable capi;
    losu_obj_func_t_script_t losu_stddef_nullable script;
  } func;
  losu_object_t losu_stddef_nullable closure;
  uint8_t nclosure;
  losu_stddef_bool_t isC;
};

struct losu_obj_func_t_script_t {

  losu_obj_func_t_script_t losu_stddef_nullable next;
  losu_obj_string_t losu_stddef_notnull name;
  losu_obj_string_t losu_stddef_nullable fspath;
  /* local segment */
  losu_obj_float_t *losu_stddef_nullable lcfloat;
  losu_obj_string_t losu_stddef_notnull *losu_stddef_nullable lcstr;
  losu_obj_func_t_script_t losu_stddef_notnull *losu_stddef_nullable lcscript;
  losu_stddef_vmins_t *losu_stddef_nullable code;
  uint32_t *losu_stddef_nullable lineinfo;

  losu_obj_func_t_script_t_locvar_t losu_stddef_nullable localvar;

  uint32_t nlcfloat, nlcstr, nlcscript, ncode, nlineinfo;

  uint32_t nlocalvar;
  /* args & stack info segment */
  uint16_t nargs, maxstack;
  losu_stddef_bool_t marked;
};

struct losu_obj_func_t_script_t_locvar_t {
  losu_obj_string_t losu_stddef_nullable name;
  uint32_t startpc;
  uint32_t endpc;
};

/**
 * @brief create a new function
 * @param vm
 * @param upvnum
 * @return losu_obj_func_t
 * @retval new func handle
 */
losu_stddef_extern losu_obj_func_t losu_stddef_notnull
losu_object_func_new(losu_vm_t losu_stddef_notnull vm, uint8_t upvnum);

/**
 * @brief free a function
 * @param vm
 * @param f
 * @return void
 */
losu_stddef_extern void
losu_object_func_free(losu_vm_t losu_stddef_notnull vm,
                      losu_obj_func_t losu_stddef_notnull f);

/**
 * @brief create a new script
 * @param vm
 * @return losu_obj_func_t_script_t
 * @retval new func handle
 * @note script don't need set gc
 */
losu_stddef_extern losu_obj_func_t_script_t losu_stddef_notnull
losu_object_func_script_new(losu_vm_t losu_stddef_notnull vm,
                            losu_obj_string_t losu_stddef_notnull name);

/**
 * @brief free a script
 * @param vm
 * @param script
 * @return void
 */
losu_stddef_extern void losu_object_func_script_free(
    losu_vm_t losu_stddef_notnull vm,
    losu_obj_func_t_script_t losu_stddef_notnull script);

#endif
