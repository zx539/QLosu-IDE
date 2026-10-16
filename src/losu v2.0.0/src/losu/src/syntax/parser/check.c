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

#include "losu/src/syntax/parser/errno.h"

losu_stddef_extern void
losu_syntax_parser_check_andnext(losu_syntax_t losu_stddef_notnull syn,
                                 int16_t token) {
  assert(syn != NULL);
  if (syn->tk.token != token)
    losu_syntax_parser_error(syn, token);
  losu_syntax_parser_next_token(syn);
}

losu_stddef_extern void
losu_syntax_parser_check_assert(losu_syntax_t losu_stddef_notnull syn,
                                losu_stddef_bool_t boolexp,
                                const char *losu_stddef_notnull errmsg) {
  assert(syn != NULL && errmsg != NULL);
  if (!boolexp)
    losu_syntax_error(syn, syn->linenumber, errmsg);
}

losu_stddef_extern void
losu_syntax_parser_check_match_andnext(losu_syntax_t losu_stddef_notnull syn,
                                       int16_t leftchar, int16_t rightchar,
                                       uint32_t line) {
  assert(syn != NULL);
  if (syn->tk.token != rightchar) {
    char lts[64], rts[64];
    losu_syntax_parser_token2str(leftchar, lts, sizeof(lts));
    losu_syntax_parser_token2str(rightchar, rts, sizeof(rts));
    losu_syntax_error(syn, line, ERRNO_EXPECTED_MATCH(lts, rts));
  }
}

losu_stddef_extern losu_obj_string_t losu_stddef_notnull
losu_syntax_parser_check_isidentify_andnext(
    losu_syntax_t losu_stddef_notnull syn) {
  assert(syn != NULL);
  losu_syntax_parser_check_assert(syn, syn->tk.token == TOKEN_IDENTIFY,
                                  ERRNO_EXPECTED_IDENTIFY);
  losu_obj_string_t losu_stddef_notnull ts = syn->tk.value._string;
  losu_syntax_parser_next_token(syn);
  return ts;
}

losu_stddef_extern void
losu_syntax_parser_check_limit(losu_syntax_t losu_stddef_notnull syn,
                               int32_t nitems, int32_t limit,
                               const char *losu_stddef_notnull errmsg) {
  assert(syn != NULL && errmsg != NULL);
  if (nitems < limit)
    return;
  losu_syntax_error(syn, syn->linenumber, errmsg);
}
