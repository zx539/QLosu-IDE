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

#ifndef LOSU_INC_SYNTAX_API_PARSER_H
#define LOSU_INC_SYNTAX_API_PARSER_H

/* interface */

losu_stddef_extern void
losu_syntax_parser_setin(losu_vm_t losu_stddef_notnull vm,
                         losu_syntax_t losu_stddef_notnull syn,
                         losu_vm_io_t losu_stddef_notnull io);

losu_stddef_extern void
losu_syntax_parser_token2str(int16_t token, char *losu_stddef_notnull const buf,
                             size_t len);

losu_stddef_extern void
losu_syntax_parser_error(losu_syntax_t losu_stddef_notnull syn, int16_t tk);

losu_stddef_extern void
losu_syntax_parser_next_token(losu_syntax_t losu_stddef_notnull syn);

losu_stddef_extern void
losu_syntax_parser_next_ahead_token(losu_syntax_t losu_stddef_notnull syn);

/* ast */

losu_stddef_extern void
losu_syntax_parser_ast_newfunc(losu_syntax_t losu_stddef_notnull syn,
                               losu_syntax_ast_t losu_stddef_notnull ast);

losu_stddef_extern void
losu_syntax_parser_ast_delfunc(losu_syntax_t losu_stddef_notnull syn);

losu_stddef_extern void losu_syntax_parser_ast_newbreak(
    losu_syntax_ast_t losu_stddef_notnull ast,
    losu_syntax_ast_t_break_t losu_stddef_notnull br);

losu_stddef_extern void losu_syntax_parser_ast_delbreak(
    losu_syntax_ast_t losu_stddef_notnull ast,
    losu_syntax_ast_t_break_t losu_stddef_notnull br);

losu_stddef_extern void losu_syntax_parser_ast_newcontinue(
    losu_syntax_ast_t losu_stddef_notnull ast,
    losu_syntax_ast_t_continue_t losu_stddef_notnull ctn, int64_t pc);

losu_stddef_extern void losu_syntax_parser_ast_delcontinue(
    losu_syntax_ast_t losu_stddef_notnull ast,
    losu_syntax_ast_t_continue_t losu_stddef_notnull ctn);

losu_stddef_extern void
losu_syntax_parser_ast_adjstack(losu_syntax_ast_t losu_stddef_notnull ast,
                                int32_t nv, int32_t ne);

/* check */

losu_stddef_extern void
losu_syntax_parser_check_andnext(losu_syntax_t losu_stddef_notnull syn,
                                 int16_t token);

losu_stddef_extern void
losu_syntax_parser_check_assert(losu_syntax_t losu_stddef_notnull syn,
                                losu_stddef_bool_t boolexp,
                                const char *losu_stddef_notnull errmsg);

losu_stddef_extern void
losu_syntax_parser_check_match_andnext(losu_syntax_t losu_stddef_notnull syn,
                                       int16_t leftchar, int16_t rightchar,
                                       uint32_t line);

losu_stddef_extern losu_obj_string_t losu_stddef_notnull
losu_syntax_parser_check_isidentify_andnext(
    losu_syntax_t losu_stddef_notnull syn);

losu_stddef_extern void
losu_syntax_parser_check_limit(losu_syntax_t losu_stddef_notnull syn,
                               int32_t nitems, int32_t limit,
                               const char *losu_stddef_notnull errmsg);

/* local variable */

losu_stddef_extern void
losu_syntax_parser_localvar_new(losu_syntax_t losu_stddef_notnull syn,
                                losu_obj_string_t losu_stddef_notnull name,
                                uint16_t i);

losu_stddef_extern void
losu_syntax_parser_locavar_adjust(losu_syntax_t losu_stddef_notnull syn,
                                  int32_t nvar);

losu_stddef_extern void
losu_syntax_parser_locvar_del(losu_syntax_t losu_stddef_notnull syn, int32_t i);

/* exp */

