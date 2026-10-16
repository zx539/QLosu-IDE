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

#include "losu/src/syntax/lexer/errno.h"

#include <ctype.h>
#include <stdio.h>

losu_stddef_extern void losu_syntax_lexer_read_strfmt(
    losu_syntax_t losu_stddef_notnull lex,
    losu_syntax_lexer_token_t_value_t losu_stddef_notnull tkv) {
  assert(lex != NULL && tkv != NULL);

  losu_vm_t vm = lex->vm;
  size_t l = 0;
  losu_syntax_lexer_check_buffer(vm, l, 32);
  losu_syntax_lexer_nextchar(lex);
  while (lex->current != '"') {
    if (lex->current == '\n' || lex->current == EOF)
      losu_syntax_error(lex, lex->linenumber, ERRNO_INVALID_STRING);
    losu_syntax_lexer_check_buffer(vm, l, 32);
    if (lex->current == '\\') {
      losu_syntax_lexer_nextchar(lex);
      switch (lex->current) {
      case '"': {
        losu_syntax_lexer_save(vm, '"', &l);
        losu_syntax_lexer_nextchar(lex);
        break;
      }
      case '\\': {
        losu_syntax_lexer_save(vm, '\\', &l);
        losu_syntax_lexer_nextchar(lex);
        break;
      }
      case '/': {
        losu_syntax_lexer_save(vm, '/', &l);
        losu_syntax_lexer_nextchar(lex);
        break;
      }
      case 'b': {
        losu_syntax_lexer_save(vm, '\b', &l);
        losu_syntax_lexer_nextchar(lex);
        break;
      }
      case 'f': {
        losu_syntax_lexer_save(vm, '\f', &l);
        losu_syntax_lexer_nextchar(lex);
        break;
      }
      case 'n': {
        losu_syntax_lexer_save(vm, '\n', &l);
        losu_syntax_lexer_nextchar(lex);
        break;
      }
      case 'r': {
        losu_syntax_lexer_save(vm, '\r', &l);
        losu_syntax_lexer_nextchar(lex);
        break;
      }
      case 't': {
        losu_syntax_lexer_save(vm, '\t', &l);
        losu_syntax_lexer_nextchar(lex);
        break;
      }
      case 'u': {
        char hex[5] = {0};
        for (int i = 0; i < 4; i++) {
          losu_syntax_lexer_nextchar(lex);
          if (!isxdigit(lex->current))
            losu_syntax_error(lex, lex->linenumber, ERRNO_INVALID_STRING);
          hex[i] = lex->current;
        }
        losu_syntax_lexer_nextchar(lex);
        uint32_t unicode = 0;
        sscanf(hex, "%x", &unicode);
        if (unicode > 0x10FFFF)
          losu_syntax_error(lex, lex->linenumber, ERRNO_INVALID_STRING);
        char buff[5] = {0};
        int32_t len = losu_syntax_lexer_unicode2utf8(unicode, buff);
        losu_syntax_lexer_check_buffer(vm, l, 32);
        for (int i = 0; i < len; i++)
          losu_syntax_lexer_save(vm, buff[i], &l);
        break;
      }
      default: {
        losu_syntax_error(lex, lex->linenumber, ERRNO_INVALID_STRING);
      }
      }
      continue;
    }
    losu_syntax_lexer_save_next(vm, lex, &l);
  }
  losu_syntax_lexer_nextchar(lex);
  tkv->_string = losu_object_string_new_len(vm, vm->buff, l);
}
