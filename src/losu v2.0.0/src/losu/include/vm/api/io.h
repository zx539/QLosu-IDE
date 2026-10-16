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

#ifndef LOSU_INC_VM_API_IO_H
#define LOSU_INC_VM_API_IO_H

struct losu_vm_io_t {
  char buff[LOSU_CONFIG_VM_IO_SIZE];
  size_t size;
  const char *losu_stddef_nullable buffer;
  int16_t (*losu_stddef_notnull fillbuff)(losu_vm_io_t losu_stddef_notnull io);
  void *losu_stddef_nullable handle;
  losu_vm_t losu_stddef_notnull vm;
  losu_obj_string_t losu_stddef_notnull name;
  losu_obj_string_t losu_stddef_nullable fspath;
  losu_stddef_bool_t bin;
  losu_stddef_bool_t out;
};

typedef losu_obj_func_t_script_t losu_stddef_notnull (
    *losu_vm_io_load_handle_t)(losu_vm_t losu_stddef_notnull vm,
                               losu_vm_io_t losu_stddef_notnull io);

losu_stddef_extern losu_vm_io_load_handle_t losu_stddef_notnull
    losu_vm_io_load_handle_ptr;

/**
 * @brief load from io to obj
 * @param io
 * @param obj
 * @return
 */
losu_stddef_extern losu_stddef_bool_t
losu_vm_io_doload(losu_vm_io_t losu_stddef_notnull io,
                  losu_object_t losu_stddef_notnull result);

/**
 * @brief throw a io error event
 * @param vm
 * @param io
 * @param msg
 * @param
 * @return
 */
losu_stddef_extern void losu_vm_io_error(losu_vm_t losu_stddef_notnull vm,
                                         losu_vm_io_t losu_stddef_notnull io,
                                         const char *losu_stddef_notnull msg,
                                         ...);

/**
 * @brief open io from handle with mode (in or out)
 * @param io
 * @param is_out
 * @param handle
 * @param name
 * @param fspath
 * @return
 */
losu_stddef_extern void losu_vm_io_open_handle(
    losu_vm_io_t losu_stddef_notnull io, losu_vm_t losu_stddef_notnull vm,
    losu_stddef_bool_t is_out, void *losu_stddef_notnull handle,
    const char *losu_stddef_notnull name,
    const char *losu_stddef_nullable fspath);

/**
 * @brief open io from buffer with mode (in or out)
 * @param io
 * @param is_out
 * @param stream
 * @param size
 * @param name
 * @param fspath
 * @return
 */
losu_stddef_extern void losu_vm_io_open_buffer(
    losu_vm_io_t losu_stddef_notnull io, losu_vm_t losu_stddef_notnull vm,
    losu_stddef_bool_t is_out, const char *losu_stddef_notnull stream,
    size_t size, const char *losu_stddef_notnull name);

/**
 * @brief get a character from io
 * @param io
 * @return
 */
losu_stddef_extern int16_t losu_vm_io_getc(losu_vm_io_t losu_stddef_notnull io);

/**
 * @brief put a character to io
 * @param io
 * @param c
 * @return
 * @retval c or EOF
 */
losu_stddef_extern int16_t losu_vm_io_putc(losu_vm_io_t losu_stddef_notnull io,
                                           uint8_t c);

#endif
