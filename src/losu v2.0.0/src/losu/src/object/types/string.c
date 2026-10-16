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

static inline losu_stddef_hash_t _inline_string_hash(const char *str,
                                                     size_t len) {
  size_t h = len;
  size_t step = (len >> 5) | 1;
  for (; len >= step; len -= step)
    h = h ^ ((h << 5) + (h >> 2) + (losu_stddef_bool_t) * (str++));
  return (losu_stddef_hash_t)h;
};

losu_stddef_extern losu_obj_string_t losu_stddef_notnull
losu_object_string_new_len(losu_vm_t losu_stddef_notnull vm,
                           const char *losu_stddef_nullable str, size_t len) {
  assert(vm != NULL);
  losu_stddef_hash_t h = _inline_string_hash(str, len);
  losu_stddef_hash_t h1 = h & (vm->strpool_size - 1);
  losu_obj_string_t ts = NULL;
  /* find old */
  for (ts = vm->strpool_strobj[h1]; ts; ts = ts->next)
    if (ts->hash == h && ts->len == len && (memcmp(str, ts->data, len) == 0))
      return ts;
  /* new */
  ts = losu_vm_mem_malloc(vm, sizeof_losu_obj_string_t(len));

  vm->gcBlocks += sizeof_losu_obj_string_t(len);
  /* set */
  *ts = (struct losu_obj_string_t){
      .marked = 0,
      .next = NULL,
      .len = len,
      .hash = h,
  };
  memcpy(ts->data, str, len);
  ts->data[len] = '\0';
  /* stringpool */
  ts->next = vm->strpool_strobj[h1];
  vm->strpool_strobj[h1] = ts;
  vm->strpool_nsize++;
  if (vm->strpool_nsize > vm->strpool_size && vm->strpool_size < UINT32_MAX / 2)
    losu_object_strpool_resize(vm, vm->strpool_size * 2);

  return ts;
}

losu_stddef_extern losu_obj_string_t losu_stddef_notnull losu_object_string_new(
    losu_vm_t losu_stddef_notnull vm, const char *losu_stddef_notnull str) {
  assert(vm != NULL);
  return losu_object_string_new_len(vm, str, strlen((str)));
}

losu_stddef_extern losu_obj_string_t losu_stddef_notnull
losu_object_string_new_const(losu_vm_t losu_stddef_notnull vm,
                             const char *losu_stddef_notnull str) {
  assert(vm != NULL);

  losu_obj_string_t ts = losu_object_string_new(vm, str);
  if (ts->marked == 0)
    ts->marked = losu_obj_string_t_const_mark_tag;
  return ts;
}

losu_stddef_extern losu_obj_string_t losu_stddef_notnull
losu_object_string_new_mark(losu_vm_t losu_stddef_notnull vm,
                            const char *losu_stddef_nullable str,
                            int16_t mark) {
  assert(vm != NULL);
  losu_obj_string_t ts = losu_object_string_new(vm, str);
  ts->marked = mark;
  return ts;
}

losu_stddef_extern void
losu_object_strpool_init(losu_vm_t losu_stddef_notnull vm) {
  assert(vm != NULL);
  vm->strpool_strobj = losu_vm_mem_vector_new(vm, 1, losu_obj_string_t);
  vm->gcBlocks += sizeof(losu_obj_string_t);
  vm->strpool_size = 1;
  vm->strpool_nsize = 0;
  vm->strpool_strobj[0] = NULL;
}

losu_stddef_extern void
losu_object_strpool_resize(losu_vm_t losu_stddef_notnull vm,
                           losu_stddef_hash_t size) {
  assert(vm != NULL);

  losu_obj_string_t *newstrobj =
      losu_vm_mem_vector_new(vm, size, losu_obj_string_t);

  memset(newstrobj, 0, size * sizeof(losu_obj_string_t));
  for (losu_stddef_hash_t i = 0; i < vm->strpool_size; i++) {
    losu_obj_string_t p = vm->strpool_strobj[i];
    while (p) {
      losu_obj_string_t next = p->next;
      losu_stddef_hash_t h = p->hash;
      losu_stddef_hash_t h1 = h & (size - 1);
      p->next = newstrobj[h1];
      newstrobj[h1] = p;
      p = next;
    }
  }
  losu_vm_mem_free(vm, vm->strpool_strobj);
  vm->gcBlocks +=
      ((int64_t)size - (int64_t)vm->strpool_size) * sizeof(losu_obj_string_t);
  vm->strpool_size = size;
  vm->strpool_strobj = newstrobj;
}

losu_stddef_extern void
losu_object_strpool_free(losu_vm_t losu_stddef_notnull vm) {
  assert(vm != NULL);

  vm->gcBlocks -= vm->strpool_size * sizeof(losu_obj_string_t);
  losu_vm_mem_free(vm, vm->strpool_strobj); /* free & gc sub */
}
