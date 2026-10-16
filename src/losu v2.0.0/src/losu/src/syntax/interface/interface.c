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

#include "losu/src/syntax/interface/errno.h"

#include <stdarg.h>
#include <stdio.h>

losu_stddef_extern void losu_syntax_error(losu_syntax_t losu_stddef_notnull syn,
                                          uint32_t line,
                                          const char *losu_stddef_notnull fmt,
                                          ...) {
  assert(syn != NULL && fmt != NULL);

  char tmp[LOSU_CONFIG_VM_IO_SIZE] = {0};
  va_list ap;
  va_start(ap, fmt);

  vsnprintf(tmp, sizeof(tmp), fmt, ap);
  losu_base_io_errprintf(ERRNO_SYNTAX_ERROR, tmp, syn->io->name->data,
                         syn->io->fspath ? syn->io->fspath->data : NULL, line);

  va_end(ap);
  losu_vm_signal_throw(syn->vm, losu_vm_sig_enum_t_err);
}

losu_stddef_extern losu_stddef_notnull losu_obj_func_t_script_t
losu_syntax_load(losu_vm_t losu_stddef_notnull vm,
                 losu_vm_io_t losu_stddef_notnull io) {
  struct losu_syntax_t _syn = {0};
  struct losu_syntax_ast_t _ast = {0};

  losu_syntax_t syn = &_syn;
  losu_syntax_ast_t ast = &_ast;

  losu_syntax_parser_setin(vm, syn, io);
  losu_syntax_lexer_init(syn);

  losu_syntax_parser_ast_newfunc(syn, ast);
  losu_syntax_parser_next_token(syn);

  while (!losu_syntax_parser_block_isend(syn->tk.token))
    if (losu_syntax_parser_block_switch(syn, 1))
      break;
  losu_syntax_parser_check_andnext(syn, TOKEN_EOZ);
  losu_syntax_parser_ast_delfunc(syn);

  return ast->script;
}
