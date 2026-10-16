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

#include <string.h>

#define losu_oType(o) ((o)->type)
#define losu_oValue(o, name) ((o)->value._##name)

losu_stddef_extern losu_obj_func_t losu_stddef_notnull
losu_object_func_new(losu_vm_t losu_stddef_notnull vm, uint8_t upvnum) {
  assert(vm != NULL);

  losu_obj_func_t f = losu_vm_mem_malloc(vm, sizeof(struct losu_obj_func_t));
  assert(f != NULL);
  vm->gcBlocks += sizeof(struct losu_obj_func_t);
  memset(f, 0, sizeof(struct losu_obj_func_t));

  if (upvnum) {
    f->nclosure = upvnum;
    size_t size = upvnum * sizeof(struct losu_object_t);
    f->closure = losu_vm_mem_malloc(vm, size);
    assert(f->closure != NULL);
    vm->gcBlocks += size;
    memset(f->closure, 0, size);
  }

  f->mark = f;

  /* insert to pool */
  f->next = vm->funcpool;
  vm->funcpool = f;

  return f;
}

losu_stddef_extern void
losu_object_func_free(losu_vm_t losu_stddef_notnull vm,
                      losu_obj_func_t losu_stddef_notnull f) {
  assert(vm != NULL && f != NULL);

  vm->gcBlocks -= sizeof_losu_obj_func_t(f->nclosure);

  losu_vm_mem_free(vm, f->closure);
  losu_vm_mem_free(vm, f);
}

losu_stddef_extern losu_obj_func_t_script_t losu_stddef_notnull
losu_object_func_script_new(losu_vm_t losu_stddef_notnull vm,
                            losu_obj_string_t losu_stddef_notnull name) {
  assert(vm != NULL);
  losu_obj_func_t_script_t script =
      losu_vm_mem_malloc(vm, sizeof(struct losu_obj_func_t_script_t));
  /* don't need set gc */
  script->name = name;
  memset(script, 0, sizeof(struct losu_obj_func_t_script_t));
  /* insert to pool */
  script->next = vm->scriptpool;
  vm->scriptpool = script;
  return script;
}

losu_stddef_extern void losu_object_func_script_free(
    losu_vm_t losu_stddef_notnull vm,
    losu_obj_func_t_script_t losu_stddef_notnull script) {
  assert(vm != NULL && script != NULL);

  vm->gcBlocks -= sizeof_losu_obj_func_t_script_t(script);

  losu_vm_mem_free(vm, script->localvar);

  losu_vm_mem_free(vm, script->lcfloat);
  losu_vm_mem_free(vm, script->lcstr);
  losu_vm_mem_free(vm, script->lcscript);
  losu_vm_mem_free(vm, script->code);
  losu_vm_mem_free(vm, script->lineinfo);
  losu_vm_mem_free(vm, script);
}
