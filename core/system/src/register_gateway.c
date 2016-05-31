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
#include <uvisor.h>
#include "halt.h"
#include "register_gateway.h"
#include "vmpu.h"

/** Perform the validation of a register gateway.
 * @internal
 * @warning This function does not verify that the operation, value and mask
 * make sense together. The code that performs the operation should check for
 * this and throw an error accordingly.
 * @param register_gateway[in]  Pointer to the register gateway structure in
 *                              flash.
 * @returns the extracted address of the register gateway.
 */
static uint32_t * register_gateway_check(TRegisterGateway * register_gateway)
{
    /* Verify that the register gateway is in public flash. */
    /* Note: We assume here that TRegisterGateway is a multiple of 32 bit. If
     * that is not true there might be false negatives with an unaligned secure
     * gateway. */
    int i = 0;
    int count = 0;
    for (; i < sizeof(TRegisterGateway) / sizeof(uint32_t); i++) {
        if (!vmpu_public_flash_addr(((uint32_t) register_gateway) + sizeof(uint32_t) * i)) {
            count++;
        }
    }
    if (count) {
        HALT_ERROR(SANITY_CHECK_FAILED, "Register gateway 0x%08X is not in public flash (%d words)",
                   (uint32_t) register_gateway, count);
    }

    /* Verify that the register gateway magic is present. */
    if (register_gateway->magic != UVISOR_REGISTER_GATEWAY_MAGIC) {
        HALT_ERROR(SANITY_CHECK_FAILED, "Register gateway 0x%08X does not contain a valid magic (0x%08X)",
                   (uint32_t) register_gateway, register_gateway->magic);
    }

    /* Verify that the destination box configuration pointer is in the dedicated
     * flash region. */
    if (register_gateway->src_box_cfg_ptr < (uint32_t) __uvisor_config.cfgtbl_ptr_start ||
        register_gateway->src_box_cfg_ptr >= (uint32_t) __uvisor_config.cfgtbl_ptr_end) {
        HALT_ERROR(SANITY_CHECK_FAILED,
                   "The pointer (0x%08X) in the secure gateway 0x%08X is not a valid box configuration pointer",
                   register_gateway->src_box_cfg_ptr, (uint32_t) register_gateway);
    }

    /* Verify that the load instruction is valid. The load instruction inside
     * the register gateway points to the literal pool that contains the real
     * address of the gateway. */
    if (!REGISTER_GATEWAY_LDR_ADDR_CHECK(register_gateway->ldr_opcode)) {
        HALT_ERROR(SANITY_CHECK_FAILED, "Register gateway 0x%08X is corrupted.", (uint32_t) register_gateway);
    }

    /* Compute the address of the literal pool that contains the address of the
     * register gateway. This is given by the address of the load instruction
     * (which is a program counter), plus the pc-relative immediate extracted
     * from the load instruction. */
    uint32_t ldr_addr = (uint32_t) &(register_gateway->ldr_opcode) +
                        (uint32_t) REGISTER_GATEWAY_LDR_IMM_GET(register_gateway->ldr_opcode);

    /* Check that the literal pool that contains the address of the register
     * gateway is in public flash. */
    if (!vmpu_public_flash_addr(ldr_addr)) {
        HALT_ERROR(SANITY_CHECK_FAILED, "Register gateway 0x%08X does not contain a valid address (0x%08X)",
                   (uint32_t) register_gateway, *((uint32_t *) ldr_addr));
    }

    /* If none of the checks above failed, then the input pointer is a valid
     * register gateway. */
    /* Note: We do not check here that the address is valid, as this is the
     * ACL-checks job. */
    return (uint32_t *) (*((uint32_t *) ldr_addr));
}

/* Perform a register gateway operation. */
uint32_t register_gateway_perform_operation(uint32_t svc_sp, uint32_t svc_pc)
{
    /* Check if the SVCall points to a secure gateway and extract the
     * destination address from it. */
    /* This function halts if it finds an error. */
    TRegisterGateway * register_gateway = (TRegisterGateway *) svc_pc;
    uint32_t * address = register_gateway_check(register_gateway);

    /* From now on we can assume the register_gateway structure and the address
     * are valid. */

    /* Check that the operation is permitted. */
    /* This function halts if it finds an error. */
    vmpu_register_gateway_check(address, register_gateway->operation, register_gateway->mask);

    /* Perform the actual operation. */
    switch (register_gateway->operation) {
    case UVISOR_RGW_OP_READ:
        return *address;
    case UVISOR_RGW_OP_WRITE:
        *(address) = register_gateway->value;
        break;
    default:
        HALT_ERROR(NOT_ALLOWED, "Register level gateway: Operation 0x%08X not recognised.",
                   register_gateway->operation);
        break;
    }

    /* Return value for write operations. */
    return 0;
}
