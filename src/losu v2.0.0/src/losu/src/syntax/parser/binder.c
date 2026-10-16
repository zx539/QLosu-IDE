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

#include "losu/src/syntax/parser/binder.h"

#include "losu/src/syntax/parser/errno.h"

losu_stddef_extern uint32_t losu_syntax_parser_binder_floatvalue(
    losu_syntax_ast_t losu_stddef_notnull ast, losu_obj_float_t num) {
  assert(ast != NULL);

  losu_obj_func_t_script_t losu_stddef_notnull spt = ast->script;
  uint32_t c = spt->nlcfloat;
  for (uint32_t i = 0; i < c; i++)
    if (spt->lcfloat[i] == num)
      return i;

  losu_vm_mem_vector_grow(ast->vm, spt->lcfloat, spt->nlcfloat, 1,
                          losu_obj_float_t, losu_vm_ins_max_U,
                          ERRNO_BINDER_FLOAT);
  c = spt->nlcfloat++;
  spt->lcfloat[c] = num;
  return c;
}

losu_stddef_extern uint32_t losu_syntax_parser_binder_stringvalue(
    losu_syntax_ast_t losu_stddef_notnull ast,
    losu_obj_string_t losu_stddef_notnull str) {
  assert(ast != NULL && str != NULL);

  losu_obj_func_t_script_t losu_stddef_notnull spt = ast->script;
  uint32_t c = spt->nlcstr;
  for (uint32_t i = 0; i < c; i++)
    if (spt->lcstr[i] == str)
      return i;

  losu_vm_mem_vector_grow(ast->vm, spt->lcstr, spt->nlcstr, 1,
                          losu_obj_string_t, losu_vm_ins_max_U,
                          ERRNO_BINDER_STRING);
  c = spt->nlcstr++;
  spt->lcstr[c] = str;
  return c;
}

losu_stddef_extern losu_stddef_bool_t losu_syntax_parser_binder_getvar(
    losu_syntax_ast_t losu_stddef_notnull ast,
    losu_syntax_ast_t_exp_t losu_stddef_notnull exp) {
  assert(ast != NULL && exp != NULL);

  switch (exp->type) {
  case losu_syntax_ast_t_exp_t_type_local: {
    losu_syntax_backend_codegen(ast, INS_GETLOCAL, exp->value.cgidx, 0);
    break;
  }
  case losu_syntax_ast_t_exp_t_type_index: {
    losu_syntax_backend_codegen(ast, INS_GETUNIT, 0, 0);
    break;
  }
  case losu_syntax_ast_t_exp_t_type_opindex: {
    losu_syntax_backend_codegen(ast, INS_GETUNIT, 1, 0);
    break;
  }
  case losu_syntax_ast_t_exp_t_type_upvalue: {
    losu_syntax_backend_codegen(ast, INS_GETUPVALUE, exp->value.upvalue.cgidx,
                                0);
    break;
  }
  case losu_syntax_ast_t_exp_t_type_exper: {
    return 0;
  }
  }
  exp->type = losu_syntax_ast_t_exp_t_type_exper;
  exp->value._jump.t = exp->value._jump.f = NO_JUMP;
  return 1;
}

losu_stddef_extern void losu_syntax_parser_binder_setvar(
    losu_syntax_ast_t losu_stddef_notnull ast,
    losu_syntax_ast_t_exp_t losu_stddef_notnull exp) {
  assert(ast != NULL && exp != NULL);

  switch (exp->type) {
  case losu_syntax_ast_t_exp_t_type_local: {
    losu_syntax_backend_codegen(ast, INS_SETLOCAL, exp->value.cgidx, 0);
    break;
  }
  case losu_syntax_ast_t_exp_t_type_index: {
    losu_syntax_backend_codegen(ast, INS_SETUNIT, 0, 0);
    break;
  }
  case losu_syntax_ast_t_exp_t_type_opindex: {
    losu_syntax_backend_codegen(ast, INS_SETUNIT, 1, 0);
    break;
  }
  case losu_syntax_ast_t_exp_t_type_upvalue: {
    losu_syntax_backend_codegen(ast, INS_SETUPVALUE, exp->value.upvalue.cgidx,
                                0);
    break;
  }
  default: {
    losu_syntax_error(ast->syn, ast->syn->linenumber, ERRNO_BINDER_SETVAR);
  }
  }
}

