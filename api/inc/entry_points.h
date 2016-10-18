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

/* Returns true if the code is running from unprivileged mode. */
#define CALLING_FROM_UNPRIVILEGED_MODE() (__get_control() & __CONTROL_nPRIV_Msk)

#define UVISOR_ENTRY_POINT(function_name, ...) \
    ({ \
        uint32_t result; \
        if (CALLING_FROM_UNPRIVILEGED_MODE()) { \
            result = UVISOR_SVC(function_name, ##__VA_ARGS__); \
        } else { \
            result = UVISOR_FUNCTION_CALL(function_name, ##__VA_ARGS__); \
        } \
        result; \
    })

#define UVISOR_ENTRY_POINT_UNPRIV(function_name, ...) \
    ({ \
        uint32_t result; \
        if (CALLING_FROM_UNPRIVILEGED_MODE()) { \
            result = UVISOR_SVC(function_name, ##__VA_ARGS__); \
        } else { \
            result = 0; \
            uvisor_error(UVISOR_ERROR_USER_NOT_ALLOWED); \
        } \
        result; \
    })

#define UVISOR_ENTRY_POINT_PRIV(function_name, ...) \
    ({ \
        uint32_t result; \
        if (CALLING_FROM_UNPRIVILEGED_MODE()) { \
            result = 0; \
            uvisor_error(UVISOR_ERROR_USER_NOT_ALLOWED); \
        } else { \
            result = UVISOR_FUNCTION_CALL(function_name, ##__VA_ARGS__); \
        } \
        result; \
    })

#define UVISOR_ENTRY_POINT_NS_NP(return_type, function_name, ...) \
    return_type function_name(__VA_ARGS__) { \
        if (CALLING_FROM_NS()) { \
            if (CALLING_FROM_NP()) { \
                function_name(__UVISOR_ENTRY_POINT_FUNCTION_ARGS(__VA_ARGS__)); \
            } \
    } \
    UVISOR_EXTERN uint32_t __attribute__((section(".entry_points"))) function


#endif /* __UVISOR_API_ENTRY_POINTS_H__ */
