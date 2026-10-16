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

static inline uint32_t _inline_pack_size(uint32_t n) {
  if (n <= 4)
    return 4;
  if (n > (UINT32_MAX >> 1))
    return UINT32_MAX;
  n--;
  n |= n >> 1;
  n |= n >> 2;
  n |= n >> 4;
  n |= n >> 8;
  n |= n >> 16;
  n++;
  return n;
}

losu_stddef_extern losu_obj_unit_t losu_stddef_notnull
losu_object_unit_new(losu_vm_t losu_stddef_notnull vm, uint32_t size) {
  assert(vm != NULL);
  /* unit */
  losu_obj_unit_t unit = losu_vm_mem_new(vm, struct losu_obj_unit_t);
  vm->gcBlocks += sizeof_losu_obj_unit_t(0);
  /* insert */
  unit->next = vm->unitpool;
  vm->unitpool = unit;
  /* set value */
  unit->mark = unit;
  unit->size = 0;
  unit->node = NULL;
  losu_object_unit_setvector(vm, unit, _inline_pack_size(size));
  /* return */
  return unit;
}

losu_stddef_extern void
losu_object_unit_free(losu_vm_t losu_stddef_notnull vm,
                      losu_obj_unit_t losu_stddef_notnull unit) {
  assert(vm != NULL && unit != NULL);

  vm->gcBlocks -= sizeof_losu_obj_unit_t(unit->size);
  losu_vm_mem_free(vm, unit->node);
  losu_vm_mem_free(vm, unit);
}

losu_stddef_extern losu_obj_unit_t_node_t losu_stddef_nullable
losu_object_unit_iternext(losu_obj_unit_t losu_stddef_notnull t,
                          losu_object_t losu_stddef_notnull key) {
  uint32_t i = 0;
  if (key->type != losu_object_types_enum_t_nil) {
    losu_object_const_t v = losu_object_unit_get(t, key);
    if (v == &losu_object_nil_const)
      return NULL;
    i = (uint32_t)(((char *)v - (char *)(&t->node[0].value)) /
                       sizeof(struct losu_obj_unit_t_node_t) +
                   1);
  }
  for (; i < t->size; i++) {
    losu_obj_unit_t_node_t n = &t->node[i];
    if (n->key.type != losu_object_types_enum_t_nil)
      return n;
  }
  return NULL;
}

losu_stddef_extern uint32_t
losu_object_unit_size(losu_obj_unit_t losu_stddef_notnull unit) {
  assert(unit != NULL);

  losu_obj_unit_t_node_t v = unit->node;
  uint32_t s = unit->size;
  uint32_t use = 0;
  for (uint32_t i = 0; i < s; i++)
    if (v[i].value.type != losu_object_types_enum_t_nil)
      use++;
  return use;
}

losu_stddef_extern losu_obj_unit_t_node_t losu_stddef_nullable
losu_object_unit_node_getmainkey(losu_obj_unit_t losu_stddef_notnull unit,
                                 losu_object_const_t losu_stddef_notnull key) {
  assert(unit != NULL && key != NULL);

  losu_stddef_hash_t h = 0;
  switch (losu_oType(key)) {
  case losu_object_types_enum_t_float:
    h = (losu_stddef_hash_t)(losu_oValue(key, float));
    break;
  case losu_object_types_enum_t_string:
    h = (losu_stddef_hash_t)((size_t)losu_oValue(key, string));
    break;
  case losu_object_types_enum_t_unit:
    h = (losu_stddef_hash_t)((size_t)losu_oValue(key, unit));
    break;
  case losu_object_types_enum_t_func:
    h = (losu_stddef_hash_t)((size_t)losu_oValue(key, func));
    break;
  case losu_object_types_enum_t_resource:
    h = (losu_stddef_hash_t)((size_t)losu_oValue(key, resource));
    break;
  case losu_object_types_enum_t_thread:
    h = (losu_stddef_hash_t)((size_t)losu_oValue(key, thread));
    break;
  case losu_object_types_enum_t_meta:
    h = (losu_stddef_hash_t)((size_t)losu_oValue(key, meta));
    break;
  case losu_object_types_enum_t_context:
    h = (losu_stddef_hash_t)((size_t)losu_oValue(key, context));
    break;
  default:
    return NULL;
  }
  return &unit->node[(h) & (unit->size - 1)];
}

