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

#ifndef LOSU_INC_SYNTAX_API_TOKEN_H
#define LOSU_INC_SYNTAX_API_TOKEN_H

#define __losu_syntax_token_text(str, tk, ...) tk __VA_ARGS__

#define losu_syntax_token_text                                                 \
  {                                                                            \
      /* logic */                                                              \
      __losu_syntax_token_text("and", TOKEN_AND, = 256),                       \
      __losu_syntax_token_text("or", TOKEN_OR),                                \
      __losu_syntax_token_text("not", TOKEN_NOT),                              \
      __losu_syntax_token_text("nil", TOKEN_NIL),                              \
                                                                               \
      /* if */                                                                 \
      __losu_syntax_token_text("if", TOKEN_IF),                                \
      __losu_syntax_token_text("else", TOKEN_ELSE),                            \
                                                                               \
      /* loop */                                                               \
      __losu_syntax_token_text("while", TOKEN_WHILE),                          \
      __losu_syntax_token_text("for", TOKEN_FOR),                              \
                                                                               \
      /* control */                                                            \
      __losu_syntax_token_text("return", TOKEN_RETURN),                        \
      __losu_syntax_token_text("break", TOKEN_BREAK),                          \
      __losu_syntax_token_text("continue", TOKEN_CONTINUE),                    \
      __losu_syntax_token_text("yield", TOKEN_YIELD),                          \
                                                                               \
      /* foreign */                                                            \
      __losu_syntax_token_text("import", TOKEN_IMPORT),                        \
                                                                               \
      /* exp */                                                                \
      __losu_syntax_token_text("var", TOKEN_VAR),                              \
      __losu_syntax_token_text("", __TOKEN_SLICE__),                           \
      __losu_syntax_token_text("_", TOKEN_SELF),                               \
                                                                               \
      /* operator */                                                           \
      __losu_syntax_token_text("/%%", TOKEN_ODIVI),                            \
      __losu_syntax_token_text("**", TOKEN_OPOW),                              \
      __losu_syntax_token_text("..", TOKEN_OCONCAT),                           \
      __losu_syntax_token_text("<<", TOKEN_OSHL),                              \
      __losu_syntax_token_text(">>", TOKEN_OSHR),                              \
      __losu_syntax_token_text(">>>", TOKEN_OSHRU),                            \
      __losu_syntax_token_text(":>", TOKEN_OPIPE),                             \
      __losu_syntax_token_text("~>", TOKEN_OLAMBDA),                           \
      __losu_syntax_token_text("==", TOKEN_OEQ),                               \
      __losu_syntax_token_text("<=", TOKEN_OLE),                               \
      __losu_syntax_token_text(">=", TOKEN_OGE),                               \
      __losu_syntax_token_text("!=", TOKEN_ONE),                               \
      __losu_syntax_token_text("::", TOKEN_OINDEX),                            \
                                                                               \
      /* etc */                                                                \
      __losu_syntax_token_text("=>", TOKEN_ASSIGN),                            \
      __losu_syntax_token_text("...", TOKEN_DOT),                              \
      __losu_syntax_token_text("[identify]", TOKEN_IDENTIFY),                  \
      __losu_syntax_token_text("[meta]", TOKEN_META),                          \
      __losu_syntax_token_text("[string]", TOKEN_STRING),                      \
      __losu_syntax_token_text("[number]", TOKEN_FLOAT),                       \
      __losu_syntax_token_text("[expression]", TOKEN_EXPRESSION),              \
      __losu_syntax_token_text("[EOZ]", TOKEN_EOZ),                            \
      __losu_syntax_token_text(NULL, __TOKEN_END__),                           \
  }

typedef const struct losu_syntax_token_list_t {
  const char *str;
  int16_t token;
} *losu_syntax_token_list_t;

losu_stddef_extern const struct losu_syntax_token_list_t
    losu_syntax_token_list[];
losu_stddef_extern const size_t losu_syntax_token_list_len;

enum losu_syntax_token_id_t losu_syntax_token_text;

#endif
