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

losu_stddef_extern void
losu_syntax_parser_case_if(losu_syntax_t losu_stddef_notnull syn) {
#define condition(syn, ast, exp)                                               \
  {                                                                            \
    losu_syntax_parser_subexp((syn), (exp), -1);                               \
    losu_syntax_parser_binder_go_true((ast), (exp), 0);                        \
  }

#define then(syn, ast, exp, line)                                              \
  {                                                                            \
    losu_syntax_parser_check_andnext((syn), '(');                              \
    condition((syn), (ast), (exp));                                            \
    losu_syntax_parser_check_andnext((syn), ')');                              \
    losu_syntax_parser_check_andnext((syn), '{');                              \
    (line) = (syn)->linenumber;                                                \
    losu_syntax_parser_block((syn));                                           \
  }

  losu_syntax_ast_t losu_stddef_notnull ast = syn->ast;
  struct losu_syntax_ast_t_exp_t _exp = {0};
  losu_syntax_ast_t_exp_t exp = &_exp;

  int64_t el = NO_JUMP;
  int32_t naloc = (int32_t)ast->naloc;
  uint32_t line;

  losu_syntax_parser_next_token(syn);
  losu_syntax_parser_check_andnext(syn, '(');

  if (syn->tk.token != TOKEN_VAR) {
    condition(syn, ast, exp);
  } else {
    losu_syntax_parser_next_token(syn);
    losu_syntax_parser_localvar_new(
        syn, losu_syntax_parser_check_isidentify_andnext(syn), 0);
    losu_syntax_parser_check_andnext(syn, '=');

    losu_syntax_parser_exp(syn);

    losu_syntax_parser_ast_adjstack(ast, 1, 1);
    losu_syntax_parser_locavar_adjust(syn, 1);
    losu_syntax_backend_codegen(ast, INS_PUSHTOP, 0, 0);
    losu_syntax_parser_binder_go_true(ast, exp, 0);
  }

  losu_syntax_parser_check_andnext(syn, ')');
  losu_syntax_parser_check_andnext(syn, '{');
  line = syn->linenumber;
  losu_syntax_parser_block(syn);

  for (;;) {
    losu_syntax_parser_check_match_andnext(syn, '{', '}', line);
    if (syn->tk.token == TOKEN_ELSE) {
      losu_syntax_parser_next_token(syn);
      if (syn->tk.token == TOKEN_IF) {
        losu_syntax_parser_binder_concat(ast, &el,
                                         losu_syntax_parser_binder_jump(ast));
        losu_syntax_parser_binder_palist(ast, exp->value._jump.f,
                                         losu_syntax_parser_binder_gettag(ast));
        losu_syntax_parser_next_token(syn);
        then(syn, ast, exp, line);
      } else {
        losu_syntax_parser_binder_concat(ast, &el,
                                         losu_syntax_parser_binder_jump(ast));
        losu_syntax_parser_binder_palist(ast, exp->value._jump.f,
                                         losu_syntax_parser_binder_gettag(ast));
        losu_syntax_parser_check_andnext(syn, '{');
        line = syn->linenumber;
        losu_syntax_parser_block(syn);
        losu_syntax_parser_binder_palist(ast, el,
                                         losu_syntax_parser_binder_gettag(ast));
        losu_syntax_parser_check_match_andnext(syn, '{', '}', line);
        break;
      }
    } else {
      losu_syntax_parser_binder_concat(ast, &el, exp->value._jump.f);
      losu_syntax_parser_binder_palist(ast, el,
                                       losu_syntax_parser_binder_gettag(ast));
      break;
    }
  }
  losu_syntax_parser_binder_popstack(ast, (int32_t)(ast->naloc) - naloc);
  losu_syntax_parser_locvar_del(syn, (int32_t)(ast->naloc) - naloc);
}
