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

#include "losu/src/syntax/parser/exp.h"

static void __inline_var_fromarray(losu_syntax_t losu_stddef_notnull syn);

static void __inline_var_fromunit(losu_syntax_t losu_stddef_notnull syn);

losu_stddef_extern void
losu_syntax_parser_case_var(losu_syntax_t losu_stddef_notnull syn) {
  assert(syn != NULL);

  losu_syntax_parser_next_token(syn);
  if (syn->tk.token == '[')
    __inline_var_fromarray(syn);
  else if (syn->tk.token == '{')
    __inline_var_fromunit(syn);
  else {
    int32_t expitems = 0;
    losu_syntax_parser_localvar_new(
        syn, losu_syntax_parser_check_isidentify_andnext(syn), 0);
    if (syn->tk.token == '=') {
      losu_syntax_parser_next_token(syn);
      losu_syntax_parser_exp(syn);
      expitems = 1;
    }
    losu_syntax_parser_ast_adjstack(syn->ast, 1, expitems);
    losu_syntax_parser_locavar_adjust(syn, 1);
  }
}

static void __inline_var_fromarray(losu_syntax_t losu_stddef_notnull syn) {
  assert(syn != NULL);
  losu_syntax_ast_t losu_stddef_notnull ast = syn->ast;
  losu_obj_string_t vname[losu_vm_ins_lim_locvar] = {0};
  uint16_t nvname = 0;
  uint32_t line = syn->linenumber;

  do {
    losu_syntax_parser_next_token(syn);
    losu_obj_string_t tmp = losu_syntax_parser_check_isidentify_andnext(syn);
    vname[nvname] = tmp;
    losu_syntax_parser_localvar_new(syn, tmp, nvname);
    nvname++;
  } while (syn->tk.token == ',');

  losu_syntax_parser_check_match_andnext(syn, '[', ']', line);
  losu_syntax_parser_ast_adjstack(ast, nvname, 0);
  losu_syntax_parser_locavar_adjust(syn, nvname);

  if (syn->tk.token != '=')
    return;

  losu_syntax_parser_next_token(syn);
  losu_syntax_parser_exp(syn);
  for (uint16_t i = 0; i < nvname; i++) {
    struct losu_syntax_ast_t_exp_t exp = {0};
    losu_syntax_backend_codegen(ast, INS_PUSHTOP, 0, 0);
    losu_syntax_backend_codegen(
        ast, INS_PUSHFLOAT, losu_syntax_parser_binder_floatvalue(ast, i), 0);
    losu_syntax_backend_codegen(ast, INS_GETUNIT, 0, 0);
    losu_syntax_parser_exp_loadname(syn, vname[i], &exp);
    losu_syntax_parser_binder_setvar(ast, &exp);
  }
  losu_syntax_parser_ast_adjstack(ast, 0, 1);
}

static void __inline_var_fromunit(losu_syntax_t losu_stddef_notnull syn) {
  assert(syn != NULL);
  losu_syntax_ast_t losu_stddef_notnull ast = syn->ast;
  losu_obj_string_t vname[losu_vm_ins_lim_locvar] = {0};
  losu_obj_string_t asname[losu_vm_ins_lim_locvar] = {0};
  uint16_t nvname = 0;
  uint32_t line = syn->linenumber;
  do {
    losu_syntax_parser_next_token(syn);
    vname[nvname] = losu_syntax_parser_check_isidentify_andnext(syn);
    if (syn->tk.token == ':') {
      losu_syntax_parser_next_token(syn);
      asname[nvname] = losu_syntax_parser_check_isidentify_andnext(syn);
    } else
      asname[nvname] = vname[nvname];
    losu_syntax_parser_localvar_new(syn, asname[nvname], nvname);
    nvname++;
  } while (syn->tk.token == ',');

  losu_syntax_parser_check_match_andnext(syn, '{', '}', line);
  losu_syntax_parser_ast_adjstack(ast, nvname, 0);
  losu_syntax_parser_locavar_adjust(syn, nvname);

  if (syn->tk.token != '=')
    return;

  losu_syntax_parser_next_token(syn);
  losu_syntax_parser_exp(syn);

  for (uint16_t i = 0; i < nvname; i++) {
    struct losu_syntax_ast_t_exp_t exp = {0};
    losu_syntax_backend_codegen(ast, INS_PUSHTOP, 0, 0);
    losu_syntax_backend_codegen(
        ast, INS_PUSHSTRING,
        losu_syntax_parser_binder_stringvalue(ast, vname[i]), 0);
    losu_syntax_backend_codegen(ast, INS_GETUNIT, 0, 0);
    losu_syntax_parser_exp_loadname(syn, asname[i], &exp);
    losu_syntax_parser_binder_setvar(ast, &exp);
  }
  losu_syntax_parser_ast_adjstack(ast, 0, 1);
}