/*
 * Copyright (c) 2013-2016, ARM Limited, All Rights Reserved
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
#ifndef __UVISOR_API_BOX_CONFIG_H__
#define __UVISOR_API_BOX_CONFIG_H__

#include "api/inc/uvisor_exports.h"
#include <stddef.h>
#include <stdint.h>

/** uVisor modes of operation. */
#define UVISOR_DISABLED   0 /**< The uVisor binary is stll embedded in the application but it is left disabled. */
#define UVISOR_PERMISSIVE 1 /**< The uVisor runs as normal but instead of blocking denied accesses it logs them. */
#define UVISOR_ENABLED    2 /**< The uVisor runs normally, blocking any attempt of denied access. */

/** Configure the uVisor mode of operation.
 * @param uvisor_mode[in]   uVisor mode of operation, chosen among
 *                          \ref UVISOR_ENABLED, \ref UVISOR_DISABLED, and
 *                          \ref UVISOR_PERMISSIVE
 */
#define UVISOR_CONFIG_MODE(uvisor_mode) \
    UVISOR_EXTERN uint32_t const __uvisor_mode = (uvisor_mode);


/** Type of the uVisor boxes index.
 * Each box has an internal index that keeps information on the box heap, stack
 * and context. This index is represented by \ref UvisorBoxIndex but an
 * operating system can extend it by embedding \ref UvisorBoxIndex into a custom
 * structure. If the OS does not specify an index type, the default \ref
 * UvisorBoxIndex is used.
 */
#ifndef UVISOR_CONFIG_INDEX_TYPE
#define UVISOR_CONFIG_INDEX_TYPE UvisorBoxIndex
#endif

/** Register your custom handlers for the system ISRs.
 * This macro tells uVisor to use the provided handlers to serve SVCall, PendSV
 * and SysTick exceptions, which are all system interrupts. The registered
 * handlers will run in exception mode (privileged).
 * @note SVCalls are not served directly by uVisor. Instead, they are
 * multiplexed by an ISR, and only if the SVCall ID is 0.
 * @param priv_svc_0_[in]   ISR for an SVCall exception. Use `NULL` if not
 *                          needed
 * @param priv_pendsv_[in]  ISR for a PendSV exception. Use `NULL` if not
 *                          needed
 * @param priv_systick_[in] ISR for a SysTick exception. Use `NULL` if not
 *                          needed
 */
#define UVISOR_CONFIG_SYSTEM_ISRS(priv_svc_0_, priv_pendsv_, priv_systick_) \
    UVISOR_EXTERN const UvisorPrivSystemIRQHooks __uvisor_priv_sys_irq_hooks = { \
        .priv_svc_0 = priv_svc_0_, \
        .priv_pendsv = priv_pendsv_, \
        .priv_systick = priv_systick_, \
    };

/** Configure the memories for a secure box.
 * @warning This configuration only applies to secure boxes. It is ignored for
 * box 0.
 * @warning This configuration requires the symbol \ref UVISOR_CONFIG_INDEX_TYPE
 * to be defined.
 * @param stack_size[in]    The required stack size for the secure box
 * @param heap_size[in]     The required heap size for the secure box
 * @param context_type[in]  The type of the custom box-specific context that
 *                          will be protected by uVisor. Optional
 */
#define UVISOR_CONFIG_BOX_MEMORIES(stack_size, heap_size, ...) \
    UVISOR_MACRO_SELECT_0_TO_1(UVISOR_CONFIG_BOX_MEMORIES2, UVISOR_CONFIG_BOX_MEMORIES3, ##__VA_ARGS__)(stack_size, \
                                                                                                        heap_size, \
                                                                                                        ##__VA_ARGS__)

/** Implementation for \ref UVISOR_CONFIG_MEMORIES when called with 2 arguments.
 * @internal
 * Just call the full API with `context_size` set to 0.
 */
#define UVISOR_CONFIG_BOX_MEMORIES2(stack_size, heap_size) \
    __UVISOR_CONFIG_BOX_MEMORIES(stack_size, heap_size, 0)

/* Implementation for \ref UVISOR_CONFIG_MEMORIES when called with 3 arguments.
 * @internal
 * First call the full API with context_size set to `sizeof(context_type)`, then
 * declare `__uvisor_ps` using `context_type`.
 */
#define UVISOR_CONFIG_BOX_MEMORIES3(stack_size, heap_size, context_type) \
    __UVISOR_CONFIG_BOX_MEMORIES(stack_size, heap_size, sizeof(context_type)) \
    \
    /* __uvisor_ps points to the type of the context.
     * In this way, the user can access the context structure with a single
     * de-reference. */ \
    UVISOR_EXTERN context_type * const * const __uvisor_ps;

/** The actual API implementation for \ref UVISOR_CONFIG_MEMORIES.
 * @internal
 */
#define __UVISOR_CONFIG_BOX_MEMORIES(stack_size, heap_size, context_size) \
    /* Save the settings for when we assemble the box configuration table. */ \
    static uint32_t const __uvisor_box_stack_size = UVISOR_MIN_STACK(stack_size); \
    static uint32_t const __uvisor_box_heap_size = (heap_size); \
    static uint32_t const __uvisor_box_context_size = context_size; \
    static uint32_t const __uvisor_box_index_size = sizeof(UVISOR_CONFIG_INDEX_TYPE); \
    \
    /* Pre-allocate enough memory to hold all BSS components.
     * The sum of all components (stack, heap, context, index) is rounded and
     * scaled to meet the MPU configuration requirements. */ \
    uint8_t __attribute__((section(".keep.uvisor.bss.boxes"), aligned(32))) box_name ## _reserved[ \
            UVISOR_STACK_SIZE_ROUND( \
                ( \
                    ( \
                        UVISOR_MIN_STACK(stack_size) + \
                        (heap_size) + \
                        sizeof(UVISOR_CONFIG_INDEX_TYPE) + \
                        (context_size) \
                    ) * 8 \
                ) / 6 \
            ) \
        ]; \

