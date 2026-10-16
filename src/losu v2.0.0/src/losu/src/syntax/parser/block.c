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
losu_syntax_parser_block(losu_syntax_t losu_stddef_notnull syn) {
  assert(syn != NULL);
  losu_syntax_ast_t losu_stddef_notnull ast = syn->ast;
  int32_t naloc = ast->naloc;
  while (!losu_syntax_parser_block_isend(syn->tk.token))
    if (losu_syntax_parser_block_switch(syn, 0))
      break;
  losu_syntax_parser_binder_popstack(ast, (int32_t)(ast->naloc) - naloc);
  losu_syntax_parser_locvar_del(syn, (int32_t)(ast->naloc) - naloc);
}

losu_stddef_extern losu_stddef_bool_t losu_syntax_parser_block_switch(
    losu_syntax_t losu_stddef_notnull syn, losu_stddef_bool_t need_return) {
  assert(syn != NULL);
  losu_syntax_ast_t losu_stddef_notnull ast = syn->ast;
  switch (syn->tk.token) {
  case TOKEN_IF: {
    losu_syntax_parser_case_if(syn);
    break;
  }
  case TOKEN_WHILE: {
    losu_syntax_parser_case_while(syn);
    break;
  }
  case TOKEN_FOR: {
    losu_syntax_parser_case_for(syn);
    break;
  }
  case TOKEN_VAR: {
    losu_syntax_parser_case_var(syn);
    break;
  }
  case TOKEN_RETURN: {
    losu_syntax_parser_case_return(syn);
    break;
  }
  case TOKEN_BREAK: {
    losu_syntax_parser_case_break(syn);
    break;
  }
  case TOKEN_CONTINUE: {
    losu_syntax_parser_case_continue(syn);
    break;
  }
  case TOKEN_YIELD: {
    losu_syntax_parser_case_yield(syn);
    break;
  }
  case ';': {
    losu_syntax_parser_next_token(syn);
    break;
  }
  default: {
    if (losu_syntax_parser_block_isend(syn->tk.token))
      return 1;
    struct losu_syntax_ast_t_exp_t _exp1 = {0};
    losu_syntax_ast_t_exp_t exp1 = &_exp1;
    losu_syntax_parser_subexp(syn, exp1, -1);
    if (syn->tk.token == '=') {
      losu_syntax_parser_next_token(syn);
      losu_syntax_parser_exp(syn);
      losu_syntax_parser_binder_setvar(ast, exp1);
    } else if (losu_syntax_parser_block_isend(syn->tk.token) && need_return) {
      losu_syntax_parser_binder_tostack(ast, exp1);
      losu_syntax_backend_codegen(ast, INS_RETURN, ast->naloc, 0);
      ast->stacklevel = ast->naloc;
      return 1;
    } else {
      losu_syntax_parser_binder_tostack(ast, exp1);
      losu_syntax_parser_ast_adjstack(ast, 0, 1);
    }
  }
  }
  return 0;
}

losu_stddef_extern losu_stddef_bool_t
losu_syntax_parser_block_isend(int16_t token) {
  if (token == '}' || token == TOKEN_EOZ)
    return 1;
  return 0;
}
