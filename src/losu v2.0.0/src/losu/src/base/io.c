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

#include <stdarg.h>
#include <stdio.h>

losu_stddef_extern int32_t losu_base_io_printf(
    losu_base_io_color_t *losu_stddef_nullable color,
    void *losu_stddef_notnull io, const char *losu_stddef_notnull fmt, ...) {
  assert(io != NULL && fmt != NULL);

  /* color */
  if (color) {
    switch (*color) {
    case losu_base_io_color_stdout_t:
      break;
    case losu_base_io_color_error_t: {
      fprintf(io, "\033[1;31m");
      break;
    }
    case losu_base_io_color_warn_t: {
      fprintf(io, "\033[1;33m");
      break;
    }
    }
  }
  va_list args;
  va_start(args, fmt);
  int32_t i = vfprintf(io, fmt, args);
  va_end(args);
  if (color) {
    fprintf(io, "\e[0m");
  }
  return i;
}

losu_stddef_extern int32_t losu_base_io_errprintf(
    const char *losu_stddef_notnull fmt, ...) {
  assert(fmt != NULL);
  va_list args;
  va_start(args, fmt);
  int32_t i = vfprintf(stderr, fmt, args);
  va_end(args);
  return i;
}

losu_stddef_extern int32_t losu_base_io_warnprintf(
    const char *losu_stddef_notnull fmt, ...) {
  assert(fmt != NULL);
  va_list args;
  va_start(args, fmt);
  fputs("\033[1;33m", stderr);
  int32_t i = vfprintf(stderr, fmt, args);
  fputs("\033[0m", stderr);
  va_end(args);
  return i;
}
