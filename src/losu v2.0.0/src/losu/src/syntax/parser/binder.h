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

#ifndef LOSU_INC_SYNTAX_API_BINDER_H
#define LOSU_INC_SYNTAX_API_BINDER_H

#define NO_JUMP -1

typedef enum losu_syntax_parser_binder_oprator_t
    losu_syntax_parser_binder_oprator_t;
#define losu_syntax_parser_binder_opt_pre(name, vmins, l, r) name
#define losu_syntax_parser_binder_operator_t_text                              \
  {                                                                            \
      /* + */                                                                  \
      losu_syntax_parser_binder_opt_pre(SYNOPT_ADD, INS_ADD, 6, 6),            \
      losu_syntax_parser_binder_opt_pre(SYNOPT_SUB, INS_SUB, 6, 6),            \
      losu_syntax_parser_binder_opt_pre(SYNOPT_MUL, INS_MULT, 7, 7),           \
      losu_syntax_parser_binder_opt_pre(SYNOPT_DIV, INS_DIV, 7, 7),            \
      losu_syntax_parser_binder_opt_pre(SYNOPT_DIVI, INS_DIVI, 7, 7),          \
      losu_syntax_parser_binder_opt_pre(SYNOPT_POW, INS_POW, 9, 8),            \
      losu_syntax_parser_binder_opt_pre(SYNOPT_MOD, INS_MOD, 7, 7),            \
      losu_syntax_parser_binder_opt_pre(SYNOPT_CONCAT, INS_CONCAT, 5, 5),      \
                                                                               \
      /* bit */                                                                \
      losu_syntax_parser_binder_opt_pre(SYNOPT_BIT_AND, INS_BIT_AND, 4, 4),    \
      losu_syntax_parser_binder_opt_pre(SYNOPT_BIT_OR, INS_BIT_OR, 3, 3),      \
      losu_syntax_parser_binder_opt_pre(SYNOPT_BIT_XOR, INS_BIT_XOR, 4, 4),    \
      losu_syntax_parser_binder_opt_pre(SYNOPT_BIT_SHL, INS_BIT_SHL, 5, 5),    \
      losu_syntax_parser_binder_opt_pre(SYNOPT_BIT_SHR, INS_BIT_SHR, 5, 5),    \
      losu_syntax_parser_binder_opt_pre(SYNOPT_BIT_SHRU, INS_BIT_SHRU, 5, 5),  \
                                                                               \
      /* :> ~> */                                                              \
      losu_syntax_parser_binder_opt_pre(SYNOPT_PIPE, INS_PIPE, 2, 2),          \
      losu_syntax_parser_binder_opt_pre(SYNOPT_LAMBDA, INS_LAMBDA, 1, 1),      \
                                                                               \
      /* neg */                                                                \
      losu_syntax_parser_binder_opt_pre(SYNOPT_NEG, INS_NEG, 9, 9),            \
      losu_syntax_parser_binder_opt_pre(SYNOPT_NOT, INS_NOT, 9, 9),            \
      losu_syntax_parser_binder_opt_pre(SYNOPT_BIT_NOT, INS_BIT_NOT, 9, 9),    \
                                                                               \
      /* != */                                                                 \
      losu_syntax_parser_binder_opt_pre(SYNOPT_NE, INS_JMPNE, 5, 5),           \
      losu_syntax_parser_binder_opt_pre(SYNOPT_EQ, INS_JMPEQ, 5, 5),           \
      losu_syntax_parser_binder_opt_pre(SYNOPT_LT, INS_JMPLT, 5, 5),           \
      losu_syntax_parser_binder_opt_pre(SYNOPT_LE, INS_JMPLE, 5, 5),           \
      losu_syntax_parser_binder_opt_pre(SYNOPT_GT, INS_JMPGT, 5, 5),           \
      losu_syntax_parser_binder_opt_pre(SYNOPT_GE, INS_JMPGE, 5, 5),           \
                                                                               \
      /* and or */                                                             \
      losu_syntax_parser_binder_opt_pre(SYNOPT_AND, INS_END, 4, 4),            \
      losu_syntax_parser_binder_opt_pre(SYNOPT_OR, INS_END, 3, 3),             \
                                                                               \
      losu_syntax_parser_binder_opt_pre(SYNOPT_INVALID, INS_END, 0, 0),        \
                                                                               \
  }

enum losu_syntax_parser_binder_oprator_t
    losu_syntax_parser_binder_operator_t_text;

#undef losu_syntax_parser_binder_opt_pre

/* apis */

