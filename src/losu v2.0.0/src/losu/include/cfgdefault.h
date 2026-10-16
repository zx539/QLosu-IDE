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

#ifndef LOSU_INC_CFGDEFAULT_H
#define LOSU_INC_CFGDEFAULT_H

#include "losu_config.h"

#pragma region "config posix items: enable"

#ifndef LOSU_CONFIG_POSIX_ENABLE
#define LOSU_CONFIG_POSIX_ENABLE 0
#endif

#pragma endregion

#pragma region "config syntax items: compiler, optimization"

#ifndef LOSU_CONFIG_SYNTAX_IO_LOAD_HANDLE
#define LOSU_CONFIG_SYNTAX_IO_LOAD_HANDLE 1
#endif

#ifndef LOSU_CONFIG_SYNTAX_IO_DUMP_HANDLE
#define LOSU_CONFIG_SYNTAX_IO_DUMP_HANDLE 1
#endif

#ifndef LOSU_CONFIG_SYNTAX_BACKEND_OPTIM
#define LOSU_CONFIG_SYNTAX_BACKEND_OPTIM 0
#endif

#pragma endregion

#pragma region "config vm items"

#ifndef LOSU_CONFIG_VM_IO_SIZE
#define LOSU_CONFIG_VM_IO_SIZE 1024
#endif

#ifndef LOSU_CONFIG_VM_GC_UPDATE_RULE
#define LOSU_CONFIG_VM_GC_UPDATE_RULE(lim) (lim) *= 2
#endif

#ifndef LOSU_CONFIG_VM_LOG_LEVEL
#define LOSU_CONFIG_VM_LOG_LEVEL 0
#endif

#pragma endregion

#endif
