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
#include "context.h"
#include "halt.h"
#include "linker.h"
#include "unvic.h"

/* This key needs to be written to AIRCR at every write access. */
#define SCB_AIRCR_VECTKEY 0x5FAUL

static void vmpu_arch_init_sau(void)
{
    /* Disable the SAU and configure all code to run in S mode. */
    SAU->CTRL = 0;

    /* Configure the public SRAM to be accessible in NS mode. */
    SAU->RNR = 0;
    SAU->RBAR = ((uint32_t) __uvisor_config.bss_end) & 0xFFFFFFE0;
    SAU->RLAR = (((uint32_t) __uvisor_config.sram_end) & 0xFFFFFFE0) | SAU_RLAR_ENABLE_Msk;

    /* Configure the whole flash to be accessible in NS mode. */
    SAU->RNR = 1;
    SAU->RBAR = ((uint32_t) __uvisor_config.flash_start) & 0xFFFFFFE0;
    SAU->RLAR = (((uint32_t) __uvisor_config.flash_end) & 0xFFFFFFE0) | SAU_RLAR_ENABLE_Msk;

    /* Enable the SAU. */
    SAU->CTRL |= SAU_CTRL_ENABLE_Msk;
}

uint32_t vmpu_fault_find_acl(uint32_t fault_addr, uint32_t size)
{
    return 0;
}

void vmpu_sys_mux_handler(uint32_t lr, uint32_t msp)
{
    /* Get the exception number. */
    int irqn = ((IPSR_Type) __get_IPSR()).b.ISR - NVIC_OFFSET;
    if (irqn >= NVIC_OFFSET) {
        HALT_ERROR(NOT_ALLOWED, "An user IRQ (%d) is being served by the system ISR.\r\n", irqn);
    }

    /* Behave differently depending on the fault. */
    switch (irqn) {
    case HardFault_IRQn:
        HALT_ERROR(FAULT_HARD, "Hard fault.\r\n");
    case MemoryManagement_IRQn:
        HALT_ERROR(FAULT_MEMMANAGE, "MemManage fault.\r\n");
    case BusFault_IRQn:
        DPRINTF("CFSR: 0x%08X\r\n", SCB->CFSR);
        DPRINTF("BFAR: 0x%08X\r\n", SCB->BFAR);
        HALT_ERROR(FAULT_BUS, "Bus fault.\r\n");
    case UsageFault_IRQn:
        HALT_ERROR(FAULT_USAGE, "Usage fault.\r\n");
    case SecureFault_IRQn:
        DPRINTF("SFSR: 0x%08X\r\n", SAU->SFSR);
        DPRINTF("SFAR: 0x%08X\r\n", SAU->SFAR);
        HALT_ERROR(FAULT_SECURE, "Secure fault.\r\n");
    default:
        HALT_ERROR(NOT_ALLOWED, "Unrecognized fault: %d.\r\n", irqn);
    }
}

void vmpu_switch(uint8_t src_box, uint8_t dst_box)
{
}

void vmpu_load_box(uint8_t box_id)
{
}

void vmpu_acl_stack(uint8_t box_id, uint32_t bss_size, uint32_t stack_size)
{
    /* The public box is handled separately. */
    if (box_id == 0) {
        /* Non-important sanity checks */
        assert(stack_size == 0);

        /* For backwards-compatibility, the public box uses the legacy stack and
         * heap. */
        DPRINTF("[box 0] BSS size: %i. Stack size: %i\r\n", bss_size, stack_size);
        g_context_current_states[0].sp = __get_PSP();
        g_context_current_states[0].bss = (uint32_t) __uvisor_config.heap_start;
        return;
    }
}

void vmpu_arch_init(void)
{
    /* AIRCR configurations:
     *      - Non-secure exceptions are de-prioritized.
     *      - BusFault, HardFault, and NMI are Secure.
     *      - SYSRESETREQ functionality is only available to Secure state.
     */
    /* TODO: Setup a sensible priority grouping. */
    SCB->AIRCR = (SCB_AIRCR_VECTKEY << SCB_AIRCR_VECTKEY_Pos) |
                 (SCB->AIRCR & SCB_AIRCR_ENDIANESS_Msk) | /* Keep unchanged */
                 (SCB_AIRCR_PRIS_Msk) |
                 (0 << SCB_AIRCR_BFHFNMINS_Pos) |
                 (SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk) | /* Keep unchanged */
                 (SCB_AIRCR_SYSRESETREQ_Msk) |
                 (0 << SCB_AIRCR_SYSRESETREQ_Pos) |
                 (0 << SCB_AIRCR_VECTCLRACTIVE_Pos);

    /* SHCSR configurations:
     *      - SecureFault exception enabled.
     *      - UsageFault exception enabled for the selected Security state.
     *      - BusFault exception enabled.
     *      - MemManage exception enabled for the selected Security state.
     */
    SCB->SHCSR |= (SCB_SHCSR_SECUREFAULTENA_Msk) |
                  (SCB_SHCSR_USGFAULTENA_Msk) |
                  (SCB_SHCSR_BUSFAULTENA_Msk) |
                  (SCB_SHCSR_MEMFAULTENA_Msk);

    /* Setup the SAU. */
    vmpu_arch_init_sau();
}

void vmpu_arch_init_hw(void)
{
}
