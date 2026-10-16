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

#include "losu/src/syntax/parser/binder.h"
#include "losu/src/syntax/parser/errno.h"
#include "losu/src/syntax/parser/exp.h"

#include <string.h>

#define losu_syntax_parser_binder_opt_pre(name, vmins, l, r) {l, r}

const struct losu_syntax_parser_priority {
  int8_t left, right;
} static priority[] = losu_syntax_parser_binder_operator_t_text;

#undef losu_syntax_parser_binder_opt_pre

losu_stddef_extern void
losu_syntax_parser_exp(losu_syntax_t losu_stddef_notnull syn) {
  assert(syn != NULL);
  struct losu_syntax_ast_t_exp_t exp = {0};
  losu_syntax_parser_subexp(syn, &exp, -1);
  losu_syntax_parser_binder_tostack(syn->ast, &exp);
}

losu_stddef_extern int32_t losu_syntax_parser_subexp(
    losu_syntax_t losu_stddef_notnull syn,
    losu_syntax_ast_t_exp_t losu_stddef_notnull exp, int32_t left_priority) {
  assert(syn != NULL && exp != NULL);

  losu_syntax_parser_binder_oprator_t op, unop;
  unop = losu_syntax_parser_exp_get_unop(syn->tk.token);
  if (unop == SYNOPT_INVALID)
    losu_syntax_parser_exp_case(syn, exp);
  else {
    losu_syntax_parser_next_token(syn);
    losu_syntax_parser_subexp(syn, exp, priority[unop].right);
    losu_syntax_parser_binder_prefix(syn->ast, unop, exp);
  }
  op = losu_syntax_parser_exp_get_binop(syn->tk.token);
  while (op != SYNOPT_INVALID && priority[op].left > left_priority) {
    struct losu_syntax_ast_t_exp_t exp_new = {0};
    int64_t left_priority_new;
    losu_syntax_parser_next_token(syn);
    losu_syntax_parser_binder_infix(syn->ast, op, exp);
    left_priority_new =
        losu_syntax_parser_subexp(syn, &exp_new, priority[op].right);
    losu_syntax_parser_binder_posfix(syn->ast, op, exp, &exp_new);
    op = left_priority_new;
  }
  return op;
}

losu_stddef_extern void
losu_syntax_parser_exp_list(losu_syntax_t losu_stddef_notnull syn) {
  assert(syn != NULL);

  losu_syntax_ast_t losu_stddef_notnull ast = syn->ast;
  struct losu_syntax_ast_t_exp_t exp = {0};
  losu_syntax_parser_subexp(syn, &exp, -1);
  while (syn->tk.token == ',') {
    losu_syntax_parser_binder_tostack(ast, &exp);
    losu_syntax_parser_next_token(syn);
    losu_syntax_parser_subexp(syn, &exp, -1);
  }
  losu_syntax_parser_binder_tostack(ast, &exp);
}

losu_stddef_extern losu_syntax_parser_binder_oprator_t
losu_syntax_parser_exp_get_unop(int16_t tk) {
  switch (tk) {
  case TOKEN_NOT:
    return SYNOPT_NOT;
  case '~':
    return SYNOPT_BIT_NOT;
  case '-':
    return SYNOPT_NEG;
  default:
    return SYNOPT_INVALID;
  }
}

