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

#ifndef LOSU_INC_LOSU_H
#define LOSU_INC_LOSU_H

#define LOSU_VERSION 2, 0, 0

#define LOSU_MAGIC 0x36363655534f4c00ULL

/** @page include cfgdefault.h to fixed macros  */
#include "losu/include/cfgdefault.h"

/** @page include errno */

/** @page typedef */
#include "losu/include/typedef.h"

/** @page base */
/* cpu */
#include "losu/include/base/api/cpu.h"
/* io */
#include "losu/include/base/api/io.h"
/* mem */
#include "losu/include/base/api/mem.h"
/* string */
#include "losu/include/base/api/string.h"

/** @page object */
/* types */
#include "losu/include/object/api/types.h"
/* object */
#include "losu/include/object/api/object.h"
/* types: nil */
#include "losu/include/object/api/types/nil.h"
/* types: float */
#include "losu/include/object/api/types/float.h"
/* types: string */
#include "losu/include/object/api/types/string.h"
/* types: unit */
#include "losu/include/object/api/types/unit.h"
/* types: func */
#include "losu/include/object/api/types/func.h"
/* types: resource */
#include "losu/include/object/api/types/resource.h"
/* types: thread */
#include "losu/include/object/api/types/thread.h"
/* types: meta */
#include "losu/include/object/api/types/meta.h"
/* types: context */
#include "losu/include/object/api/types/context.h"

/** @page vm */
/* interface */
#include "losu/include/vm/api/interface.h"
/* io */
#include "losu/include/vm/api/io.h"
/* instruct */
#include "losu/include/vm/api/instruct.h"
/* mem */
#include "losu/include/vm/api/mem.h"
/* gc */
#include "losu/include/vm/api/gc.h"
/* signal */
#include "losu/include/vm/api/signal.h"
/* regctx */
#include "losu/include/vm/api/regctx.h"
/* stack */
#include "losu/include/vm/api/stack.h"
/* alu */
#include "losu/include/vm/api/alu.h"
/* thread */
#include "losu/include/vm/api/thread.h"
/* fiber */
#include "losu/include/vm/api/fiber.h"
/* core */
#include "losu/include/vm/api/core.h"

/** @page syntax */
/* interface */
#include "losu/include/syntax/api/interface.h"
/* token */
#include "losu/include/syntax/api/token.h"
/* lexer */
#include "losu/include/syntax/api/lexer.h"
/* parser */
#include "losu/include/syntax/api/parser.h"
/* backend */
#include "losu/include/syntax/api/backend.h"

#endif
