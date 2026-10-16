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

/* include losu.h */
#include "losu/include/losu.h"

#if LOSU_CONFIG_POSIX_ENABLE == 1

#include <sched.h>
#include <unistd.h>

#endif

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

losu_stddef_extern size_t losu_base_cpu_nproc() {
  long i = 1;
#if LOSU_CONFIG_POSIX_ENABLE == 1
#ifdef _SC_NPROCESSORS_ONLN
  i = sysconf(_SC_NPROCESSORS_ONLN);
#endif
#endif
  return i;
}

losu_stddef_extern inline const char *losu_stddef_notnull losu_base_cpu_arch() {
  const char *s = NULL;
#if defined(__wasm32__) || defined(__wasm32)
  s = "wasm32";
#elif defined(__x86_64__) || defined(_M_X64)
  s = "x86_64";
#elif defined(__i386__) || defined(_M_IX86)
  s = "x86";
#elif defined(__aarch64__) || defined(_M_ARM64)
  s = "aarch64";
#elif defined(__arm__) || defined(_M_ARM)
  s = "arm";
#else
  s = "unknown";
#endif

  return s;
}

losu_stddef_extern void losu_base_cpu_yield() {
#if LOSU_CONFIG_POSIX_ENABLE == 1
#ifdef EMSCRIPTEN
  emscripten_sleep(0);
#else
  sched_yield();
#endif
#endif
}
