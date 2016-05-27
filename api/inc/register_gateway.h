/*
 * Copyright (c) 2015-2015, ARM Limited, All Rights Reserved
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
#ifndef __UVISOR_API_REGISTER_GATEWAY_H__
#define __UVISOR_API_REGISTER_GATEWAY_H__

#include "api/inc/register_gateway_exports.h"
#include "api/inc/uvisor_exports.h"
#include <stdint.h>

/* Register gateway metadata
 * The metadata attached to the register gateway depends on the operation
 * (simple read/write vs. masked read/write). */

#if defined(__CC_ARM)

/* TODO */

#elif defined(__GNUC__)

/* 0 arguments: Simple read, no mask */
#define __UVISOR_REGISTER_GATEWAY_METADATA0() \
    /* .value           = */ ".word 0\n" \
    /* .operation       = */ ".word " UVISOR_TO_STRING(UVISOR_RGW_OP_READ) "\n" \
    /* .mask            = */ ".word 0\n"

/* 1 argument: Simple write, no mask */
#define __UVISOR_REGISTER_GATEWAY_METADATA1(val) \
    /* .value           = */ ".word " UVISOR_TO_STRING(val) "\n" \
    /* .operation       = */ ".word " UVISOR_TO_STRING(UVISOR_RGW_OP_WRITE) "\n" \
    /* .mask            = */ ".word 0\n"

/* 2 arguments: Masked read */
#define __UVISOR_REGISTER_GATEWAY_METADATA2(op, mask) \
    /* .value           = */ ".word 0\n" \
    /* .operation       = */ ".word " UVISOR_TO_STRING(op) "\n" \
    /* .mask            = */ ".word " UVISOR_TO_STRING(mask) "\n"

/* 3 arguments: Masked write */
#define __UVISOR_REGISTER_GATEWAY_METADATA3(val, op, mask) \
    /* .value           = */ ".word " UVISOR_TO_STRING(val) "\n" \
    /* .operation       = */ ".word " UVISOR_TO_STRING(op) "\n" \
    /* .mask            = */ ".word " UVISOR_TO_STRING(mask) "\n"

#endif /* __CC_ARM || __GNUC__ */

/* Support macro used to overload the register gateway metadata based on the
 * inputs - operation, value, metadata. */
#define __UVISOR_REGISTER_GATEWAY_METADATA(...) \
     __UVISOR_MACRO_SELECT(_0, ##__VA_ARGS__, /* No macro for 4 args */          , \
                                              __UVISOR_REGISTER_GATEWAY_METADATA3, \
                                              __UVISOR_REGISTER_GATEWAY_METADATA2, \
                                              __UVISOR_REGISTER_GATEWAY_METADATA1, \
                                              __UVISOR_REGISTER_GATEWAY_METADATA0)(__VA_ARGS__)

/** Register Gateway - Read operation
 *
 * This macro provides a variadic API to perform 32-bit read operations on
 * restricted registers. Such accesses are assembled into a read-only flash
 * structure that is read and validated by uVisor before performing the
 * operation.
 *
 * The following APIs can be used:
 * void uvisor_read(src_box, address);
 * void uvisor_read(src_box, address, operation, mask);
 *
 * @warning These APIs currently only support hardcoded values, mask and
 * operation. This means that an access of this type is supported:
 * ```C
 * // OK
 * uvisor_write(&SYSCFG->CTRL, 0x1, UVISOR_RGW_OP_WRITE_AND, 0x1);
 * ```
 * While the following ones are not:
 * ```C
 * // Won't work
 * uint32_t some_value = 1;
 * uint32_t some_operation = UVISOR_RGW_OP_WRITE;
 * uvisor_write(&SYSCFG->CTRL, some_value);
 * uvisor_write(&SYSCFG->CTRL, 0x1, some_operation, 0x1);
 * ```
 *
 * @param src_box[in]   The name of the source box as decalred in
 *                      `UVISOR_BOX_CONFIG`.
 * @param address[in]   The address for the data access.
 * @param operation[in] The operation to perform at the address for the read. It
 *                      is chosen among the `UVISOR_RGW_OP_*` macros.
 * @param mask[in]      The mask to apply for the read operation.
 * @returns The value read from address using the operation and mask provided
 * (or their respective defaults if they have not been provided).
 */
#define uvisor_read(src_box, address, ...) \
    ({ \
        uint32_t res; \
        asm volatile( \
            "svc %[svc_id]\n" \
            "b.n skip_args%=\n" \
            "ldr.w r0, =%[addr]\n" \
            ".word " UVISOR_TO_STRING(UVISOR_REGISTER_GATEWAY_MAGIC) "\n" \
            ".word " UVISOR_TO_STRING(src_box) "_cfg_ptr\n" \
            __UVISOR_REGISTER_GATEWAY_METADATA(__VA_ARGS__) \
            "skip_args%=:\n" \
            :          "=r" (res) \
            : [svc_id] "I"  ((UVISOR_SVC_ID_REGISTER_GATEWAY) & 0xFF), \
              [addr]   "X"  ((uint32_t *) (address)) \
        ); \
        res; \
    })

/** Register Gateway - Write operation
 *
 * This macro provides a variadic API to perform 32-bit write operations on
 * restricted registers. Such accesses are assembled into a read-only flash
 * structure that is read and validated by uVisor before performing the
 * operation.
 *
 * The following APIs can be used:
 * void uvisor_write(src_box, address, value);
 * void uvisor_write(src_box, address, value, operation, mask);
 *
 * @warning These APIs currently only support hardcoded values, mask and
 * operation. This means that an access of this type is supported:
 * ```C
 * // OK
 * uvisor_write(&SYSCFG->CTRL, 0x1, UVISOR_RGW_OP_WRITE_AND, 0x1);
 * ```
 * While the following ones are not:
 * ```C
 * // Won't work
 * uint32_t some_value = 1;
 * uint32_t some_operation = UVISOR_RGW_OP_WRITE;
 * uvisor_write(&SYSCFG->CTRL, some_value);
 * uvisor_write(&SYSCFG->CTRL, 0x1, some_operation, 0x1);
 * ```
 *
 * @param src_box[in]   The name of the source box as decalred in
 *                      `UVISOR_BOX_CONFIG`.
 * @param address[in]   The address for the data access.
 * @param value[in]     The value to write at address.
 * @param operation[in] The operation to perform at the address for the read. It
 *                      is chosen among the `UVISOR_RGW_OP_*` macros.
 * @param mask[in]      The mask to apply for the write operation.
 */
#define uvisor_write(src_box, address, value, ...) \
    ({ \
        asm volatile( \
            "svc %[svc_id]\n" \
            "b.n skip_args%=\n" \
            "ldr.w r0, =%[addr]\n" \
            ".word " UVISOR_TO_STRING(UVISOR_REGISTER_GATEWAY_MAGIC) "\n" \
            ".word " UVISOR_TO_STRING(src_box) "_cfg_ptr\n" \
            __UVISOR_REGISTER_GATEWAY_METADATA(value, ##__VA_ARGS__) \
            "skip_args%=:\n" \
            :: [svc_id] "I" ((UVISOR_SVC_ID_REGISTER_GATEWAY) & 0xFF), \
               [addr]   "X" ((uint32_t *) (address)) \
        ); \
    })

#endif /* __UVISOR_API_REGISTER_GATEWAY_H__ */
