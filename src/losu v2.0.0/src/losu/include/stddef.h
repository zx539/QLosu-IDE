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

#ifndef LOSU_INC_STDDEF_H
#define LOSU_INC_STDDEF_H

#ifdef NDEBUG
#define assert(x)
#else
#include <assert.h>
#endif

#include <stddef.h>
#include <stdint.h>

#define LOSU_STDDEF_MAGIC (0x36363655534f4c00ULL)

#ifndef losu_stddef_extern
#define losu_stddef_extern extern
#ifdef __cplusplus
#undef losu_stddef_extern
#define losu_stddef_extern extern "C"
#endif
#endif

#if __STDC_VERSION__ >= 201112L
#define losu_stddef_c11 1
#endif

#ifndef losu_stddef_notnull
#define losu_stddef_notnull
#if defined(__clang__)
#undef losu_stddef_notnull
#define losu_stddef_notnull _Nonnull
#endif
#endif

#ifndef losu_stddef_nullable
#define losu_stddef_nullable
#if defined(__clang__)
#undef losu_stddef_nullable
#define losu_stddef_nullable _Nullable
#endif
#endif

#if losu_stddef_c11
#define losu_stddef_assert(cond, msg) _Static_assert((cond), msg)
#else
#define losu_stddef_assert(cond, msg)
#endif

typedef uint8_t losu_stddef_bool_t;
typedef uint32_t losu_stddef_hash_t;
typedef uint32_t losu_stddef_vmins_t;
typedef double losu_stddef_float_t;
typedef char *losu_stddef_string_t;
typedef const char *losu_stddef_string_const_t;

#endif
