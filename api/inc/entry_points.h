/*
 * Copyright (c) 2016, ARM Limited, All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __UVISOR_API_ENTRY_POINTS_H__
#define __UVISOR_API_ENTRY_POINTS_H__

#include "api/inc/error.h"
#include <stdint.h>

/* We re-define this symbol from the CMSIS headers so that we do not need to
 * include the compiler-specific header file. */
static UVISOR_FORCEINLINE uint32_t __get_control(void)
{
#if defined __CC_ARM
  register uint32_t result asm("control");
  return result;
#elif (defined __ARMCC_VERSION && __ARMCC_VERSION >= 6010050) || defined __GNUC__
  uint32_t result;
  asm volatile ("mrs %0, control" : "=r" (result));
  return result;
#endif /* Compiler-specific __get_CONTROL() implementation. */
}

/* We re-define this symbol from the CMSIS headers so that we do not need to
 * include the hardware-specific header file. */
#define __CONTROL_nPRIV_Msk (0x1UL)

/* Returns true if the code is running from NP mode (S or NS). */
/* Note: This macro works in both the ARMv7-M and the ARMv8-M architectures. */
#define CALLING_FROM_NP() (__get_control() & __CONTROL_nPRIV_Msk)

/* Utility macro to generate a function signature from a list of types. */
#define __UVISOR_ENTRY_POINT_SIGNATURE_ARGS(...) \
     __UVISOR_MACRO_SELECT(_0, ##__VA_ARGS__, __UVISOR_ENTRY_POINT_SIGNATURE_ARGS4, \
                                              __UVISOR_ENTRY_POINT_SIGNATURE_ARGS3, \
                                              __UVISOR_ENTRY_POINT_SIGNATURE_ARGS2, \
                                              __UVISOR_ENTRY_POINT_SIGNATURE_ARGS1, \
                                              __UVISOR_ENTRY_POINT_SIGNATURE_ARGS0)(__VA_ARGS__)
#define __UVISOR_ENTRY_POINT_SIGNATURE_ARGS0() \
    void
#define __UVISOR_ENTRY_POINT_SIGNATURE_ARGS1(type_a1) \
    type_a1 a1
#define __UVISOR_ENTRY_POINT_SIGNATURE_ARGS2(type_a1, type_a2) \
    type_a1 a1, type_a2 a2
#define __UVISOR_ENTRY_POINT_SIGNATURE_ARGS3(type_a1, type_a2, type_a3) \
    type_a1 a1, type_a2 a2, type_a3 a3
#define __UVISOR_ENTRY_POINT_SIGNATURE_ARGS4(type_a1, type_a2, type_a3, type_a4) \
    type_a1 a1, type_a2 a2, type_a3 a3, type_a4 a4

/* Utility macro to generate a comma if needed. */
#define __UVISOR_ENTRY_POINT_COMMA(...) \
     __UVISOR_MACRO_SELECT(_0, ##__VA_ARGS__, __UVISOR_ENTRY_POINT_COMMA1, \
                                              __UVISOR_ENTRY_POINT_COMMA1, \
                                              __UVISOR_ENTRY_POINT_COMMA1, \
                                              __UVISOR_ENTRY_POINT_COMMA1, \
                                              __UVISOR_ENTRY_POINT_COMMA0)()
#define __UVISOR_ENTRY_POINT_COMMA0()
#define __UVISOR_ENTRY_POINT_COMMA1() ,

/* Utility macro to generate a function call from a list of types. */
#define __UVISOR_ENTRY_POINT_CALL_ARGS(...) \
     __UVISOR_MACRO_SELECT(_0, ##__VA_ARGS__, __UVISOR_ENTRY_POINT_CALL_ARGS4, \
                                              __UVISOR_ENTRY_POINT_CALL_ARGS3, \
                                              __UVISOR_ENTRY_POINT_CALL_ARGS2, \
                                              __UVISOR_ENTRY_POINT_CALL_ARGS1, \
                                              __UVISOR_ENTRY_POINT_CALL_ARGS0)(__VA_ARGS__)
#define __UVISOR_ENTRY_POINT_CALL_ARGS0()
#define __UVISOR_ENTRY_POINT_CALL_ARGS1(type_a1)                            a1
#define __UVISOR_ENTRY_POINT_CALL_ARGS2(type_a1, type_a2)                   a1, a2
#define __UVISOR_ENTRY_POINT_CALL_ARGS3(type_a1, type_a2, type_a3)          a1, a2, a3
#define __UVISOR_ENTRY_POINT_CALL_ARGS4(type_a1, type_a2, type_a3, type_a4) a1, a2, a3, a4

/* Utility macro to expand a list of arguments before passing them to the SVC
 * macro. */
#define __UVISOR_ENTRY_POINT_EXPAND_SVC(function, ...) UVISOR_SVC(function, ##__VA_ARGS__)

/* Entry points are defined based on the architecture:
 *     - On ARMv7-M, the low privilege is the NP state, the high privilege is
 *       the P state.
 *     - On ARMv8-M, the low privilege is the NS side, the high privilege is the
 *       S side. In particular, uVisor entry points can only be accessed in
 *       P mode in the S side, and in both modes in the NS side. */
#if defined(ARCH_MPU_ARMv8M)

#include "arm_cmse.h"

/* Returns true if the code is running from NS mode (P or NP). */
#define CALLING_FROM_NS() (cmse_nonsecure_caller())