losu_stddef_extern losu_syntax_parser_binder_oprator_t
losu_syntax_parser_exp_get_binop(int16_t tk) {
  switch (tk) {
    /* alu op */
  case '+':
    return SYNOPT_ADD;
  case '-':
    return SYNOPT_SUB;
  case '*':
    return SYNOPT_MUL;
  case '/':
    return SYNOPT_DIV;
  case TOKEN_ODIVI:
    return SYNOPT_DIVI;
  case TOKEN_OPOW:
    return SYNOPT_POW;
  case '%':
    return SYNOPT_MOD;
  case TOKEN_OCONCAT:
    return SYNOPT_CONCAT;
    /* bit op */
  case '&':
    return SYNOPT_BIT_AND;
  case '|':
    return SYNOPT_BIT_OR;
  case '^':
    return SYNOPT_BIT_XOR;
  case TOKEN_OSHL:
    return SYNOPT_BIT_SHL;
  case TOKEN_OSHR:
    return SYNOPT_BIT_SHR;
  case TOKEN_OSHRU:
    return SYNOPT_BIT_SHRU;
    /* func op */
  case TOKEN_OPIPE:
    return SYNOPT_PIPE;
  case TOKEN_OLAMBDA:
    return SYNOPT_LAMBDA;
    /* compare op */
  case TOKEN_ONE:
    return SYNOPT_NE;
  case TOKEN_OEQ:
    return SYNOPT_EQ;
  case '<':
    return SYNOPT_LT;
  case TOKEN_OLE:
    return SYNOPT_LE;
  case '>':
    return SYNOPT_GT;
  case TOKEN_OGE:
    return SYNOPT_GE;

  /* logic op */
  case TOKEN_AND:
    return SYNOPT_AND;
  case TOKEN_OR:
    return SYNOPT_OR;
  default:
    return SYNOPT_INVALID;
  }
}

losu_stddef_extern void
losu_syntax_parser_exp_case(losu_syntax_t losu_stddef_notnull syn,
                            losu_syntax_ast_t_exp_t losu_stddef_notnull exp) {
  assert(syn != NULL && exp != NULL);
  losu_syntax_ast_t losu_stddef_notnull ast = syn->ast;
  switch (syn->tk.token) {
  case TOKEN_NIL: {
    losu_syntax_backend_codegen(ast, INS_PUSHNIL, 1, 0);
    losu_syntax_parser_next_token(syn);
    break;
  }
  case TOKEN_SELF: {
    losu_syntax_backend_codegen(ast, INS_PUSHSELF, 0, 0);
    losu_syntax_parser_next_token(syn);
    break;
  }
  case TOKEN_FLOAT: {
    losu_syntax_backend_codegen(
        ast, INS_PUSHFLOAT,
        losu_syntax_parser_binder_floatvalue(ast, syn->tk.value._float), 0);
    losu_syntax_parser_next_token(syn);
    break;
  }
  case TOKEN_STRING: {
    losu_syntax_backend_codegen(
        ast, INS_PUSHSTRING,
        losu_syntax_parser_binder_stringvalue(ast, syn->tk.value._string), 0);
    losu_syntax_parser_next_token(syn);
    break;
  }
  case TOKEN_META: {
    losu_syntax_backend_codegen(
        ast, INS_PUSHMETA,
        losu_syntax_parser_binder_stringvalue(ast, syn->tk.value._string), 0);
    losu_syntax_parser_next_token(syn);
    break;
  }
  case '{': {
    losu_syntax_parser_exp_wrapper(syn);
    break;
  }
  case '[': {
    losu_syntax_parser_exp_unit_array(syn, syn->linenumber);
    break;
  }
  case TOKEN_IDENTIFY: {
    losu_syntax_parser_exp_loadname(syn, syn->tk.value._string, exp);
    losu_syntax_parser_next_token(syn);
    losu_syntax_parser_exp_loadexp(syn, exp);
    return;
  }
  case '(': {
    losu_syntax_parser_next_token(syn);
    losu_syntax_parser_subexp(syn, exp, -1);
    losu_syntax_parser_check_andnext(syn, ')');
    losu_syntax_parser_exp_loadexp(syn, exp);
    return;
  }
  default: {
    losu_syntax_error(syn, syn->linenumber, ERRNO_INVALID_EXPRESSION);
    return;
  }
  }
  exp->type = losu_syntax_ast_t_exp_t_type_exper;
  exp->value._jump.t = exp->value._jump.f = NO_JUMP;
  losu_syntax_parser_exp_loadexp(syn, exp);
}

losu_stddef_extern void losu_syntax_parser_exp_loadname(
    losu_syntax_t losu_stddef_notnull syn,
    losu_obj_string_t losu_stddef_notnull name,
    losu_syntax_ast_t_exp_t losu_stddef_notnull exp) {
  assert(syn != NULL && name != NULL && exp != NULL);
  uint32_t index = losu_syntax_parser_exp_getvarlevel(syn, name, exp);
  if (index > 0) {
    index = losu_syntax_parser_exp_setvarlevel(syn, syn->ast, name, exp, index);
    exp->type = losu_syntax_ast_t_exp_t_type_upvalue;
    exp->value.upvalue.cgidx = index;
  }
}