losu_stddef_extern void losu_syntax_parser_binder_go_true(
    losu_syntax_ast_t losu_stddef_notnull ast,
    losu_syntax_ast_t_exp_t losu_stddef_notnull exp, losu_stddef_bool_t keep) {
  assert(ast != NULL && exp != NULL);
  losu_syntax_parser_binder_go(ast, exp, 1, keep ? INS_JMPONF : INS_JMPF);
}

losu_stddef_extern void losu_syntax_parser_binder_go_false(
    losu_syntax_ast_t losu_stddef_notnull ast,
    losu_syntax_ast_t_exp_t losu_stddef_notnull exp, losu_stddef_bool_t keep) {
  assert(ast != NULL && exp != NULL);
  losu_syntax_parser_binder_go(ast, exp, 0, keep ? INS_JMPONT : INS_JMPT);
}

losu_stddef_extern void
losu_syntax_parser_binder_go(losu_syntax_ast_t losu_stddef_notnull ast,
                             losu_syntax_ast_t_exp_t losu_stddef_notnull exp,
                             losu_stddef_bool_t inv, losu_vm_ins_OP_t jump_op) {
  assert(ast != NULL && exp != NULL);
  int64_t prepc;
  losu_stddef_vmins_t *pre;
  int64_t *golist, *exitlist;
  if (!inv) {
    golist = &exp->value._jump.f;
    exitlist = &exp->value._jump.t;
  } else {
    golist = &exp->value._jump.t;
    exitlist = &exp->value._jump.f;
  }
  losu_syntax_parser_binder_getvar(ast, exp);
  prepc = ast->pc - 1;
  pre = &ast->script->code[prepc];
  if (!losu_vm_ins_isjmp(losu_vm_ins_cgGetOP(*pre)))
    prepc = losu_syntax_backend_codegen(ast, jump_op, NO_JUMP, 0);
  else if (inv)
    losu_vm_ins_cgSetOP(
        *pre, losu_syntax_parser_binder_jumpinvert(losu_vm_ins_cgGetOP(*pre)));

  losu_syntax_parser_binder_concat(ast, exitlist, prepc);
  losu_syntax_parser_binder_palist(ast, *golist,
                                   losu_syntax_parser_binder_gettag(ast));
  *golist = NO_JUMP;
}

losu_stddef_extern int64_t losu_syntax_parser_binder_getjump(
    losu_syntax_ast_t losu_stddef_notnull ast, int64_t pc) {
  assert(ast != NULL);
  int32_t o = losu_vm_ins_cgGetS(ast->script->code[pc]);
  if (o == NO_JUMP)
    return NO_JUMP;
  return pc + o + 1;
}

losu_stddef_extern int64_t
losu_syntax_parser_binder_jump(losu_syntax_ast_t losu_stddef_notnull ast) {
  assert(ast != NULL);
  int64_t j = losu_syntax_backend_codegen(ast, INS_JMP, NO_JUMP, 0);
  if (j == ast->lasttag) {
    losu_syntax_parser_binder_concat(ast, &j, ast->jump);
    ast->jump = NO_JUMP;
  }
  return j;
}

losu_stddef_extern void
losu_syntax_parser_binder_fixjump(losu_syntax_ast_t losu_stddef_notnull ast,
                                  int64_t pc, int64_t dt) {
  assert(ast != NULL);
  losu_stddef_vmins_t *jmp = &ast->script->code[pc];
  if (dt != NO_JUMP) {
    int64_t o = dt - (pc + 1);
    if ((o > 0 ? o : -o) > losu_vm_ins_max_S) {
      losu_syntax_error(ast->syn, ast->syn->linenumber, ERRNO_BINDER_FIXJUMP);
      losu_vm_ins_cgSetS(*jmp, NO_JUMP);
      return;
    }
  }
  losu_vm_ins_cgSetS(*jmp, NO_JUMP);
}

losu_stddef_extern void
losu_syntax_parser_binder_concat(losu_syntax_ast_t losu_stddef_notnull ast,
                                 int64_t *losu_stddef_notnull l1, int64_t l2) {
  assert(ast != NULL && l1 != NULL);
  if (*l1 != NO_JUMP) {
    int64_t l = *l1;
    for (;;) {
      int64_t n = losu_syntax_parser_binder_getjump(ast, l);
      if (n == NO_JUMP) {
        losu_syntax_parser_binder_fixjump(ast, l, l2);
        return;
      }
      l = n;
    }
    return;
  }
  *l1 = l2;
}

losu_stddef_extern int64_t
losu_syntax_parser_binder_gettag(losu_syntax_ast_t losu_stddef_notnull ast) {
  assert(ast != NULL);
  if (ast->pc != ast->lasttag) {
    int64_t ltg = ast->lasttag;
    ast->lasttag = ast->pc;
    losu_syntax_parser_binder_palist(ast, ast->jump, ltg);
    ast->jump = NO_JUMP;
  }
  return ast->pc;
}

