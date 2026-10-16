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

losu_stddef_extern inline void
losu_syntax_lexer_nextchar(losu_syntax_t losu_stddef_notnull lex) {
  assert(lex != NULL);
  lex->current = losu_vm_io_getc(lex->io);
}

losu_stddef_extern inline void
losu_syntax_lexer_addline(losu_syntax_t losu_stddef_notnull lex) {
  if (lex->linenumber == UINT32_MAX)
    losu_syntax_error(lex, lex->linenumber, ERRNO_LINE_OVERFLOW);
  lex->linenumber++;
}

losu_stddef_extern inline void
losu_syntax_lexer_save(losu_vm_t losu_stddef_notnull vm, uint8_t c,
                       size_t *len) {
  vm->buff[(*len)++] = c;
}

losu_stddef_extern int16_t losu_syntax_lexer_next(
    losu_syntax_t losu_stddef_notnull lex,
    losu_syntax_lexer_token_t_value_t losu_stddef_notnull tkv) {
  assert(lex != NULL && tkv != NULL);
  for (;;) {
    switch (lex->current) {
    case EOF: {
      return TOKEN_EOZ;
    }
    /* --- ignore ---- */
    case '\0':
    case '\r':
    case '\t':
    case ' ': {
      losu_syntax_lexer_nextchar(lex);
      break;
    }
    /* ---- next line ---- */
    case '\n': {
      losu_syntax_lexer_nextchar(lex);
      losu_syntax_lexer_addline(lex);
      break;
    }
      /* ---- operator ---- */
    case '/': {
      losu_syntax_lexer_nextchar(lex);
      if (lex->current == '%') {
        losu_syntax_lexer_nextchar(lex);
        return INS_DIVI;
      }
      if (lex->current == '/') {
        losu_syntax_lexer_nextchar(lex);
        while (lex->current != EOF) {
          if (lex->current == '\n')
            break;
          losu_syntax_lexer_nextchar(lex);
        }
        break;
      }
      if (lex->current == '*') {
        losu_syntax_lexer_nextchar(lex);
        for (;;) {
          if (lex->current == EOF)
            losu_syntax_error(lex, lex->linenumber, ERRNO_INVALID_COMMENT);
          if (lex->current == '\n')
            losu_syntax_lexer_addline(lex);
          if (lex->current == '*') {
            losu_syntax_lexer_nextchar(lex);
            if (lex->current == '/') {
              losu_syntax_lexer_nextchar(lex);
              break;
            }
            continue;
          }
          losu_syntax_lexer_nextchar(lex);
        }
        break;
      }
      return '/';
    }
    case '*': {
      losu_syntax_lexer_nextchar(lex);
      if (lex->current == '*') {
        losu_syntax_lexer_nextchar(lex);
        return TOKEN_OPOW;
      }
      return '*';
    }
    case '.': {
      losu_syntax_lexer_nextchar(lex);
      if (lex->current == '.') {
        losu_syntax_lexer_nextchar(lex);
        if (lex->current == '.') {
          losu_syntax_lexer_nextchar(lex);
          return TOKEN_DOT;
        }
        return INS_CONCAT;
      }
      return '.';
    }
    case '<': {
      losu_syntax_lexer_nextchar(lex);
      if (lex->current == '<') {
        losu_syntax_lexer_nextchar(lex);
        return TOKEN_OSHL;
      }
      if (lex->current == '=') {
        losu_syntax_lexer_nextchar(lex);
        return TOKEN_OLE;
      }
      return '<';
    }
    case '>': {
      losu_syntax_lexer_nextchar(lex);
      if (lex->current == '>') {
        losu_syntax_lexer_nextchar(lex);
        if (lex->current == '>') {
          losu_syntax_lexer_nextchar(lex);
          return TOKEN_OSHRU;
        }
        return TOKEN_OSHR;
      }
      if (lex->current == '=') {
        losu_syntax_lexer_nextchar(lex);
        return TOKEN_OGE;
      }
      return '>';
    }
    case ':': {
      losu_syntax_lexer_nextchar(lex);
      if (lex->current == '>') {
        losu_syntax_lexer_nextchar(lex);
        return TOKEN_OPIPE;
      }
      if (lex->current == ':') {
        losu_syntax_lexer_nextchar(lex);
        return TOKEN_OINDEX;
      }
      return ':';
    }
    case '~': {
      losu_syntax_lexer_nextchar(lex);
      if (lex->current == '>') {
        losu_syntax_lexer_nextchar(lex);
        return TOKEN_OLAMBDA;
      }
      return '~';
    }
    case '=': {
      losu_syntax_lexer_nextchar(lex);
      if (lex->current == '=') {
        losu_syntax_lexer_nextchar(lex);
        return TOKEN_OEQ;
      }
      if (lex->current == '>') {
        losu_syntax_lexer_nextchar(lex);
        return TOKEN_ASSIGN;
      }
      return '=';
    }
    case '!': {
      losu_syntax_lexer_nextchar(lex);
      if (lex->current == '=') {
        losu_syntax_lexer_nextchar(lex);
        return TOKEN_ONE;
      }
      return '!';
    }
    /* ---- identify ---- */
    case '`': {
      losu_syntax_lexer_read_richident(lex, tkv);
      return TOKEN_IDENTIFY;
    }
    /* ---- meta ---- */
    case '\'': {
      losu_syntax_lexer_read_meta(lex, tkv);
      return TOKEN_META;
    }
    /* ---- string ---- */
    case '"': {
      losu_syntax_lexer_read_strfmt(lex, tkv);
      return TOKEN_STRING;
    }
    case '#': {
      losu_syntax_lexer_nextchar(lex);
      if (lex->current == '|') {
        losu_syntax_lexer_nextchar(lex);
        losu_syntax_lexer_read_strmuti(lex, tkv);
        return TOKEN_STRING;
      }
      if (lex->current == 'x' || lex->current == 'X') {
        losu_syntax_lexer_nextchar(lex);
        losu_syntax_lexer_read_strhex(lex, tkv);
        return TOKEN_STRING;
      }
      return '#';
    }
    /* ---- float ---- */
    case '0': {
      losu_syntax_lexer_nextchar(lex);
      tkv->_float = 0;
      if (lex->current == 'x') {
        losu_syntax_lexer_nextchar(lex);
        uint32_t intnum = 0;
        for (int i = 0; i < 8; i++) {
          if (!isxdigit(lex->current))
            break;
          intnum = (intnum << 4) |
                   losu_syntax_lexer_hex2uchar((uint8_t)lex->current);
          losu_syntax_lexer_nextchar(lex);
        }
        tkv->_float = (int32_t)intnum;
      } else if (lex->current == 'b') {
        losu_syntax_lexer_nextchar(lex);
        uint32_t intnum = 0;
        for (int i = 0; i < 32; i++) {
          if (lex->current != '0' && lex->current != '1')
            break;
          intnum = (intnum << 1) | (uint32_t)(lex->current == '1');
          losu_syntax_lexer_nextchar(lex);
        }
        tkv->_float = (int32_t)intnum;
      } else if (lex->current == 'a') {
        losu_syntax_lexer_nextchar(lex);
        if (!isalnum(lex->current))
          losu_syntax_error(lex, lex->linenumber, "invalid format '0a'");
        tkv->_float = lex->current;
        losu_syntax_lexer_nextchar(lex);
      } else if (lex->current == '.') {
        losu_syntax_lexer_nextchar(lex);
        losu_syntax_lexer_read_float(lex, tkv, 1);
      }
      return TOKEN_FLOAT;
    }
    default: {
      uint8_t fixed = 0;
      if (isdigit(lex->current)) {
        losu_syntax_lexer_read_float(lex, tkv, 0);
        return TOKEN_FLOAT;
      }
      if (lex->current == '_') {
        losu_syntax_lexer_nextchar(lex);
        if (isalnum(lex->current) || lex->current == '_' ||
            losu_syntax_lexer_utf8len((uint8_t)lex->current) > 1)
          fixed = '_';
        else
          return TOKEN_SELF;
      }
      if (!isalpha(lex->current) && lex->current < 0x80 && !fixed) {
        int16_t c = lex->current;
        losu_syntax_lexer_nextchar(lex);
        return c;
      }
      losu_syntax_lexer_read_identify(lex, fixed, tkv);
      if (tkv->_string->marked > 0xff)
        return tkv->_string->marked;
      return TOKEN_IDENTIFY;
    }
    }
  }
}