/** Configure the namespace for a secure box.
 * @warning This configuration only applies to secure boxes. It is ignored for
 * box 0.
 * @param box_namespace[in]     Pointer to the box namespace string. Use `NULL`
 *                              if none is required
 */
#define UVISOR_CONFIG_BOX_NAMESPACE(box_namespace) \
    /* Save the settings for when we assemble the box configuration table. */ \
    static char const * const __uvisor_box_namespace = (box_namespace);

/** Configure the ACLs for a box.
 * This macro can be used for both the main box and the secure boxes.
 * @param acl_list[in]  An array of `UvisorBoxAclItem`s
 */
#define UVISOR_CONFIG_BOX_ACL(acl_list) \
    /* Save the settings for when we assemble the box configuration table. */ \
    static UvisorBoxAclItem const * const __uvisor_box_acl_list = (acl_list); \
    static uint32_t const __uvisor_box_acl_count = UVISOR_ARRAY_COUNT(acl_list); \

/** Configure box 0.
 * @warning Only call this macro after having called the compulsory \ref
 * UVISOR_CONFIG_BOX_ACL function.
 * @warning This macro can only be called once.
 */
#define UVISOR_CONFIG_BOX_0() \
    /* Pre-allocate enough memory to hold a single stack guard band.
     * This is not needed by box 0 but is always allocoated before any other
     * secure box memory, so it must be taken into account. */ \
    uint8_t __attribute__((section(".keep.uvisor.bss.boxes"), aligned(32))) __reserved_stack[UVISOR_STACK_BAND_SIZE]; \
    \
    /* Assemble the configuration table for box 0. */ \
    static __attribute__((section(".keep.uvisor.cfgtbl"), aligned(4))) UvisorBoxConfig const box0_cfg = { \
        /* .magic        = */ UVISOR_BOX_MAGIC, \
        /* .version      = */ UVISOR_BOX_VERSION, \
        /* .stack_size   = */ 0, \
        /* .index_size   = */ sizeof(UVISOR_CONFIG_INDEX_TYPE), \
        /* .context_size = */ 0, \
        /* .heap_size    = */ 0, \
        /* .namespace    = */ NULL, \
        /* .acl_list     = */ __uvisor_box_acl_list, \
        /* .acl_count    = */ __uvisor_box_acl_count \
    }; \
    \
    /* Store the pointer to the configuration table. */ \
    UVISOR_EXTERN __attribute__((section(".keep.uvisor.cfgtbl_ptr_first"), aligned(4))) \
    void const * const box0_cfg_ptr = &box0_cfg;

/** Configure a secure box.
 * @warning Only call this macro after having called the compulsory \ref
 * UVISOR_CONFIG_BOX_ACL, \ref UVISOR_CONFIG_BOX_MEMORIES, and \ref
 * UVISOR_CONFIG_BOX_NAMESPACE functions.
 * @warning This macro can only be called once for each secure box.
 */
#define UVISOR_CONFIG_BOX_SECURE(box_name) \
    /* Assemble the configuration table for box 0. */ \
    static __attribute__((section(".keep.uvisor.cfgtbl"), aligned(4))) UvisorBoxConfig const box_name ## _cfg = { \
        /* .magic        = */ UVISOR_BOX_MAGIC, \
        /* .version      = */ UVISOR_BOX_VERSION, \
        /* .stack_size   = */ __uvisor_box_stack_size, \
        /* .index_size   = */ __uvisor_box_index_size, \
        /* .context_size = */ __uvisor_box_context_size, \
        /* .heap_size    = */ __uvisor_box_heap_size, \
        /* .namespace    = */ __uvisor_box_namespace, \
        /* .acl_list     = */ __uvisor_box_acl_list, \
        /* .acl_count    = */ __uvisor_box_acl_count \
    }; \
    \
    /* Store the pointer to the configuration table. */ \
    UVISOR_EXTERN __attribute__((section(".keep.uvisor.cfgtbl_ptr"), aligned(4))) \
    void const * const box_name ## _cfg_ptr = &box_name ## _cfg;

#define uvisor_ctx (*__uvisor_ps)

UVISOR_EXTERN const uint32_t __uvisor_mode;

/* Return the numeric box ID of the current box. */
UVISOR_EXTERN int uvisor_box_id_self(void);

/* Return the numeric box ID of the box that is calling through the most recent
 * secure gateway. Return -1 if there is no secure gateway calling box. */
UVISOR_EXTERN int uvisor_box_id_caller(void);

/* Copy the box namespace of the specified box ID to the memory provided by
 * box_namespace. The box_namespace's length must be at least
 * MAX_BOX_NAMESPACE_LENGTH bytes. Return how many bytes were copied into
 * box_namespace. Return UVISOR_ERROR_INVALID_BOX_ID if the provided box ID is
 * invalid. Return UVISOR_ERROR_BUFFER_TOO_SMALL if the provided box_namespace
 * is too small to hold MAX_BOX_NAMESPACE_LENGTH bytes. Return
 * UVISOR_ERROR_BOX_NAMESPACE_ANONYMOUS if the box is anonymous. */
UVISOR_EXTERN int uvisor_box_namespace(int box_id, char *box_namespace, size_t length);

#endif /* __UVISOR_API_BOX_CONFIG_H__ */