losu_stddef_extern void
losu_object_unit_setvector(losu_vm_t losu_stddef_notnull vm,
                           losu_obj_unit_t losu_stddef_notnull unit,
                           uint32_t size) {
  assert(vm != NULL && unit != NULL);

  unit->node = losu_vm_mem_vector_new(vm, size, struct losu_obj_unit_t_node_t);
  for (uint32_t i = 0; i < size; i++) {
    losu_oType(&unit->node[i].key) = losu_oType(&unit->node[i].value) =
        losu_object_types_enum_t_nil;
    unit->node[i].next = NULL;
  }
  vm->gcBlocks += (int64_t)sizeof_losu_obj_unit_t(size) -
                  (int64_t)sizeof_losu_obj_unit_t(unit->size);
  unit->size = size;
  unit->free = &unit->node[size - 1];
}

losu_stddef_extern void
losu_object_unit_rehash(losu_vm_t losu_stddef_notnull vm,
                        losu_obj_unit_t losu_stddef_notnull unit) {
  assert(vm != NULL && unit != NULL);

  uint32_t osize = unit->size;
  losu_obj_unit_t_node_t onode = unit->node;
  uint32_t usize = losu_object_unit_size(unit);
  if (usize >= osize - (osize / 4))
    losu_object_unit_setvector(vm, unit, osize * 2);
  else if (usize <= osize / 4 && osize > 4)
    losu_object_unit_setvector(vm, unit, osize / 2);
  else
    losu_object_unit_setvector(vm, unit, osize);
  for (uint32_t i = 0; i < osize; i++)
    if (losu_oType(&onode[i].value) != losu_object_types_enum_t_nil)
      *losu_object_unit_set(vm, unit, &(onode + i)->key) = (onode + i)->value;

  losu_vm_mem_free(vm, onode);
}

losu_stddef_extern losu_object_t losu_stddef_nullable losu_object_unit_set(
    losu_vm_t losu_stddef_notnull vm, losu_obj_unit_t losu_stddef_notnull unit,
    losu_object_const_t losu_stddef_notnull key) {
  assert(vm != NULL && unit != NULL && key != NULL);

  if (losu_oType(key) == losu_object_types_enum_t_string)
    return losu_object_unit_setstr(vm, unit, losu_oValue(key, string));
  else if (losu_oType(key) == losu_object_types_enum_t_float)
    return losu_object_unit_setfloat(vm, unit, losu_oValue(key, float));

  return losu_object_unit_setany(vm, unit, key);
}

losu_stddef_extern losu_object_t losu_stddef_nullable losu_object_unit_setany(
    losu_vm_t losu_stddef_notnull vm, losu_obj_unit_t losu_stddef_notnull unit,
    losu_object_const_t losu_stddef_notnull key) {
  assert(vm != NULL && unit != NULL && key != NULL);

  losu_obj_unit_t_node_t mkey, n;
  n = mkey = losu_object_unit_node_getmainkey(unit, key);
  if (mkey == NULL)
    return NULL;
  do {
    if (losu_object_isequ(key, &n->key))
      return &n->value;
    n = n->next;
  } while (n);
  if (losu_oType(&mkey->key) != losu_object_types_enum_t_nil) {
    losu_obj_unit_t_node_t oth;
    n = unit->free;
    if (mkey > n &&
        (oth = losu_object_unit_node_getmainkey(unit, &mkey->key)) != mkey) {
      while (oth->next != mkey) {
        oth = oth->next;
        assert(oth != NULL);
        if (oth == NULL)
          return NULL;
      }
      oth->next = n;
      *n = *mkey;
      mkey->next = NULL;
    } else {
      n->next = mkey->next;
      mkey->next = n;
      mkey = n;
    }
  }
  mkey->key = *key;
  for (;;) {
    if (losu_oType(&unit->free->key) == losu_object_types_enum_t_nil)
      return &mkey->value;
    else if (unit->free == unit->node)
      break;
    else
      (unit->free)--;
  }
  losu_object_unit_rehash(vm, unit);
  return losu_object_unit_setany(vm, unit, key);
}

