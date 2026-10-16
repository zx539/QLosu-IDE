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

#ifndef LOSU_INC_BASE_API_STRING_H
#define LOSU_INC_BASE_API_STRING_H

/**
 * @brief create a new string
 * @param str c string
 * @return losu_base_string_t
 * @retval NULL is failed
 */
losu_stddef_extern losu_base_string_t losu_stddef_nullable
losu_base_string_new(const char *losu_stddef_notnull str);

/**
 * @brief free a string
 * @param str string handle
 * @return void
 */
losu_stddef_extern void
losu_base_string_free(losu_base_string_t losu_stddef_notnull str);
/**
 * @brief append a string to another string
 * @param str string handle
 * @param str2 string handle
 * @return losu_stddef_bool_t
 * @retval 0 for success, 1 for fail
 */
losu_stddef_extern losu_stddef_bool_t
losu_base_string_append(losu_base_string_t losu_stddef_notnull str,
                        losu_base_string_t losu_stddef_notnull str2);
/**
 * @brief append a char to a string
 * @param ts string handle
 * @param c char
 * @return losu_stddef_bool_t
 * @retval 0 for success, 1 for fail
 */
losu_stddef_extern losu_stddef_bool_t
losu_base_string_append_char(losu_base_string_t losu_stddef_notnull ts, char c);
/**
 * @brief append a c-string to a string
 * @param ts string handle
 * @param str c string
 * @return losu_stddef_bool_t
 * @retval 0 for success, 1 for fail
 */
losu_stddef_extern losu_stddef_bool_t
losu_base_string_append_str(losu_base_string_t losu_stddef_notnull ts,
                            const char *losu_stddef_notnull str);
/**
 * @brief get c-string from a string
 * @param ts string handle
 * @return const char*
 * @retval NULL is failed
 */
losu_stddef_extern const char *losu_stddef_notnull
losu_base_string_cstr(losu_base_string_t losu_stddef_notnull ts);
/**
 * @brief get length of a string
 * @param ts string handle
 * @return size_t
 */
losu_stddef_extern size_t
losu_base_string_len(losu_base_string_t losu_stddef_notnull ts);

#endif