losu_stddef_extern void
losu_syntax_parser_binder_palist(losu_syntax_ast_t losu_stddef_notnull ast,
                                 int64_t list, int64_t tag) {
  assert(ast != NULL);
  if (tag == ast->lasttag)
    losu_syntax_parser_binder_concat(ast, &ast->jump, list);
  else
    losu_syntax_parser_binder_palist_func(ast, list, tag, INS_END, 0);
}

losu_stddef_extern void
losu_syntax_parser_binder_palist_func(losu_syntax_ast_t losu_stddef_notnull ast,
                                      int64_t list, int64_t tag,
                                      losu_vm_ins_OP_t ins, int64_t instag) {
  assert(ast != NULL);
  losu_stddef_vmins_t *code = ast->script->code;
  while (list != NO_JUMP) {
    int64_t next = losu_syntax_parser_binder_getjump(ast, list);
    losu_stddef_vmins_t *i = &code[list];
    losu_vm_ins_OP_t op = losu_vm_ins_cgGetOP((*i));
    if (op == ins)
      losu_syntax_parser_binder_fixjump(ast, list, instag);
    else {
      losu_syntax_parser_binder_fixjump(ast, list, tag);
      if (op == INS_JMPONT)
        losu_vm_ins_cgSetOP(*i, INS_JMPT);
      else if (op == INS_JMPONF)
        losu_vm_ins_cgSetOP(*i, INS_JMPF);
    }
    list = next;
  }
}

losu_stddef_extern losu_vm_ins_OP_t
losu_syntax_parser_binder_jumpinvert(losu_vm_ins_OP_t op) {
  switch (op) {
  case INS_JMPNE:
    return INS_JMPEQ;
  case INS_JMPEQ:
    return INS_JMPNE;

  case INS_JMPLT:
    return INS_JMPGE;
  case INS_JMPLE:
    return INS_JMPGT;

  case INS_JMPGT:
    return INS_JMPLE;
  case INS_JMPGE:
    return INS_JMPLT;

  case INS_JMPT:
  case INS_JMPONT:
    return INS_JMPF;

  case INS_JMPF:
  case INS_JMPONF:
    return INS_JMPT;

  default:
    return INS_END;
  }
}

losu_stddef_extern losu_stddef_bool_t losu_syntax_parser_binder_needval(
    losu_syntax_ast_t losu_stddef_notnull ast, int64_t l, losu_vm_ins_OP_t op) {
  assert(ast != NULL);
  for (; l != NO_JUMP; l = losu_syntax_parser_binder_getjump(ast, l))
    if (losu_vm_ins_cgGetOP(ast->script->code[l]) != op)
      return 1;
  return 0;
}

losu_stddef_extern void losu_syntax_parser_binder_tostack(
    losu_syntax_ast_t losu_stddef_notnull ast,
    losu_syntax_ast_t_exp_t losu_stddef_notnull exp) {
#define codelab(ast, op, arg)                                                  \
  (losu_syntax_parser_binder_gettag((ast)),                                    \
   losu_syntax_backend_codegen((ast), (op), (arg), 0))

  assert(ast != NULL && exp != NULL);
  if (losu_syntax_parser_binder_getvar(ast, exp))
    return;

  losu_vm_ins_OP_t pre = losu_vm_ins_cgGetOP(ast->script->code[ast->pc - 1]);
  if (!losu_vm_ins_isjmp(pre) && exp->value._jump.f == NO_JUMP &&
      exp->value._jump.t == NO_JUMP)
    return;

  int64_t final;
  int64_t j = NO_JUMP, pn = NO_JUMP, pl = NO_JUMP;
  if (losu_vm_ins_isjmp(pre) ||
      losu_syntax_parser_binder_needval(ast, exp->value._jump.f, INS_JMPONF) ||
      losu_syntax_parser_binder_needval(ast, exp->value._jump.t, INS_JMPONT)) {
    if (losu_vm_ins_isjmp(pre))
      losu_syntax_parser_binder_concat(ast, &exp->value._jump.t, ast->pc - 1);
    else {
      j = codelab(ast, INS_JMP, NO_JUMP);
      losu_syntax_parser_binder_popstack(ast, 1);
    }
    pn = codelab(ast, INS_PUSHNILJMP, NO_JUMP);
    pl = codelab(ast, INS_PUSHFLOAT,
                 losu_syntax_parser_binder_floatvalue(ast, 1));
    losu_syntax_parser_binder_palist(ast, j,
                                     losu_syntax_parser_binder_gettag(ast));
  }
  final = losu_syntax_parser_binder_gettag(ast);
  losu_syntax_parser_binder_palist_func(ast, exp->value._jump.f, pn, INS_JMPONF,
                                        final);
  losu_syntax_parser_binder_palist_func(ast, exp->value._jump.t, pl, INS_JMPONT,
                                        final);
  exp->value._jump.f = exp->value._jump.t = NO_JUMP;

#undef codelab
}

