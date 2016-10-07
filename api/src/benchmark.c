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

void uvisor_benchmark_configure(void)
{
    /* in sequence:
     *   - initialize the benchmarking unit (default overhead = 0)
     *   - execute a nop measurement to assess the overhead introduced by the
     *     measurement itself
     *   - pass the computed overhead to the configuration routine to complete
     *     the calibration */
    UVISOR_SVC(uvisor_benchmark_configure, 0);
    uvisor_benchmark_start();
    UVISOR_SVC(uvisor_benchmark_configure, uvisor_benchmark_stop());
}

void uvisor_benchmark_start(void)
{
    UVISOR_SVC(uvisor_benchmark_start);
}

uint32_t uvisor_benchmark_stop(void)
{
    return UVISOR_SVC(uvisor_benchmark_stop);
}