/* Attribute that tells the compiler to generate a veneer wrapper for a function
 * in ARMv8-M. */
#define UVISOR_ENTRY_POINT __attribute__((cmse_nonsecure_entry))

/**/
#define UVISOR_ENTRY_POINT_FROM_LOW_PRIVILEGE(return_type, function_name, ...) \
    return_type UVISOR_EXTERN UVISOR_ENTRY_POINT function_name(__UVISOR_ENTRY_POINT_SIGNATURE_ARGS(__VA_ARGS__)) \
    { \
        if (CALLING_FROM_NS()) { \
            if (CALLING_FROM_NP()) { \
                return (return_type) __UVISOR_ENTRY_POINT_EXPAND_SVC( \
                    function_name __UVISOR_ENTRY_POINT_COMMA(__VA_ARGS__) __UVISOR_ENTRY_POINT_CALL_ARGS(__VA_ARGS__) \
                ); \
            } else { \
                return function_name(__UVISOR_ENTRY_POINT_CALL_ARGS(__VA_ARGS__)); \
            } \
        } else { \
            /* FIXME: Error */ \
            return (return_type) 0; \
        } \
    } \

/**/
#define UVISOR_ENTRY_POINT_FROM_HIGH_PRIVILEGE(return_type, function_name, ...) \
    return_type UVISOR_EXTERN UVISOR_ENTRY_POINT function_name(__UVISOR_ENTRY_POINT_SIGNATURE_ARGS(__VA_ARGS__)) \
    { \
        if (CALLING_FROM_NS()) { \
            if (CALLING_FROM_NP()) { \
                /* FIXME: Error */ \
                return (return_type) 0; \
            } else { \
                /* FIXME: Error */ \
                return (return_type) 0; \
            } \
        } else { \
            if (CALLING_FROM_NP()) { \
                /* FIXME: Error */ \
                return (return_type) 0; \
            } else { \
                return function_name(__UVISOR_ENTRY_POINT_CALL_ARGS(__VA_ARGS__)); \
            } \
        } \
    }

/**/
#define UVISOR_ENTRY_POINT_FROM_ANY_PRIVILEGE(return_type, function_name, ...) \
    return_type UVISOR_EXTERN UVISOR_ENTRY_POINT function_name(__UVISOR_ENTRY_POINT_SIGNATURE_ARGS(__VA_ARGS__)) \
    { \
        if (CALLING_FROM_NS()) { \
            if (CALLING_FROM_NP()) { \
                return (return_type) __UVISOR_ENTRY_POINT_EXPAND_SVC( \
                    function_name __UVISOR_ENTRY_POINT_COMMA(__VA_ARGS__) __UVISOR_ENTRY_POINT_CALL_ARGS(__VA_ARGS__) \
                ); \
            } else { \
                return function_name(__UVISOR_ENTRY_POINT_CALL_ARGS(__VA_ARGS__)); \
            } \
        } else { \
            if (CALLING_FROM_NP()) { \
                /* FIXME: Error */ \
                return (return_type) 0; \
            } else { \
                return function_name(__UVISOR_ENTRY_POINT_CALL_ARGS(__VA_ARGS__)); \
            } \
        } \
    }

#else /* defined(ARCH_MPU_ARMv8M) */

/**/
#define UVISOR_ENTRY_POINT_FROM_LOW_PRIVILEGE(return_type, function_name, ...) \
    return_type UVISOR_EXTERN function_name(__UVISOR_ENTRY_POINT_SIGNATURE_ARGS(__VA_ARGS__)) \
    { \
        if (CALLING_FROM_NP()) { \
            return (return_type) __UVISOR_ENTRY_POINT_EXPAND_SVC( \
                function_name __UVISOR_ENTRY_POINT_COMMA(__VA_ARGS__) __UVISOR_ENTRY_POINT_CALL_ARGS(__VA_ARGS__) \
            ); \
        } else { \
            /* FIXME: Error */ \
            return (return_type) 0; \
        } \
    } \

/**/
#define UVISOR_ENTRY_POINT_FROM_HIGH_PRIVILEGE(return_type, function_name, ...) \
    return_type UVISOR_EXTERN function_name(__UVISOR_ENTRY_POINT_SIGNATURE_ARGS(__VA_ARGS__)) \
    { \
        if (CALLING_FROM_NP()) { \
            /* FIXME: Error */ \
            return (return_type) 0; \
        } else { \
            return function_name(__UVISOR_ENTRY_POINT_CALL_ARGS(__VA_ARGS__)); \
        } \
    } \

/**/
#define UVISOR_ENTRY_POINT_FROM_ANY_PRIVILEGE(return_type, function_name, ...) \
    return_type UVISOR_EXTERN function_name(__UVISOR_ENTRY_POINT_SIGNATURE_ARGS(__VA_ARGS__)) \
    { \
        if (CALLING_FROM_NP()) { \
            return (return_type) __UVISOR_ENTRY_POINT_EXPAND_SVC( \
                function_name __UVISOR_ENTRY_POINT_COMMA(__VA_ARGS__) __UVISOR_ENTRY_POINT_CALL_ARGS(__VA_ARGS__) \
            ); \
        } else { \
            return function_name(__UVISOR_ENTRY_POINT_CALL_ARGS(__VA_ARGS__)); \
        } \
    } \

#endif /* defined(ARCH_MPU_ARMv8M) */

#endif /* __UVISOR_API_ENTRY_POINTS_H__ */