losu_stddef_extern losu_object_t losu_stddef_nullable losu_object_unit_setfloat(
    losu_vm_t losu_stddef_notnull vm, losu_obj_unit_t losu_stddef_notnull unit,
    losu_obj_float_t key) {
  assert(vm != NULL && unit != NULL);

  losu_obj_unit_t_node_t losu_stddef_notnull mkey;
  losu_obj_unit_t_node_t losu_stddef_notnull n;

  n = mkey = &unit->node[(losu_stddef_hash_t)(key) & (unit->size - 1)];
  do {
    if (losu_oType(&n->key) == losu_object_types_enum_t_float &&
        key == losu_oValue(&n->key, float))
      return &n->value;
    n = n->next;
  } while (n);
  if (losu_oType(&mkey->key) != losu_object_types_enum_t_nil) {
    losu_obj_unit_t_node_t oth;
    n = unit->free;
    if (mkey > n &&
        (oth = losu_object_unit_node_getmainkey(unit, &mkey->key)) != mkey) {
      while (oth->next != mkey) {
        oth = oth->next;
        if (oth == NULL)
          return NULL;
      }
      oth->next = n;
      *n = *mkey;
      mkey->next = NULL;
    } else {
      n->next = mkey->next;
      mkey->next = n;
      mkey = n;
    }
  }
  mkey->key = (struct losu_object_t){
      .type = losu_object_types_enum_t_float,
      .value._float = key,
  };
  for (;;) {
    if (losu_oType(&unit->free->key) == losu_object_types_enum_t_nil)
      return &mkey->value;
    else if (unit->free == unit->node)
      break;
    else
      (unit->free)--;
  }
  losu_object_unit_rehash(vm, unit);
  return losu_object_unit_setfloat(vm, unit, key);
}

losu_stddef_extern losu_object_t losu_stddef_nullable losu_object_unit_setstr(
    losu_vm_t losu_stddef_notnull vm, losu_obj_unit_t losu_stddef_notnull unit,
    losu_obj_string_t losu_stddef_notnull key) {
  assert(vm != NULL && unit != NULL && key != NULL);

  losu_obj_unit_t_node_t losu_stddef_notnull mkey;
  losu_obj_unit_t_node_t losu_stddef_notnull n;
  mkey = n = &unit->node[(losu_stddef_hash_t)((size_t)key) & (unit->size - 1)];
  assert(mkey != NULL && n != NULL);

  do {
    if (losu_oType(&n->key) == losu_object_types_enum_t_string &&
        key == losu_oValue(&n->key, string))
      return &n->value;
    n = n->next;
  } while (n);
  if (losu_oType(&mkey->key) != losu_object_types_enum_t_nil) {
    losu_obj_unit_t_node_t oth;
    n = unit->free;
    if (mkey > n &&
        (oth = losu_object_unit_node_getmainkey(unit, &mkey->key)) != mkey) {
      while (oth->next != mkey) {
        oth = oth->next;
        if (oth == NULL)
          return NULL;
      }
      oth->next = n;
      *n = *mkey;
      mkey->next = NULL;
    } else {
      n->next = mkey->next;
      mkey->next = n;
      mkey = n;
    }
  }
  mkey->key = (struct losu_object_t){
      .type = losu_object_types_enum_t_string,
      .value._string = key,
  };
  for (;;) {
    if (losu_oType(&unit->free->key) == losu_object_types_enum_t_nil)
      return &mkey->value;
    else if (unit->free == unit->node)
      break;
    else
      (unit->free)--;
  }
  losu_object_unit_rehash(vm, unit);
  return losu_object_unit_setstr(vm, unit, key);
}