losu_stddef_extern uint32_t losu_syntax_parser_exp_getvarlevel(
    losu_syntax_t losu_stddef_notnull syn,
    losu_obj_string_t losu_stddef_notnull name,
    losu_syntax_ast_t_exp_t losu_stddef_notnull exp) {
  assert(syn != NULL && name != NULL && exp != NULL);

  losu_syntax_ast_t losu_stddef_nullable ast = NULL;
  uint32_t lev = 0;
  for (ast = syn->ast; ast; ast = ast->prev) {
    for (int32_t i = ast->naloc - 1; i >= 0; i--) {
      if (name == ast->script->localvar[ast->aloc[i]].name) {
        if (lev) {
          exp->type = losu_syntax_ast_t_exp_t_type_upvalue;
          exp->value.upvalue.upidx = i;
          exp->value.upvalue.upfunc = ast;
          return lev;
        }
        exp->type = losu_syntax_ast_t_exp_t_type_local;
        exp->value.cgidx = i;
        return 0;
      }
    }
  }
  losu_syntax_error(syn, syn->linenumber, ERRNO_UNDEFINED(name));
  return 0;
}

losu_stddef_extern uint32_t losu_syntax_parser_exp_setvarlevel(
    losu_syntax_t losu_stddef_notnull syn,
    losu_syntax_ast_t losu_stddef_notnull ast,
    losu_obj_string_t losu_stddef_notnull name,
    losu_syntax_ast_t_exp_t losu_stddef_notnull exp, uint32_t level) {
  assert(syn != NULL && name != NULL && exp != NULL);
  if (level == 1) {
    for (uint16_t i = 0; i < ast->nupvalue; i++)
      if (ast->upvalue[i].type == losu_syntax_ast_t_exp_t_type_local &&
          ast->upvalue[i].value.cgidx == exp->value.upvalue.upidx)
        return i;
    losu_syntax_parser_check_limit(syn, ast->nupvalue, losu_vm_ins_lim_upvalue,
                                   ERRNO_UPVALUE_OVERFLOW);
    ast->upvalue[ast->nupvalue] = (struct losu_syntax_ast_t_exp_t){
        .type = losu_syntax_ast_t_exp_t_type_local,
        .value.cgidx = exp->value.upvalue.upidx,
    };
    return ast->nupvalue++;
  } else {
    for (uint16_t i = 0; i < ast->nupvalue; i++)
      if (ast->upvalue[i].type == losu_syntax_ast_t_exp_t_type_upvalue &&
          ast->upvalue[i].value.upvalue.upfunc == exp->value.upvalue.upfunc &&
          ast->upvalue[i].value.upvalue.upidx == exp->value.upvalue.upidx)
        return i;
    losu_syntax_parser_check_limit(syn, ast->nupvalue, losu_vm_ins_lim_upvalue,
                                   ERRNO_UPVALUE_OVERFLOW);
    ast->upvalue[ast->nupvalue] = *exp;
    ast->upvalue[ast->nupvalue].value.upvalue.cgidx =
        losu_syntax_parser_exp_setvarlevel(syn, ast->prev, name, exp,
                                           level - 1);
    return ast->nupvalue++;
  }
}

