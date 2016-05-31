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
#ifndef __REGISTER_GATEWAY_H__
#define __REGISTER_GATEWAY_H__

#include "api/inc/register_gateway_exports.h"
#include <stdint.h>

/** Register gateway structure. */
typedef struct {
    uint16_t svc_opcode;
    uint16_t branch;
    uint32_t ldr_opcode;
    uint32_t magic;
    uint32_t src_box_cfg_ptr;
    uint32_t value;
    uint32_t operation;
    uint32_t mask;
} UVISOR_PACKED TRegisterGateway;

/** Masks to extract the fixed part of the ldr opcode in the register gateway.
 * @internal
 * The opcode is encoded as a 32 bit (Thumb-2) LDR.W instruction:
 *      LDR.W <R0>, [PC, #imm]
 * Where:
 *      LDR.W = 0xF8DF----
 *      <R0>  = 0x----0---
 *      #imm  = 0x-----XXX
 * Only #imm is variable. The other fields will be checked against the expected
 * values. The opcode is stored in the TRegisterGateway struct with inverted
 * endianness, so it must be swapped when peforming the #imm extraction.
 */
#define __REGISTER_GATEWAY_LDR_OPCODE_MASK  0xF000FFFF
#define __REGISTER_GATEWAY_LDR_OPCODE_VALUE 0xF8DF
#define __REGISTER_GATEWAY_LDR_IMM_MASK     0x0FFF0000
#define __REGISTER_GATEWAY_LDR_IMM_SHIFT    16

/** Check that the ldr opcode inside the register gateway is valid.
 * @internal
 * @param ldr_opcode[in]    Opcode of the LDR.W instruction of the gateway
 * @retval true  the opcode is valid
 * @retval false the opcode is not valid
 */
#define REGISTER_GATEWAY_LDR_ADDR_CHECK(ldr_opcode) \
    (((uint32_t) (ldr_opcode) & ((uint32_t) (__REGISTER_GATEWAY_LDR_OPCODE_MASK))) == \
                                ((uint32_t) (__REGISTER_GATEWAY_LDR_OPCODE_VALUE)))

/** Extract the pc-relative immediate from the register gateway.
 * @internal
 * @param ldr_opcode[in]    Opcode of the LDR.W instruction of the gateway
 * @returns the immediate field in the LDR.W instruction.
 */
#define REGISTER_GATEWAY_LDR_IMM_GET(ldr_opcode) \
    (((uint32_t) (ldr_opcode) & ((uint32_t) (__REGISTER_GATEWAY_LDR_IMM_MASK))) >> \
                                ((uint32_t) (__REGISTER_GATEWAY_LDR_IMM_SHIFT)))

/** Perform a register gateway operation.
 * @param svc_sp[in]    Unprivileged stack pointer at the time of the register
 *                      gateway
 * @param svc_pc[in]    Program counter at the time of the register gateway
 */
uint32_t register_gateway_perform_operation(uint32_t svc_sp, uint32_t svc_pc);

#endif /* __REGISTER_GATEWAY_H__ */