losu_stddef_extern losu_object_t losu_stddef_nullable losu_object_unit_setmeta(
    losu_vm_t losu_stddef_notnull vm, losu_obj_unit_t losu_stddef_notnull unit,
    losu_obj_meta_t losu_stddef_notnull key) {
  assert(vm != NULL && unit != NULL && key != NULL);

  losu_obj_unit_t_node_t losu_stddef_notnull mkey;
  losu_obj_unit_t_node_t losu_stddef_notnull n;
  mkey = n = &unit->node[(losu_stddef_hash_t)((size_t)key) & (unit->size - 1)];
  assert(mkey != NULL && n != NULL);

  do {
    if (losu_oType(&n->key) == losu_object_types_enum_t_meta &&
        key == losu_oValue(&n->key, meta))
      return &n->value;
    n = n->next;
  } while (n);
  if (losu_oType(&mkey->key) != losu_object_types_enum_t_nil) {
    losu_obj_unit_t_node_t oth;
    n = unit->free;
    if (mkey > n &&
        (oth = losu_object_unit_node_getmainkey(unit, &mkey->key)) != mkey) {
      while (oth->next != mkey) {
        oth = oth->next;
        if (oth == NULL)
          return NULL;
      }
      oth->next = n;
      *n = *mkey;
      mkey->next = NULL;
    } else {
      n->next = mkey->next;
      mkey->next = n;
      mkey = n;
    }
  }
  mkey->key = (struct losu_object_t){
      .type = losu_object_types_enum_t_meta,
      .value._meta = key,
  };
  for (;;) {
    if (losu_oType(&unit->free->key) == losu_object_types_enum_t_nil)
      return &mkey->value;
    else if (unit->free == unit->node)
      break;
    else
      (unit->free)--;
  }
  losu_object_unit_rehash(vm, unit);
  return losu_object_unit_setmeta(vm, unit, key);
}

losu_stddef_extern losu_object_const_t losu_stddef_notnull
losu_object_unit_get(losu_obj_unit_t losu_stddef_notnull unit,
                     losu_object_const_t losu_stddef_notnull key) {
  assert(unit != NULL && key != NULL);

  if (losu_oType(key) == losu_object_types_enum_t_float)
    return losu_object_unit_getfloat(unit, losu_oValue(key, float));
  else if (losu_oType(key) == losu_object_types_enum_t_string)
    return losu_object_unit_getstr(unit, losu_oValue(key, string));
  else if (losu_oType(key) == losu_object_types_enum_t_meta)
    return losu_object_unit_getmeta(unit, losu_oValue(key, meta));

  return losu_object_unit_getany(unit, key);
}

losu_stddef_extern losu_object_const_t losu_stddef_notnull
losu_object_unit_getany(losu_obj_unit_t losu_stddef_notnull unit,
                        losu_object_const_t losu_stddef_notnull key) {
  assert(unit != NULL && key != NULL);
  losu_obj_unit_t_node_t n = losu_object_unit_node_getmainkey(unit, key);
  if (n == NULL)
    return (losu_object_const_t)&losu_object_nil_const;
  do {
    if (losu_object_isequ(key, &n->key))
      return &n->value;
    n = n->next;
  } while (n);
  return (losu_object_const_t)&losu_object_nil_const;
}

losu_stddef_extern losu_object_const_t losu_stddef_notnull
losu_object_unit_getfloat(losu_obj_unit_t losu_stddef_notnull unit,
                          losu_obj_float_t key) {
  assert(unit != NULL);

  losu_obj_unit_t_node_t n =
      &unit->node[(losu_stddef_hash_t)(key) & (unit->size - 1)];
  do {
    if (losu_oType(&n->key) == losu_object_types_enum_t_float &&
        key == losu_oValue(&n->key, float))
      return &n->value;
    n = n->next;
  } while (n);
  return (losu_object_const_t)&losu_object_nil_const;
}

losu_stddef_extern losu_object_const_t losu_stddef_notnull
losu_object_unit_getstr(losu_obj_unit_t losu_stddef_notnull unit,
                        losu_obj_string_t losu_stddef_notnull key) {
  assert(unit != NULL && key != NULL);

  losu_obj_unit_t_node_t n =
      &unit->node[(losu_stddef_hash_t)((size_t)key) & (unit->size - 1)];
  do {
    if (losu_oType(&n->key) == losu_object_types_enum_t_string &&
        key == losu_oValue(&n->key, string))
      return &n->value;
    n = n->next;
  } while (n);
  return (losu_object_const_t)&losu_object_nil_const;
}

losu_stddef_extern losu_object_const_t losu_stddef_notnull
losu_object_unit_getmeta(losu_obj_unit_t losu_stddef_notnull unit,
                         losu_obj_meta_t losu_stddef_notnull key) {
  assert(unit != NULL && key != NULL);

  losu_obj_unit_t_node_t n =
      &unit->node[(losu_stddef_hash_t)((size_t)key) & (unit->size - 1)];
  do {
    if (losu_oType(&n->key) == losu_object_types_enum_t_meta &&
        key == losu_oValue(&n->key, meta))
      return &n->value;
    n = n->next;
  } while (n);
  return (losu_object_const_t)&losu_object_nil_const;
}
