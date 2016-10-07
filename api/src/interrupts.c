/*
 * Copyright (c) 2013-2015, ARM Limited, All Rights Reserved
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
#include "core/uvisor.h"
#include <stdint.h>

void vIRQ_SetVector(uint32_t irqn, uint32_t vector)
{
    UVISOR_SVC(vIRQ_SetVector, irqn, vector);
}

uint32_t vIRQ_GetVector(uint32_t irqn)
{
    return UVISOR_SVC(vIRQ_GetVector, irqn);
}

void vIRQ_EnableIRQ(uint32_t irqn)
{
    UVISOR_SVC(vIRQ_EnableIRQ, irqn);
}

void vIRQ_DisableIRQ(uint32_t irqn)
{
    UVISOR_SVC(vIRQ_DisableIRQ, irqn);
}

void vIRQ_DisableAll(void)
{
    UVISOR_SVC(vIRQ_DisableAll);
}

void vIRQ_EnableAll(void)
{
    UVISOR_SVC(vIRQ_EnableAll);
}

void vIRQ_ClearPendingIRQ(uint32_t irqn)
{
    UVISOR_SVC(vIRQ_ClearPendingIRQ, irqn);
}

void vIRQ_SetPendingIRQ(uint32_t irqn)
{
    UVISOR_SVC(vIRQ_SetPendingIRQ, irqn);
}

uint32_t vIRQ_GetPendingIRQ(uint32_t irqn)
{
    return UVISOR_SVC(vIRQ_GetPendingIRQ, irqn);
}

void vIRQ_SetPriority(uint32_t irqn, uint32_t priority)
{
    UVISOR_SVC(vIRQ_SetPriority, irqn, priority);
}

uint32_t vIRQ_GetPriority(uint32_t irqn)
{
    return UVISOR_SVC(vIRQ_GetPriority, irqn);
}

int vIRQ_GetLevel(void)
{
    return UVISOR_SVC(vIRQ_GetLevel);
}

void vIRQ_SystemReset(TResetReason reason)
{
    UVISOR_SVC(debug_die, reason);
}