losu_stddef_extern void losu_syntax_parser_exp_loadexp(
    losu_syntax_t losu_stddef_notnull syn,
    losu_syntax_ast_t_exp_t losu_stddef_notnull exp) {
  assert(syn != NULL && exp != NULL);

  losu_syntax_ast_t losu_stddef_notnull ast = syn->ast;
  for (;;) {
    switch (syn->tk.token) {
    case '.': {
      losu_syntax_parser_binder_tostack(ast, exp);
      losu_syntax_parser_next_token(syn);
      if (syn->tk.token != '(') {
        uint32_t nameidx = losu_syntax_parser_binder_stringvalue(
            ast, losu_syntax_parser_check_isidentify_andnext(syn));
        losu_syntax_backend_codegen(ast, INS_PUSHSTRING, nameidx, 0);
        exp->type = losu_syntax_ast_t_exp_t_type_index;
      } else {
        int32_t line = syn->linenumber;
        losu_syntax_parser_next_token(syn);
        switch (syn->tk.token) {
        case '&': {
          losu_syntax_parser_binder_tostack(ast, exp);
          losu_syntax_parser_next_token(syn);
          losu_obj_string_t type_name =
              losu_syntax_parser_check_isidentify_andnext(syn);
          losu_syntax_backend_codegen(
              ast, INS_TYPECOVERT,
              losu_syntax_parser_binder_stringvalue(ast, type_name), 0);
          losu_syntax_parser_check_match_andnext(syn, '(', ')', line);
          break;
        }
        case '?': {
          losu_syntax_parser_binder_tostack(ast, exp);
          losu_syntax_parser_next_token(syn);
          losu_obj_string_t type_name =
              losu_syntax_parser_check_isidentify_andnext(syn);
          losu_syntax_backend_codegen(
              ast, INS_TYPEGUARD,
              losu_syntax_parser_binder_stringvalue(ast, type_name), 0);
          losu_syntax_parser_check_match_andnext(syn, '(', ')', line);
          break;
        }
        case TOKEN_IDENTIFY: {
          losu_syntax_parser_binder_tostack(ast, exp);
          losu_obj_string_t type_name =
              losu_syntax_parser_check_isidentify_andnext(syn);
          losu_syntax_backend_codegen(
              ast, INS_TYPEASSERT,
              losu_syntax_parser_binder_stringvalue(ast, type_name), 0);
          losu_syntax_parser_check_match_andnext(syn, '(', ')', line);
          break;
        }
        case ')': {
          losu_syntax_parser_binder_tostack(ast, exp);
          losu_syntax_backend_codegen(syn->ast, INS_TYPEGET, 0, 0);
          losu_syntax_parser_next_token(syn);
          break;
        }
        default: {
          losu_syntax_error(syn, syn->linenumber, ERRNO_UNEXPECTED_TYPE);
        }
        }
        exp->type = losu_syntax_ast_t_exp_t_type_exper;
        exp->value._jump.t = exp->value._jump.f = NO_JUMP;
      }
      break;
    }
    case TOKEN_OINDEX: {
      losu_syntax_parser_binder_tostack(ast, exp);
      losu_syntax_parser_next_token(syn);
      uint32_t line = syn->linenumber;
      losu_syntax_parser_check_andnext(syn, '[');
      losu_syntax_parser_exp(syn);
      losu_syntax_parser_check_match_andnext(syn, '[', ']', line);
      exp->type = losu_syntax_ast_t_exp_t_type_opindex;
      break;
    }
    case '[': {
      uint32_t line = syn->linenumber;
      losu_syntax_parser_binder_tostack(ast, exp);
      losu_syntax_parser_next_token(syn);
      losu_syntax_parser_exp(syn);
      losu_syntax_parser_check_match_andnext(syn, '[', ']', line);
      exp->type = losu_syntax_ast_t_exp_t_type_index;
      break;
    }
    case '(':
    case '{': {
      losu_syntax_parser_binder_tostack(ast, exp);
      losu_syntax_parser_exp_func_args(syn, 0);
      exp->type = losu_syntax_ast_t_exp_t_type_exper;
      exp->value._jump.t = exp->value._jump.f = NO_JUMP;
      break;
    }
    default: {
      return;
    }
    }
  }
}

losu_stddef_extern void
losu_syntax_parser_exp_func_args(losu_syntax_t losu_stddef_notnull syn,
                                 losu_stddef_bool_t self) {
  assert(syn != NULL);
  losu_syntax_ast_t losu_stddef_notnull ast = syn->ast;
  int32_t slev = ast->stacklevel - self - 1;

  switch (syn->tk.token) {
  case '(': {
    uint32_t line = syn->linenumber;
    losu_syntax_parser_next_token(syn);
    if (syn->tk.token != ')')
      losu_syntax_parser_exp_list(syn);
    losu_syntax_parser_check_match_andnext(syn, '(', ')', line);
    break;
  }
  case '{': {
    losu_syntax_parser_exp_wrapper(syn);
    break;
  }
  }
  while (syn->tk.token == '{')
    losu_syntax_parser_exp_wrapper(syn);
  ast->stacklevel = slev;
  losu_syntax_backend_codegen(ast, INS_CALL, slev, 0);
}

