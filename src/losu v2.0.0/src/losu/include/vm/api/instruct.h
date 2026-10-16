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

#ifndef LOSU_INC_VM_API_INSTRUCT_H
#define LOSU_INC_VM_API_INSTRUCT_H

typedef uint8_t losu_vm_ins_OP_t, losu_vm_ins_B_t;
typedef uint32_t losu_vm_ins_U_t;
typedef uint16_t losu_vm_ins_A_t;
typedef int32_t losu_vm_ins_S_t;

#define losu_vm_ins_size (sizeof(losu_stddef_vmins_t))
#define losu_vm_ins_size_OP 8
#define losu_vm_ins_size_B 8
#define losu_vm_ins_size_A 16
#define losu_vm_ins_size_U 24
#define losu_vm_ins_size_S 24

#define losu_vm_ins_pos_OP 0
#define losu_vm_ins_pos_U 8
#define losu_vm_ins_pos_S 8
#define losu_vm_ins_pos_B 8
#define losu_vm_ins_pos_A 16

#define losu_vm_ins_max_U (((losu_stddef_vmins_t)(1) << losu_vm_ins_size_U) - 1)
#define losu_vm_ins_max_S (losu_vm_ins_max_U >> 1)
#define losu_vm_ins_max_B (((losu_stddef_vmins_t)(1) << losu_vm_ins_size_B) - 1)
#define losu_vm_ins_max_A (((losu_stddef_vmins_t)(1) << losu_vm_ins_size_A) - 1)

#define losu_vm_ins_lim_stack 1024
#define losu_vm_ins_lim_locvar 255
#define losu_vm_ins_lim_upvalue 255
#define losu_vm_ins_lim_setarray 255
#define losu_vm_ins_lim_setmap (losu_vm_ins_lim_setarray / 2)
#define losu_vm_ins_lim_strlen SIZE_MAX - 1

#define losu_vm_ins_cgMask0(n, p) (~((((losu_stddef_vmins_t)1 << n) - 1) << p))

#define losu_vm_ins_cgNewOP(op) ((losu_stddef_vmins_t)(op))
#define losu_vm_ins_cgGetOP(i) ((losu_vm_ins_OP_t)(i))
#define losu_vm_ins_cgSetOP(i, op)                                             \
  ((i) = (((losu_stddef_vmins_t)(i) &                                          \
           losu_vm_ins_cgMask0(losu_vm_ins_size_OP, 0)) |                      \
          (losu_stddef_vmins_t)(op)))

#define losu_vm_ins_cgNewU(op, u)                                              \
  ((losu_stddef_vmins_t)(op) | ((losu_stddef_vmins_t)(u) << losu_vm_ins_pos_U))
#define losu_vm_ins_cgGetU(i)                                                  \
  ((losu_vm_ins_U_t)((losu_stddef_vmins_t)(i) >> losu_vm_ins_pos_U))
#define losu_vm_ins_cgSetU(i, u)                                               \
  ((i) = (((i) & losu_vm_ins_cgMask0(losu_vm_ins_size_U, losu_vm_ins_pos_U)) | \
          ((losu_stddef_vmins_t)(u) << losu_vm_ins_pos_U)))

#define losu_vm_ins_cgNewS(op, s)                                              \
  (losu_vm_ins_cgNewU((op), (s) + losu_vm_ins_max_S))
#define losu_vm_ins_cgGetS(i)                                                  \
  ((losu_vm_ins_S_t)(losu_vm_ins_cgGetU(i) - losu_vm_ins_max_S))
#define losu_vm_ins_cgSetS(i, s)                                               \
  (losu_vm_ins_cgSetU(i, (s) + losu_vm_ins_max_S))

#define losu_vm_ins_cgNewBA(op, b, a)                                          \
  (((losu_stddef_vmins_t)(op)) |                                               \
   ((losu_stddef_vmins_t)(a) << losu_vm_ins_pos_A) |                           \
   ((losu_stddef_vmins_t)(b) << losu_vm_ins_pos_B))
#define losu_vm_ins_cgGetA(i) (((losu_vm_ins_A_t)((i) >> losu_vm_ins_pos_A)))
#define losu_vm_ins_cgSetA(i, a)                                               \
  ((i) = (((i) & losu_vm_ins_cgMask0(losu_vm_ins_size_A, losu_vm_ins_pos_A)) | \
          ((losu_stddef_vmins_t)(a) << losu_vm_ins_pos_A)))
#define losu_vm_ins_cgGetB(i) ((losu_vm_ins_B_t)(((i) >> losu_vm_ins_pos_B)))
#define losu_vm_ins_cgSetB(i, b)                                               \
  ((i) = (((i) & losu_vm_ins_cgMask0(losu_vm_ins_size_B, losu_vm_ins_pos_B)) | \
          ((losu_stddef_vmins_t)(b) << losu_vm_ins_pos_B)))

