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

losu_stddef_extern losu_obj_resource_t losu_stddef_notnull
losu_object_resource_new(
    losu_vm_t losu_stddef_notnull vm, const char *losu_stddef_notnull meta,
    void *losu_stddef_notnull res_t,
    losu_obj_resource_t_gc_hook_t losu_stddef_nullable mark_t,
    losu_obj_resource_t_gc_hook_t losu_stddef_nullable sweep_t) {
  assert(vm != NULL);

  losu_obj_resource_t r = losu_vm_mem_new(vm, struct losu_obj_resource_t);
  assert(r != NULL);

  vm->gcBlocks += sizeof(struct losu_obj_resource_t);
  r->res_t = res_t;
  r->mark_t = mark_t;
  r->sweep_t = sweep_t;
  r->marked = 0;
  r->meta = losu_object_string_new(vm, meta);
  /* insert to resource pool */
  r->next = vm->respool;
  vm->respool = r;
  return r;
}

losu_stddef_extern void
losu_object_resource_close(losu_vm_t losu_stddef_notnull vm,
                           losu_obj_resource_t losu_stddef_notnull resource) {
  assert(resource != NULL);
  if (resource->sweep_t) {
    resource->sweep_t(vm, resource->res_t);
    resource->mark_t = resource->sweep_t = NULL;
  }
}

losu_stddef_extern void
losu_object_resource_free(losu_vm_t losu_stddef_notnull vm,
                          losu_obj_resource_t losu_stddef_notnull r) {
  assert(vm != NULL && r != NULL);
  losu_object_resource_close(vm, r);
  vm->gcBlocks -= sizeof(struct losu_obj_resource_t);
  losu_vm_mem_free(vm, r);
}

losu_stddef_extern void
losu_object_resource_defaulthook_mark(void *losu_stddef_notnull gc,
                                      void *losu_stddef_notnull p) {
  assert(gc != NULL && p != NULL);
  losu_vm_gc_mark_obj(gc, p);
}

losu_stddef_extern void
losu_object_resource_gc_defaulthook_sweep(void *losu_stddef_notnull vm,
                                          void *losu_stddef_notnull p) {
  assert(vm != NULL && p != NULL);
  losu_vm_mem_free(vm, p);
}