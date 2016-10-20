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
UVISOR_ENTRY_POINT_FROM_LOW_PRIVILEGE(int, uvisor_box_namespace, int, char *, size_t);

/*******************************************************************************
 * Debug
 ******************************************************************************/
UVISOR_ENTRY_POINT_FROM_LOW_PRIVILEGE(void, uvisor_debug_init, const TUvisorDebugDriver * const);

/*******************************************************************************
 * Error
 ******************************************************************************/
UVISOR_ENTRY_POINT_FROM_ANY_PRIVILEGE(void, uvisor_error, THaltUserError);

/*******************************************************************************
 * Interrupts
 ******************************************************************************/
UVISOR_ENTRY_POINT_FROM_LOW_PRIVILEGE(void,     vIRQ_SetVector,       uint32_t,     uint32_t);
UVISOR_ENTRY_POINT_FROM_LOW_PRIVILEGE(uint32_t, vIRQ_GetVector,       uint32_t              );
UVISOR_ENTRY_POINT_FROM_LOW_PRIVILEGE(void,     vIRQ_EnableIRQ,       uint32_t              );
UVISOR_ENTRY_POINT_FROM_LOW_PRIVILEGE(void,     vIRQ_DisableIRQ,      uint32_t              );
UVISOR_ENTRY_POINT_FROM_LOW_PRIVILEGE(void,     vIRQ_DisableAll                             );
UVISOR_ENTRY_POINT_FROM_LOW_PRIVILEGE(void,     vIRQ_EnableAll,                             );
UVISOR_ENTRY_POINT_FROM_LOW_PRIVILEGE(void,     vIRQ_ClearPendingIRQ, uint32_t              );
UVISOR_ENTRY_POINT_FROM_LOW_PRIVILEGE(void,     vIRQ_SetPendingIRQ,   uint32_t              );
UVISOR_ENTRY_POINT_FROM_LOW_PRIVILEGE(uint32_t, vIRQ_GetPendingIRQ,   uint32_t              );
UVISOR_ENTRY_POINT_FROM_LOW_PRIVILEGE(void,     vIRQ_SetPriority,     uint32_t,     uint32_t);
UVISOR_ENTRY_POINT_FROM_LOW_PRIVILEGE(uint32_t, vIRQ_GetPriority,     uint32_t              );
UVISOR_ENTRY_POINT_FROM_LOW_PRIVILEGE(int,      vIRQ_GetLevel                               );
UVISOR_ENTRY_POINT_FROM_LOW_PRIVILEGE(void,     vIRQ_SystemReset,     TResetReason          );

/*******************************************************************************
 * Page allocator
 ******************************************************************************/
UVISOR_ENTRY_POINT_FROM_LOW_PRIVILEGE(int, uvisor_page_malloc, UvisorPageTable * const      )
UVISOR_ENTRY_POINT_FROM_LOW_PRIVILEGE(int, uvisor_page_free,   const UvisorPageTable * const)
