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

losu_stddef_extern void
losu_syntax_parser_case_continue(losu_syntax_t losu_stddef_notnull syn) {
  assert(syn != NULL);

  losu_syntax_ast_t losu_stddef_notnull ast = syn->ast;
  losu_syntax_ast_t_continue_t ctn = ast->continuelist;
  int32_t c = ast->stacklevel;

  if (ctn == NULL)
    losu_syntax_error(syn, syn->linenumber, ERRNO_CONTINUE);

  losu_syntax_parser_next_token(syn);

  losu_syntax_parser_binder_popstack(ast, c - ctn->stacklevel);
  if (ctn->tmppc != NO_JUMP)
    losu_syntax_backend_codegen(ast, INS_JMP, ctn->tmppc - ast->pc - 1, 0);
  else {
    if (ctn->startpc == NO_JUMP)
      ctn->tmppc = losu_syntax_backend_codegen(ast, INS_JMP, 1, 0);
    else
      ctn->tmppc = losu_syntax_backend_codegen(ast, INS_JMP,
                                               ctn->startpc - ast->pc - 1, 0);
  }
  ast->stacklevel = c;
}