losu_stddef_extern void
losu_syntax_parser_exp(losu_syntax_t losu_stddef_notnull syn);

losu_stddef_extern int32_t losu_syntax_parser_subexp(
    losu_syntax_t losu_stddef_notnull syn,
    losu_syntax_ast_t_exp_t losu_stddef_notnull exp, int32_t left_priority);

/* block chunk */

losu_stddef_extern void
losu_syntax_parser_block(losu_syntax_t losu_stddef_notnull syn);

losu_stddef_extern losu_stddef_bool_t losu_syntax_parser_block_switch(
    losu_syntax_t losu_stddef_notnull syn, losu_stddef_bool_t need_return);

losu_stddef_extern losu_stddef_bool_t
losu_syntax_parser_block_isend(int16_t token);

/* case if */

losu_stddef_extern void
losu_syntax_parser_case_if(losu_syntax_t losu_stddef_notnull syn);

/* case while */
losu_stddef_extern void
losu_syntax_parser_case_while(losu_syntax_t losu_stddef_notnull syn);

/* case for */
losu_stddef_extern void
losu_syntax_parser_case_for(losu_syntax_t losu_stddef_notnull syn);

/* case var */

losu_stddef_extern void
losu_syntax_parser_case_var(losu_syntax_t losu_stddef_notnull syn);

/* case return */

losu_stddef_extern void
losu_syntax_parser_case_return(losu_syntax_t losu_stddef_notnull syn);

/* case break */

losu_stddef_extern void
losu_syntax_parser_case_break(losu_syntax_t losu_stddef_notnull syn);

/* case continue */

losu_stddef_extern void
losu_syntax_parser_case_continue(losu_syntax_t losu_stddef_notnull syn);

/* case yield */

losu_stddef_extern void
losu_syntax_parser_case_yield(losu_syntax_t losu_stddef_notnull syn);

/* structures */

struct losu_syntax_ast_t_exp_t {
  union losu_syntax_ast_t_exp_t_value_t {
    uint16_t cgidx;
    struct losu_syntax_ast_t_exp_t_jump_t {
      int64_t t, f;
    } _jump;
    struct losu_syntax_ast_t_exp_t_upvalue_t {
      void *losu_stddef_nullable upfunc;
      uint16_t upidx;
      uint16_t cgidx;
    } upvalue;
  } value;
  losu_stddef_bool_t type;
};

enum losu_syntax_ast_t_exp_t_type_t {
  losu_syntax_ast_t_exp_t_type_local,
  losu_syntax_ast_t_exp_t_type_index,
  losu_syntax_ast_t_exp_t_type_opindex,
  losu_syntax_ast_t_exp_t_type_upvalue,
  losu_syntax_ast_t_exp_t_type_exper,
};

struct losu_syntax_ast_t {
  int64_t pc, lasttag, jump;

  losu_syntax_ast_t losu_stddef_nullable prev;
  losu_obj_func_t_script_t losu_stddef_nullable script;
  losu_syntax_t losu_stddef_nullable syn;
  losu_vm_t losu_stddef_nullable vm;

  losu_syntax_ast_t_break_t losu_stddef_nullable breaklist;
  losu_syntax_ast_t_continue_t losu_stddef_nullable continuelist;

  int32_t stacklevel;
  uint32_t aloc[losu_vm_ins_lim_locvar];
  struct losu_syntax_ast_t_exp_t upvalue[losu_vm_ins_lim_upvalue];

  uint16_t naloc;
  uint16_t nupvalue;
};

struct losu_syntax_ast_t_break_t {
  losu_syntax_ast_t_break_t losu_stddef_nullable pre;
  int64_t list;
  int32_t stacklevel;
};

struct losu_syntax_ast_t_continue_t {
  int64_t tmppc, startpc;
  losu_syntax_ast_t_continue_t losu_stddef_nullable pre;
  int32_t stacklevel;
};

#endif