losu_stddef_extern void
losu_syntax_parser_exp_wrapper(losu_syntax_t losu_stddef_notnull syn) {
  uint32_t line = syn->linenumber;
  losu_syntax_parser_next_ahead_token(syn);
  if (syn->tkahead.token == TOKEN_ASSIGN) {
    losu_syntax_parser_next_token(syn);
    losu_syntax_parser_next_token(syn);
    losu_syntax_parser_exp_lambda(syn, line, 0);
  } else {
    losu_syntax_parser_next_token(syn);
    losu_syntax_parser_next_ahead_token(syn);
    if (syn->tk.token == TOKEN_STRING && syn->tkahead.token == ':')
      losu_syntax_parser_exp_unit_map(syn, line);
    else if (syn->tk.token == TOKEN_FLOAT && syn->tkahead.token == ':')
      losu_syntax_parser_exp_unit_map(syn, line);
    else if (syn->tk.token == TOKEN_IDENTIFY && syn->tkahead.token == ':')
      losu_syntax_parser_exp_unit_map(syn, line);
    else if (syn->tk.token == TOKEN_META && syn->tkahead.token == ':')
      losu_syntax_parser_exp_unit_map(syn, line);
    else if (syn->tk.token == '[')
      losu_syntax_parser_exp_unit_map(syn, line);
    else if (syn->tk.token == TOKEN_DOT || syn->tk.token == ';')
      losu_syntax_parser_exp_unit_map(syn, line);
    else if (syn->tk.token == '}')
      losu_syntax_parser_exp_unit_map(syn, line);
    else /* lambda */
    {
      if (syn->tk.token == TOKEN_IDENTIFY &&
          (syn->tkahead.token == ',' || syn->tkahead.token == TOKEN_ASSIGN))
        losu_syntax_parser_exp_lambda(syn, line, 1);
      else
        losu_syntax_parser_exp_lambda(syn, line, 0);
    }
  }
}

losu_stddef_extern void
losu_syntax_parser_exp_unit_array(losu_syntax_t losu_stddef_notnull syn,
                                  uint32_t line) {
  assert(syn != NULL);

  losu_syntax_ast_t losu_stddef_notnull ast = syn->ast;
  int64_t pc = losu_syntax_backend_codegen(ast, INS_CREATEUNIT, 0, 0);
  uint32_t niss = 0;
  losu_syntax_parser_check_andnext(syn, '[');
  if (syn->tk.token != ']') {
    niss = 1;
    losu_syntax_parser_exp(syn);
    while (syn->tk.token == ',') {
      losu_syntax_parser_next_token(syn);
      if (syn->tk.token == ']')
        break;
      losu_syntax_parser_exp(syn);
      niss++;
      if (niss / losu_vm_ins_lim_setarray > losu_vm_ins_max_A)
        losu_syntax_error(syn, line, ERRNO_ARRAY_OVERFLOW);
      if (niss % losu_vm_ins_lim_setarray == 0)
        losu_syntax_backend_codegen(ast, INS_SETARRAY, losu_vm_ins_lim_setarray,
                                    niss / losu_vm_ins_lim_setarray - 1);
    }
    losu_syntax_backend_codegen(ast, INS_SETARRAY,
                                niss % losu_vm_ins_lim_setarray,
                                niss / losu_vm_ins_lim_setarray);
  }
  losu_syntax_parser_check_match_andnext(syn, '[', ']', line);
  if (niss > losu_vm_ins_max_U)
    niss = losu_vm_ins_max_U;
  losu_vm_ins_cgSetU(ast->script->code[pc], niss);
}

static inline void
_inline_unit_property(losu_stddef_notnull losu_syntax_t syn,
                      losu_stddef_notnull losu_syntax_ast_t ast);