losu_stddef_extern uint32_t losu_syntax_parser_binder_floatvalue(
    losu_syntax_ast_t losu_stddef_notnull ast, losu_obj_float_t num);

losu_stddef_extern uint32_t losu_syntax_parser_binder_stringvalue(
    losu_syntax_ast_t losu_stddef_notnull ast,
    losu_obj_string_t losu_stddef_notnull str);

losu_stddef_extern losu_stddef_bool_t losu_syntax_parser_binder_getvar(
    losu_syntax_ast_t losu_stddef_notnull ast,
    losu_syntax_ast_t_exp_t losu_stddef_notnull exp);

losu_stddef_extern void losu_syntax_parser_binder_setvar(
    losu_syntax_ast_t losu_stddef_notnull ast,
    losu_syntax_ast_t_exp_t losu_stddef_notnull exp);

losu_stddef_extern void losu_syntax_parser_binder_go_true(
    losu_syntax_ast_t losu_stddef_notnull ast,
    losu_syntax_ast_t_exp_t losu_stddef_notnull exp, losu_stddef_bool_t keep);

losu_stddef_extern void losu_syntax_parser_binder_go_false(
    losu_syntax_ast_t losu_stddef_notnull ast,
    losu_syntax_ast_t_exp_t losu_stddef_notnull exp, losu_stddef_bool_t keep);

losu_stddef_extern void
losu_syntax_parser_binder_go(losu_syntax_ast_t losu_stddef_notnull ast,
                             losu_syntax_ast_t_exp_t losu_stddef_notnull exp,
                             losu_stddef_bool_t inv, losu_vm_ins_OP_t jump_op);

losu_stddef_extern int64_t losu_syntax_parser_binder_getjump(
    losu_syntax_ast_t losu_stddef_notnull ast, int64_t pc);

losu_stddef_extern int64_t
losu_syntax_parser_binder_jump(losu_syntax_ast_t losu_stddef_notnull ast);

losu_stddef_extern void
losu_syntax_parser_binder_fixjump(losu_syntax_ast_t losu_stddef_notnull ast,
                                  int64_t pc, int64_t dt);

losu_stddef_extern void
losu_syntax_parser_binder_concat(losu_syntax_ast_t losu_stddef_notnull ast,
                                 int64_t *losu_stddef_notnull l1, int64_t l2);

losu_stddef_extern int64_t
losu_syntax_parser_binder_gettag(losu_syntax_ast_t losu_stddef_notnull ast);

losu_stddef_extern void
losu_syntax_parser_binder_palist(losu_syntax_ast_t losu_stddef_notnull ast,
                                 int64_t list, int64_t tag);

losu_stddef_extern void
losu_syntax_parser_binder_palist_func(losu_syntax_ast_t losu_stddef_notnull ast,
                                      int64_t list, int64_t tag,
                                      losu_vm_ins_OP_t ins, int64_t instag);

losu_stddef_extern losu_vm_ins_OP_t
losu_syntax_parser_binder_jumpinvert(losu_vm_ins_OP_t op);

losu_stddef_extern losu_stddef_bool_t losu_syntax_parser_binder_needval(
    losu_syntax_ast_t losu_stddef_notnull ast, int64_t l, losu_vm_ins_OP_t op);

losu_stddef_extern void losu_syntax_parser_binder_tostack(
    losu_syntax_ast_t losu_stddef_notnull ast,
    losu_syntax_ast_t_exp_t losu_stddef_notnull exp);

losu_stddef_extern void
losu_syntax_parser_binder_popstack(losu_syntax_ast_t losu_stddef_notnull ast,
                                   int32_t i);

losu_stddef_extern void losu_syntax_parser_binder_prefix(
    losu_syntax_ast_t losu_stddef_notnull ast,
    losu_syntax_parser_binder_oprator_t op,
    losu_syntax_ast_t_exp_t losu_stddef_notnull exp);

losu_stddef_extern void losu_syntax_parser_binder_infix(
    losu_syntax_ast_t losu_stddef_notnull ast,
    losu_syntax_parser_binder_oprator_t op,
    losu_syntax_ast_t_exp_t losu_stddef_notnull exp);

losu_stddef_extern void losu_syntax_parser_binder_posfix(
    losu_syntax_ast_t losu_stddef_notnull ast,
    losu_syntax_parser_binder_oprator_t op,
    losu_syntax_ast_t_exp_t losu_stddef_notnull exp1,
    losu_syntax_ast_t_exp_t losu_stddef_notnull exp2);

#endif