losu_stddef_extern void
losu_syntax_parser_binder_popstack(losu_syntax_ast_t losu_stddef_notnull ast,
                                   int32_t i) {
  assert(ast != NULL);
  if (i > 0)
    losu_syntax_backend_codegen(ast, INS_POP, i, 0);
  else
    losu_syntax_backend_codegen(ast, INS_PUSHNIL, -i, 0);
}

losu_stddef_extern void losu_syntax_parser_binder_prefix(
    losu_syntax_ast_t losu_stddef_notnull ast,
    losu_syntax_parser_binder_oprator_t op,
    losu_syntax_ast_t_exp_t losu_stddef_notnull exp) {
  assert(ast != NULL && exp != NULL);
  if (op == SYNOPT_NEG) {
    losu_syntax_parser_binder_tostack(ast, exp);
    losu_syntax_backend_codegen(ast, INS_NEG, 0, 0);
  } else if (op == SYNOPT_BIT_NOT) {
    losu_syntax_parser_binder_tostack(ast, exp);
    losu_syntax_backend_codegen(ast, INS_BIT_NOT, 0, 0);
  } else {
    losu_stddef_vmins_t *pre;
    losu_syntax_parser_binder_getvar(ast, exp);
    pre = &(ast->script->code[ast->pc - 1]);
    if (losu_vm_ins_isjmp(losu_vm_ins_cgGetOP(*pre)))
      losu_vm_ins_cgSetOP(*pre, losu_syntax_parser_binder_jumpinvert(
                                    losu_vm_ins_cgGetOP(*pre)));
    else
      losu_syntax_backend_codegen(ast, INS_NOT, 0, 0);
    int64_t tmp = exp->value._jump.f;
    exp->value._jump.f = exp->value._jump.t;
    exp->value._jump.t = tmp;
  }
}

losu_stddef_extern void losu_syntax_parser_binder_infix(
    losu_syntax_ast_t losu_stddef_notnull ast,
    losu_syntax_parser_binder_oprator_t op,
    losu_syntax_ast_t_exp_t losu_stddef_notnull exp) {
  assert(ast != NULL && exp != NULL);
  switch (op) {
  case SYNOPT_AND: {
    losu_syntax_parser_binder_go_true(ast, exp, 1);
    break;
  }
  case SYNOPT_OR: {
    losu_syntax_parser_binder_go_false(ast, exp, 1);
    break;
  }
  default: {
    losu_syntax_parser_binder_tostack(ast, exp);
    break;
  }
  }
}

losu_stddef_extern void losu_syntax_parser_binder_posfix(
    losu_syntax_ast_t losu_stddef_notnull ast,
    losu_syntax_parser_binder_oprator_t op,
    losu_syntax_ast_t_exp_t losu_stddef_notnull exp1,
    losu_syntax_ast_t_exp_t losu_stddef_notnull exp2) {

#define losu_syntax_parser_binder_opt_pre(name, vmins, l, r) vmins
  static const losu_vm_ins_OP_t _alops[] =
      losu_syntax_parser_binder_operator_t_text;
#undef losu_syntax_parser_binder_opt_pre

  assert(ast != NULL && exp1 != NULL && exp2 != NULL);

  switch (op) {
  case SYNOPT_AND: {
    losu_syntax_parser_binder_getvar(ast, exp2);
    exp1->value._jump.t = exp2->value._jump.t;
    losu_syntax_parser_binder_concat(ast, &exp1->value._jump.f,
                                     exp2->value._jump.f);
    break;
  }
  case SYNOPT_OR: {
    losu_syntax_parser_binder_getvar(ast, exp2);
    exp1->value._jump.f = exp2->value._jump.f;
    losu_syntax_parser_binder_concat(ast, &exp1->value._jump.t,
                                     exp2->value._jump.t);
    break;
  }
  default: {
    losu_syntax_parser_binder_tostack(ast, exp2);
    losu_syntax_backend_codegen(ast, _alops[op], -1, 0);
    break;
  }
  }
}
