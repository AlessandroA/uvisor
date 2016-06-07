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
#ifndef __UVISOR_API_PRIV_SYS_IRQ_HOOK_EXPORTS_H__
#define __UVISOR_API_PRIV_SYS_IRQ_HOOK_EXPORTS_H__

/*
 * Privileged system interrupt hooks
 *
 * In this version of uVisor, uVisor lives alongside an RTOS that requires
 * running privileged code. In order for the RTOS to run any privileged code,
 * uVisor must allow the RTOS to handle a subset of privileged system
 * interrupts. Only the following system interrupts are hookable. Code called
 * by these hooks circumvents uVisor security. HANDLE WITH CARE. */
typedef struct {
    void (*priv_svc_0)(void);
    void (*priv_pendsv)(void);
    void (*priv_systick)(void);
} UvisorPrivSystemIRQHooks;

#endif