losu_stddef_extern void
losu_syntax_parser_exp_unit_map(losu_syntax_t losu_stddef_notnull syn,
                                uint32_t line) {
  assert(syn != NULL);
  losu_syntax_ast_t losu_stddef_notnull ast = syn->ast;

  int64_t pc = losu_syntax_backend_codegen(ast, INS_CREATEUNIT, 0, 0);
  uint64_t ntiem_all = 0;
  uint64_t nitem_json = 0;
  uint64_t nitem_array = 0;
  uint64_t nitem_export = 0;

  if (syn->tk.token != '}') {
    /* if has json segment */
    if (syn->tk.token != TOKEN_DOT && syn->tk.token != ';') {
      nitem_json = 1;
      _inline_unit_property(syn, ast);
      while (syn->tk.token == ',') {
        losu_syntax_parser_next_token(syn);
        if (syn->tk.token == '}' || syn->tk.token == TOKEN_DOT ||
            syn->tk.token == ';')
          break;
        _inline_unit_property(syn, ast);
        nitem_json++;
        if (nitem_json % losu_vm_ins_lim_setmap == 0)
          losu_syntax_backend_codegen(ast, INS_SETMAP, losu_vm_ins_lim_setmap,
                                      0);
      }
      losu_syntax_backend_codegen(ast, INS_SETMAP,
                                  nitem_json % losu_vm_ins_lim_setmap, 0);
      ntiem_all += nitem_json;
    }
    /* if has array segment */
    if (syn->tk.token == TOKEN_DOT) {
      losu_syntax_parser_next_token(syn);
      if (syn->tk.token != '}' && syn->tk.token != ';') {
        nitem_array = 1;
        losu_syntax_parser_exp(syn);
        while (syn->tk.token == ',') {
          losu_syntax_parser_next_token(syn);
          if (syn->tk.token == '}' || syn->tk.token == ';')
            break;
          losu_syntax_parser_exp(syn);
          nitem_array++;
          if (nitem_array / losu_vm_ins_lim_setarray > losu_vm_ins_max_A)
            losu_syntax_error(syn, line, ERRNO_ARRAY_OVERFLOW);
          if (nitem_array % losu_vm_ins_lim_setarray == 0)
            losu_syntax_backend_codegen(
                ast, INS_SETARRAY, losu_vm_ins_lim_setarray,
                nitem_array / losu_vm_ins_lim_setarray - 1);
        }
        losu_syntax_backend_codegen(ast, INS_SETARRAY,
                                    nitem_array % losu_vm_ins_lim_setarray,
                                    nitem_array / losu_vm_ins_lim_setarray);
        ntiem_all += nitem_array;
      }
    }
    /* if has export */
    if (syn->tk.token == ';') {
      losu_syntax_parser_next_token(syn);
      if (syn->tk.token != '}') {
        nitem_export = 1;
        losu_obj_string_t name =
            losu_syntax_parser_check_isidentify_andnext(syn);
        struct losu_syntax_ast_t_exp_t exp = {0};
        losu_syntax_backend_codegen(
            ast, INS_PUSHSTRING,
            losu_syntax_parser_binder_stringvalue(ast, name), 0);
        losu_syntax_parser_exp_loadname(syn, name, &exp);
        losu_syntax_parser_binder_tostack(ast, &exp);
        while (syn->tk.token == ',') {
          losu_syntax_parser_next_token(syn);
          if (syn->tk.token == '}')
            break;
          memset(&exp, 0, sizeof(exp));
          name = losu_syntax_parser_check_isidentify_andnext(syn);
          losu_syntax_backend_codegen(
              ast, INS_PUSHSTRING,
              losu_syntax_parser_binder_stringvalue(ast, name), 0);
          losu_syntax_parser_exp_loadname(syn, name, &exp);
          losu_syntax_parser_binder_tostack(ast, &exp);
          nitem_export++;
          if (nitem_export % losu_vm_ins_lim_setmap == 0)
            losu_syntax_backend_codegen(ast, INS_SETMAP, losu_vm_ins_lim_setmap,
                                        0);
        }
        losu_syntax_backend_codegen(ast, INS_SETMAP,
                                    nitem_export % losu_vm_ins_lim_setmap, 0);
        ntiem_all += nitem_export;
      }
    }
  }
  /* end */
  losu_syntax_parser_check_match_andnext(syn, '{', '}', line);
  if (ntiem_all > losu_vm_ins_max_U)
    ntiem_all = losu_vm_ins_max_U;
  losu_vm_ins_cgSetU(ast->script->code[pc], ntiem_all);
}

