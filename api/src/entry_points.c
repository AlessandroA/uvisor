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
#include "api/inc/uvisor-lib.h"
#include "api/inc/entry_points.h"

#include <stddef.h>
#include <stdint.h>

/*******************************************************************************
 * Box identity
 ******************************************************************************/

UVISOR_EXTERN int uvisor_box_namespace(int box_id, char *box_namespace, size_t length)
{
    return UVISOR_ENTRY_POINT(uvisor_box_namespace, box_id, box_namespace, length);
}


/*******************************************************************************
 * Debug
 ******************************************************************************/

UVISOR_EXTERN void uvisor_debug_init(const TUvisorDebugDriver * const driver)
{
    UVISOR_ENTRY_POINT(uvisor_debug_init, driver);
}


/*******************************************************************************
 * Error
 ******************************************************************************/

UVISOR_EXTERN void uvisor_error(THaltUserError reason)
{
    UVISOR_ENTRY_POINT(uvisor_error, reason);
}


/*******************************************************************************
 * Interrupts
 ******************************************************************************/

UVISOR_EXTERN void vIRQ_SetVector(uint32_t irqn, uint32_t vector)
{
    UVISOR_ENTRY_POINT(vIRQ_SetVector, irqn, vector);
}

UVISOR_EXTERN uint32_t vIRQ_GetVector(uint32_t irqn)
{
    return UVISOR_ENTRY_POINT(vIRQ_GetVector, irqn);
}

UVISOR_EXTERN void vIRQ_EnableIRQ(uint32_t irqn)
{
    UVISOR_ENTRY_POINT(vIRQ_EnableIRQ, irqn);
}

UVISOR_EXTERN void vIRQ_DisableIRQ(uint32_t irqn)
{
    UVISOR_ENTRY_POINT(vIRQ_DisableIRQ, irqn);
}

UVISOR_EXTERN void vIRQ_DisableAll(void)
{
    UVISOR_ENTRY_POINT(vIRQ_DisableAll);
}

UVISOR_EXTERN void vIRQ_EnableAll(void)
{
    UVISOR_ENTRY_POINT(vIRQ_EnableAll);
}

UVISOR_EXTERN void vIRQ_ClearPendingIRQ(uint32_t irqn)
{
    UVISOR_ENTRY_POINT(vIRQ_ClearPendingIRQ, irqn);
}

UVISOR_EXTERN void vIRQ_SetPendingIRQ(uint32_t irqn)
{
    UVISOR_ENTRY_POINT(vIRQ_SetPendingIRQ, irqn);
}

UVISOR_EXTERN uint32_t vIRQ_GetPendingIRQ(uint32_t irqn)
{
    return UVISOR_ENTRY_POINT(vIRQ_GetPendingIRQ, irqn);
}

UVISOR_EXTERN void vIRQ_SetPriority(uint32_t irqn, uint32_t priority)
{
    UVISOR_ENTRY_POINT(vIRQ_SetPriority, irqn, priority);
}

UVISOR_EXTERN uint32_t vIRQ_GetPriority(uint32_t irqn)
{
    return UVISOR_ENTRY_POINT(vIRQ_GetPriority, irqn);
}

UVISOR_EXTERN int vIRQ_GetLevel(void)
{
    return UVISOR_ENTRY_POINT(vIRQ_GetLevel);
}

UVISOR_EXTERN void vIRQ_SystemReset(TResetReason reason)
{
    UVISOR_ENTRY_POINT(debug_die, reason);
}


/*******************************************************************************
 * Page allocator
 ******************************************************************************/

UVISOR_EXTERN int uvisor_page_malloc(UvisorPageTable * const table)
{
    return UVISOR_ENTRY_POINT(uvisor_page_malloc, table);
}

UVISOR_EXTERN int uvisor_page_free(const UvisorPageTable * const table)
{
    return UVISOR_ENTRY_POINT(uvisor_page_free, table);
}