#define losu_vm_ins_pretext(name, mode, stack) name
#define V 0
#define losu_vm_ins_text                                                       \
  {                                                                            \
      /* op push pop */                                                        \
      losu_vm_ins_pretext(INS_END, INS_MODE_OP, 0),    /* iO   0   0 */        \
      losu_vm_ins_pretext(INS_RETURN, INS_MODE_IU, 0), /* iU   0   0 */        \
      losu_vm_ins_pretext(INS_CALL, INS_MODE_IU, 1),   /* iU   1   0 */        \
                                                                               \
      losu_vm_ins_pretext(INS_POP, INS_MODE_IU, V),        /* iU   V   0 */    \
      losu_vm_ins_pretext(INS_PUSHNILJMP, INS_MODE_OP, 0), /* iO   0   0 */    \
                                                                               \
      losu_vm_ins_pretext(INS_PUSHNIL, INS_MODE_IU, V),      /* iU   V   0 */  \
      losu_vm_ins_pretext(INS_PUSHFUNCTION, INS_MODE_BA, V), /* iBA  V   0 */  \
      losu_vm_ins_pretext(INS_PUSHSTRING, INS_MODE_IU, 1),   /* iU   1   0 */  \
      losu_vm_ins_pretext(INS_PUSHMETA, INS_MODE_IU, 1),     /* iU   1   0 */  \
      losu_vm_ins_pretext(INS_PUSHFLOAT, INS_MODE_IU, 1),    /* iU   1   0 */  \
      losu_vm_ins_pretext(INS_PUSHTOP, INS_MODE_OP, 1),      /* iO   1   0 */  \
      losu_vm_ins_pretext(INS_PUSHSELF, INS_MODE_OP, 1),     /* iO   1   0 */  \
      losu_vm_ins_pretext(INS_PUSHCAPI, INS_MODE_IU, 1),     /* iU   1   0 */  \
                                                                               \
      losu_vm_ins_pretext(INS_GETLOCAL, INS_MODE_IU, 1),    /* iU   1   0 */   \
      losu_vm_ins_pretext(INS_SETLOCAL, INS_MODE_IU, -1),   /* iU   0   1 */   \
      losu_vm_ins_pretext(INS_GETUPVALUE, INS_MODE_IU, 1),  /* iU   1   0 */   \
      losu_vm_ins_pretext(INS_SETUPVALUE, INS_MODE_IU, -1), /* iU   0   1 */   \
                                                                               \
      losu_vm_ins_pretext(INS_CREATEUNIT, INS_MODE_IU, 1), /* iU   1   0 */    \
      losu_vm_ins_pretext(INS_SETUNIT, INS_MODE_IU, -3),   /* iU   0   3 */    \
      losu_vm_ins_pretext(INS_GETUNIT, INS_MODE_IU, -1),   /* iU   1   2 */    \
      losu_vm_ins_pretext(INS_SETARRAY, INS_MODE_BA, V),   /* iBA  V   0 */    \
      losu_vm_ins_pretext(INS_SETMAP, INS_MODE_IU, V),     /* iU   V   0 */    \
                                                                               \
      /* + - * / /% ** % .. */                                                 \
      losu_vm_ins_pretext(INS_ADD, INS_MODE_OP, -1),    /* iO   1   2 */       \
      losu_vm_ins_pretext(INS_SUB, INS_MODE_OP, -1),    /* iO   1   2 */       \
      losu_vm_ins_pretext(INS_MULT, INS_MODE_OP, -1),   /* iO   1   2 */       \
      losu_vm_ins_pretext(INS_DIV, INS_MODE_OP, -1),    /* iO   1   2 */       \
      losu_vm_ins_pretext(INS_DIVI, INS_MODE_OP, -1),   /* iO   1   2 */       \
      losu_vm_ins_pretext(INS_POW, INS_MODE_OP, -1),    /* iO   1   2 */       \
      losu_vm_ins_pretext(INS_MOD, INS_MODE_OP, -1),    /* iO   1   2 */       \
      losu_vm_ins_pretext(INS_CONCAT, INS_MODE_OP, -1), /* iO   1   2 */       \
                                                                               \
      /* & ｜ ^ << >> >>> */                                                   \
      losu_vm_ins_pretext(INS_BIT_AND, INS_MODE_OP, -1),  /* iO   1   2 */     \
      losu_vm_ins_pretext(INS_BIT_OR, INS_MODE_OP, -1),   /* iO   1   2 */     \
      losu_vm_ins_pretext(INS_BIT_XOR, INS_MODE_OP, -1),  /* iO   1   2 */     \
      losu_vm_ins_pretext(INS_BIT_SHL, INS_MODE_OP, -1),  /* iO   1   2 */     \
      losu_vm_ins_pretext(INS_BIT_SHR, INS_MODE_OP, -1),  /* iO   1   2 */     \
      losu_vm_ins_pretext(INS_BIT_SHRU, INS_MODE_OP, -1), /* iO   1   2 */     \
                                                                               \
      /* :> ~> */                                                              \
      losu_vm_ins_pretext(INS_PIPE, INS_MODE_OP, -1),   /* iO   1   2 */       \
      losu_vm_ins_pretext(INS_LAMBDA, INS_MODE_OP, -1), /* iO   1   2 */       \
                                                                               \
      /* - not ~ */                                                            \
      losu_vm_ins_pretext(INS_NEG, INS_MODE_OP, 0),     /* iO   1   1 */       \
      losu_vm_ins_pretext(INS_NOT, INS_MODE_OP, 0),     /* iO   1   1 */       \
      losu_vm_ins_pretext(INS_BIT_NOT, INS_MODE_OP, 0), /* iO   1   1 */       \
                                                                               \
      losu_vm_ins_pretext(INS_JMPNE, INS_MODE_IS, -2), /* iS   0   2 */        \
      losu_vm_ins_pretext(INS_JMPEQ, INS_MODE_IS, -2), /* iS   0   2 */        \
      losu_vm_ins_pretext(INS_JMPLT, INS_MODE_IS, -2), /* iS   0   2 */        \
      losu_vm_ins_pretext(INS_JMPLE, INS_MODE_IS, -2), /* iS   0   2 */        \
      losu_vm_ins_pretext(INS_JMPGT, INS_MODE_IS, -2), /* iS   0   2 */        \
      losu_vm_ins_pretext(INS_JMPGE, INS_MODE_IS, -2), /* iS   0   2 */        \
                                                                               \
      losu_vm_ins_pretext(INS_JMPT, INS_MODE_IS, -1),   /* iS   0   1 */       \
      losu_vm_ins_pretext(INS_JMPF, INS_MODE_IS, -1),   /* iS   0   1 */       \
      losu_vm_ins_pretext(INS_JMPONT, INS_MODE_IS, -1), /* iS   0   1 */       \
      losu_vm_ins_pretext(INS_JMPONF, INS_MODE_IS, -1), /* iS   0   1 */       \
      losu_vm_ins_pretext(INS_JMP, INS_MODE_IS, 0),     /* iS   0   0 */       \
                                                                               \
      losu_vm_ins_pretext(INS_FORPREP, INS_MODE_IS, 0),    /* iS   0   0 */    \
      losu_vm_ins_pretext(INS_FORNEXT, INS_MODE_IS, -3),   /* iS   0   3 */    \
      losu_vm_ins_pretext(INS_FORINPREP, INS_MODE_IS, 2),  /* iS   2   0 */    \
      losu_vm_ins_pretext(INS_FORINNEXT, INS_MODE_IS, -3), /* iS   0   3 */    \
                                                                               \
      losu_vm_ins_pretext(INS_YIELD, INS_MODE_OP, 0), /* iO   0   0 */         \
                                                                               \
      losu_vm_ins_pretext(INS_IMPORT, INS_MODE_IU, 1),       /* iU   1   0 */  \
      losu_vm_ins_pretext(INS_IMPORT_CACHE, INS_MODE_IU, 0), /* iU   0   0 */  \
                                                                               \
      losu_vm_ins_pretext(INS_TYPEASSERT, INS_MODE_IU, 0), /* iU   0   0 */    \
      losu_vm_ins_pretext(INS_TYPEGUARD, INS_MODE_IU, 0),  /* iU   0   0 */    \
      losu_vm_ins_pretext(INS_TYPECOVERT, INS_MODE_IU, 0), /* iU   0   0 */    \
      losu_vm_ins_pretext(INS_TYPEGET, INS_MODE_IU, 0),    /* iU   0   0 */    \
                                                                               \
      losu_vm_ins_pretext(INS_TOTAL_NUMBER, INS_MODE_OP, 0),                   \
  }

#define losu_vm_ins_isjmp(op) ((INS_JMPNE <= (op) && (op) <= INS_JMP))

enum losu_vm_ins_enum_t losu_vm_ins_text;

#undef V
#undef losu_vm_ins_pretext

enum losu_vm_ins_mode_enum_t {
  INS_MODE_OP,
  INS_MODE_IU,
  INS_MODE_IS,
  INS_MODE_BA,
};

#endif