static inline void
_inline_unit_property(losu_stddef_notnull losu_syntax_t syn,
                      losu_stddef_notnull losu_syntax_ast_t ast) {
  switch (syn->tk.token) {
  case TOKEN_IDENTIFY:
  case TOKEN_STRING: {
    losu_syntax_backend_codegen(
        ast, INS_PUSHSTRING,
        losu_syntax_parser_binder_stringvalue(ast, syn->tk.value._string), 0);
    losu_syntax_parser_next_token(syn);
    break;
  }
  case TOKEN_FLOAT: {
    losu_syntax_backend_codegen(
        ast, INS_PUSHFLOAT,
        losu_syntax_parser_binder_floatvalue(ast, syn->tk.value._float), 0);
    losu_syntax_parser_next_token(syn);
    break;
  }
  case TOKEN_META: {
    losu_syntax_backend_codegen(
        ast, INS_PUSHMETA,
        losu_syntax_parser_binder_stringvalue(ast, syn->tk.value._string), 0);
    losu_syntax_parser_next_token(syn);
    break;
  }
  case '[': {
    uint32_t line = syn->linenumber;
    losu_syntax_parser_next_token(syn);
    losu_syntax_parser_exp(syn);
    losu_syntax_parser_check_match_andnext(syn, '[', ']', line);
    break;
  }
  default: {
    losu_syntax_error(syn, syn->linenumber, ERRNO_UNIT_PROPERTY);
  }
  }
  losu_syntax_parser_check_andnext(syn, ':');
  losu_syntax_parser_exp(syn);
}

losu_stddef_extern void
losu_syntax_parser_exp_lambda(losu_syntax_t losu_stddef_notnull syn,
                              uint32_t line, losu_stddef_bool_t hasargs) {
  assert(syn != NULL);
  struct losu_syntax_ast_t new_ast = {0};
  losu_syntax_parser_ast_newfunc(syn, &new_ast);

  if (hasargs)
    losu_syntax_parser_exp_lambda_argdef(syn);
  while (!losu_syntax_parser_block_isend(syn->tk.token))
    if (losu_syntax_parser_block_switch(syn, 1))
      break;
  losu_syntax_parser_ast_delfunc(syn);

  losu_syntax_parser_check_match_andnext(syn, '{', '}', line);

  losu_syntax_ast_t ast = syn->ast;
  losu_obj_func_t_script_t spt = ast->script;

  for (uint16_t i = 0; i < new_ast.nupvalue; i++)
    losu_syntax_parser_binder_tostack(ast, new_ast.upvalue + i);

  losu_vm_mem_vector_grow(syn->vm, spt->lcscript, spt->nlcscript, 1,
                          losu_obj_func_t_script_t, losu_vm_ins_max_A,
                          ERRNO_LAMBDA_OVERFLOW);
  spt->lcscript[spt->nlcscript] = new_ast.script;
  losu_syntax_backend_codegen(ast, INS_PUSHFUNCTION, new_ast.nupvalue,
                              spt->nlcscript);
  spt->nlcscript++;
}

losu_stddef_extern void
losu_syntax_parser_exp_lambda_argdef(losu_syntax_t losu_stddef_notnull syn) {
  assert(syn != NULL);
  int32_t np = 0;
  do {
    losu_syntax_parser_localvar_new(
        syn, losu_syntax_parser_check_isidentify_andnext(syn), np++);
  } while ((syn->tk.token == ',') ? (losu_syntax_parser_next_token(syn), 1)
                                  : 0);
  losu_syntax_parser_check_andnext(syn, TOKEN_ASSIGN);
  losu_syntax_parser_locavar_adjust(syn, np);
  syn->ast->script->nargs = syn->ast->naloc;
  losu_syntax_backend_stack_adjust(syn->ast, syn->ast->naloc);
}
