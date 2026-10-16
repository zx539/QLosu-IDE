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

#include "losu/src/syntax/backend/errno.h"

#define NO_JUMP -1

#define losu_vm_ins_pretext(name, mode, stack) {mode, stack}
#define V 0

struct _inline_opstack {
  int8_t mode;
  int8_t dlt;
} static const _inline_opstack[] = losu_vm_ins_text;

#undef V
#undef losu_vm_ins_pretext

losu_stddef_extern int64_t
losu_syntax_backend_codegen(losu_syntax_ast_t losu_stddef_notnull ast,
                            losu_vm_ins_OP_t op, int64_t arg1, int64_t arg2) {
  assert(ast != NULL);

  losu_stddef_vmins_t newins = 0;
  losu_syntax_t losu_stddef_notnull syn = ast->syn;
  losu_obj_func_t_script_t losu_stddef_notnull spt = ast->script;

#if LOSU_CONFIG_SYNTAX_BACKEND_OPTIM != 0
  losu_stddef_bool_t optd = 0;
  int64_t tmp =
      losu_syntax_backend_optim_handle_ptr(ast, op, arg1, arg2, &optd);
  if (optd)
    return tmp;
#endif

  int32_t dlt = _inline_opstack[op].dlt;
  switch (op) {
  case INS_POP: {
    if (arg1 == 0)
      return NO_JUMP;
    dlt = arg1;
    break;
  }
  case INS_PUSHNIL: {
    if (arg1 == 0)
      return NO_JUMP;
    dlt = arg1;
    break;
  }
  case INS_PUSHFUNCTION: {
    dlt = 1 - arg1;
    break;
  }
  case INS_SETARRAY: {
    if (arg1 == 0)
      return NO_JUMP;
    dlt = -arg1;
    break;
  }
  case INS_SETMAP: {
    if (arg1 == 0)
      return NO_JUMP;
    dlt = -2 * arg1;
    break;
  }
  }
  losu_syntax_backend_stack_adjust(ast, dlt);
  switch (_inline_opstack[op].mode) {
  case INS_MODE_OP: {
    newins = losu_vm_ins_cgNewOP(op);
    break;
  }
  case INS_MODE_IU: {
    newins = losu_vm_ins_cgNewU(op, arg1);
    break;
  }
  case INS_MODE_IS: {
    newins = losu_vm_ins_cgNewS(op, arg1);
    break;
  }
  case INS_MODE_BA: {
    newins = losu_vm_ins_cgNewBA(op, arg1, arg2);
    break;
  }
  }
  losu_vm_mem_vector_grow(syn->vm, spt->lineinfo, spt->nlineinfo, 1, uint32_t,
                          UINT32_MAX, ERRNO_OVERFLOW_LINE);
  spt->lineinfo[spt->nlineinfo++] = syn->linenumber;
  losu_vm_mem_vector_grow(syn->vm, spt->code, ast->pc, 1, losu_stddef_vmins_t,
                          UINT32_MAX, ERRNO_OVERFLOW_INS);
  spt->code[ast->pc] = newins;
  return ast->pc++;
}
