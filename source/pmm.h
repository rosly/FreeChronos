/* 
 * This file is a part of FreeChronos project
 * Copyright (c) 2013, Radoslaw Biernaki <radoslaw.biernacki@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1) Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2) Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3) No personal names or organizations' names associated with the 'RadOs' project
 *    may be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE FREECHRONOS PROJET AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __PMM_
#define __PMM_

/**
 * /note from slau259b.pdf fixed by diagram from slas554e.pdf
 *fSYS max       DVCC        SVSHRVL[1:0]              SVSMHRRL[2:0]         PMMCOREV[1:0]
 *in MHz            in V           Sets SVSH_IT- level     Sets SVSH_IT+ and
 *                                                                        SVMH levels
 *  8                 >1.8            00                               000                           00
 *  12               >2.0            01                               001                           01
 *  16               >2.2            10                               010                           10
 *  20               >2.4            11                               011                           11
 */

typedef enum {
   PMM_LEVEL_18 = 0,
   PMM_LEVEL_20,
   PMM_LEVEL_22,
   PMM_LEVEL_24
} PMM_Level_T;

typedef enum {
   PMM_LPM0 = 0,
   PMM_LPM1,
   PMM_LPM2,
   PMM_LPM3,
   PMM_LPM4,
} Pmm_LowPower_T;

/**
 * /brief Allows to set the desired VCore level in safe/designed matter
 * /param level desired PMMCOREV
 * /note: change level by one step only
 */
void PMM_SetVCore(PMM_Level_T level);

/**
 * /brief Set global high power request enable, which controls the spped of wakeup from lowpower
 * /param hipower true for HiPower request false to clear HiPower
 */
void PMM_SetHiPower(bool hipower);

/**
 * /brief Request the lowpower mode. There will be no return from this function until some ISR will enable power by Pmm_WakeUp
 * \note This function should be called only by Idle task in OS enviroment, since it will disable CPU and no more scheduling will be done until next interrupt
 * /param hipower true for HiPower request false to clear HiPower
 */
void PMM_LowPower(Pmm_LowPower_T state);

/** \note this function can be called only from ISR */
#define PMM_WakeUp() LPM4_EXIT //always clear all bits, this will wakeup the CPU from any LPM state

#define PMM_Por() PMMCTL0 |= PMMSWPOR

#endif /* __PMM_ */

