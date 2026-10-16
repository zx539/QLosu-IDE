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

#ifndef LOSU_INC_SYNTAX_API_LEXER_H
#define LOSU_INC_SYNTAX_API_LEXER_H

/* ---- interface ---- */

/**
 * @brief init lexer
 * @param lex
 * @return
 */
losu_stddef_extern void
losu_syntax_lexer_init(losu_syntax_t losu_stddef_notnull lex);

/* ---- charset ---- */

/**
 * @brief get length of utf-8 charset
 * @param ch first byte of charset
 * @return
 * @retval -1 is error
 */
losu_stddef_extern int32_t losu_syntax_lexer_utf8len(uint8_t ch);

/**
 * @brief hex to char
 * @param ch
 * @return
 * @retval -1 is error
 */
losu_stddef_extern uint8_t losu_syntax_lexer_hex2uchar(uint8_t ch);

/**
 * @brief unicode to utf-8
 * @param unicode
 * @param buff
 * @return
 */
losu_stddef_extern int32_t losu_syntax_lexer_unicode2utf8(
    uint32_t unicode, char *losu_stddef_notnull buff);

/* ---- buffer ---- */

/**
 * @brief check vm buffer is enough
 * @param vm
 * @param n
 * @param len
 * @return
 */
losu_stddef_extern void
losu_syntax_lexer_check_buffer(losu_vm_t losu_stddef_notnull vm, size_t n,
                               size_t len);

/* ---- next ---- */

/**
 * @brief get next char
 * @param lex
 * @return
 */
losu_stddef_extern void
losu_syntax_lexer_nextchar(losu_syntax_t losu_stddef_notnull lex);

/**
 * @brief add line number
 * @param lex
 * @return
 */
losu_stddef_extern void
losu_syntax_lexer_addline(losu_syntax_t losu_stddef_notnull lex);

/**
 * @brief save charset
 * @param vm
 * @param c
 * @param len
 * @return
 */
losu_stddef_extern void losu_syntax_lexer_save(losu_vm_t losu_stddef_notnull vm,
                                               uint8_t c,
                                               size_t *losu_stddef_notnull len);

#define losu_syntax_lexer_save_next(vm, lex, l)                                \
  (losu_syntax_lexer_save((vm), (lex)->current, (l)),                          \
   losu_syntax_lexer_nextchar((lex)))

/**
 * @brief get next token with value
 * @param lex
 * @param tkv
 * @return
 */
losu_stddef_extern int16_t losu_syntax_lexer_next(
    losu_syntax_t losu_stddef_notnull lex,
    losu_syntax_lexer_token_t_value_t losu_stddef_notnull tkv);

/* ---- read ---- */

/**
 * @brief read identify
 * @param lex
 * @param fix if need fix char '_'
 * @param tkv
 * @return
 */
losu_stddef_extern void losu_syntax_lexer_read_identify(
    losu_syntax_t losu_stddef_notnull lex, uint8_t fix,
    losu_syntax_lexer_token_t_value_t losu_stddef_notnull tkv);

/**
 * @brief read float
 * @param lex
 * @param tkv
 * @param dot
 * @return
 */
losu_stddef_extern void losu_syntax_lexer_read_float(
    losu_syntax_t losu_stddef_notnull lex,
    losu_syntax_lexer_token_t_value_t losu_stddef_notnull tkv,
    losu_stddef_bool_t dot);

/**
 * @brief read rich identify
 * @param lex
 * @param tkv
 * @return
 */
losu_stddef_extern void losu_syntax_lexer_read_richident(
    losu_syntax_t losu_stddef_notnull lex,
    losu_syntax_lexer_token_t_value_t losu_stddef_notnull tkv);

/**
 * @brief read meta
 * @param lex
 * @param tkv
 * @return
 */
losu_stddef_extern void losu_syntax_lexer_read_meta(
    losu_syntax_t losu_stddef_notnull lex,
    losu_syntax_lexer_token_t_value_t losu_stddef_notnull tkv);

/**
 * @brief read strfmt
 * @param lex
 * @param tkv
 * @return
 */
losu_stddef_extern void losu_syntax_lexer_read_strfmt(
    losu_syntax_t losu_stddef_notnull lex,
    losu_syntax_lexer_token_t_value_t losu_stddef_notnull tkv);

/**
 * @brief read string muti line
 * @param lex
 * @param tkv
 * @return
 */
losu_stddef_extern void losu_syntax_lexer_read_strmuti(
    losu_syntax_t losu_stddef_notnull lex,
    losu_syntax_lexer_token_t_value_t losu_stddef_notnull tkv);

/**
 * @brief read string hex
 * @param lex
 * @param tkv
 * @return
 */
losu_stddef_extern void losu_syntax_lexer_read_strhex(
    losu_syntax_t losu_stddef_notnull lex,
    losu_syntax_lexer_token_t_value_t losu_stddef_notnull tkv);

#endif
