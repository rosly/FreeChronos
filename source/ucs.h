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

#ifndef __UCS_
#define __UCS_

typedef enum {
   UCS_X1_DRVSTRGHT_MIN = 0,     /* lowers power consumption in LF, in HF freq 4MHz - 8MHz */
   UCS_X1_DRVSTRGHT_LOW,          /* low power consumption in LF, in HF freq 8MHz - 16 MHz */
   UCS_X1_DRVSTRGHT_MED,          /* med power consumption in LF, in HF freq 16MHz - 24MHz */
   UCS_X1_DRVSTRGHT_MAX           /* hi power consumption in LF, in HF freq 24MHz - 32MHz */
} UCS_X1_DrvStrght_T;

typedef enum {
   UCS_X1CAP_2PF = 0,
   UCS_X1CAP_6PF,
   UCS_X1CAP_9PF,
   UCS_X1CAP_12PF,
} UCS_X1Cap_T;

typedef enum {
   UCS_CLOCK_XT1CLK = 0,
   UCS_CLOCK_VLOCLK,
   UCS_CLOCK_REFOCLK,
   UCS_CLOCK_DCOCLK,
   UCS_CLOCK_DCOCLKDIV,
   UCS_CLOCK_XT2CLK, /* when available, otherwise DCOCLKDIV */

   UCS_CLOCK_NONE, /* valid only for fll ref */
} UCS_Clock_T;

typedef struct {
   UCS_Clock_T fllref;
   UCS_Clock_T aclk;
   UCS_Clock_T smclk;
   UCS_Clock_T mclk;
} UCS_ClockSource_T;

typedef enum {
   UCS_FLLREFDIV_1 = 0, /* fFLLREFCLK/1 */
   UCS_FLLREFDIV_2, /* fFLLREFCLK/2 */
   UCS_FLLREFDIV_4, /* fFLLREFCLK/4 */
   UCS_FLLREFDIV_8, /* fFLLREFCLK/8 */
   UCS_FLLREFDIV_12, /* fFLLREFCLK/12 */
   UCS_FLLREFDIV_16, /* fFLLREFCLK/16 */
} UCS_FllRefDiv_T;

typedef enum {
   UCS_DCORANGE_0 = 0, /* 0,07 - 1,7 MHz */
   UCS_DCORANGE_1,       /* 0,15 - 3,45 MHz */
   UCS_DCORANGE_2,       /* 0,32 - 7,38 MHz */
   UCS_DCORANGE_3,       /* 0,64 - 14,0 MHz */
   UCS_DCORANGE_4,       /* 1,30 - 28,2 MHz */
   UCS_DCORANGE_5,       /* 2,50 - 54,1 MHz */
   UCS_DCORANGE_6,       /* 4,60 - 88,0 MHz */
   UCS_DCORANGE_7,       /* 8,50 - 135,0 MHz */
} UCS_DCORange_T;

typedef enum {
   UCS_FLLD_1 = 0, /* fDCOCLK/1 */
   UCS_FLLD_2,       /* fDCOCLK/2 */
   UCS_FLLD_4,       /* fDCOCLK/4 */
   UCS_FLLD_8,       /* fDCOCLK/8 */
   UCS_FLLD_16,     /* fDCOCLK/16 */
   UCS_FLLD_32,     /* fDCOCLK/32 */
} UCS_FllD_T;

typedef struct {
   UCS_FllRefDiv_T fllrefdiv;
   UCS_DCORange_T dcorsel;
   UCS_FllD_T flld;
   uint16_t flln;
   bool dismod;
} UCS_FllSetting_T;

/**
 * /brief Set the XT1 oscilator params
 */
void UCS_XT1Set(bool enable, UCS_X1_DrvStrght_T drvstrght, UCS_X1Cap_T cap);

void UCS_SourceGet(UCS_ClockSource_T *src);

void UCS_SourceSet(const UCS_ClockSource_T *src);

void UCS_FllSet(const UCS_FllSetting_T *set);

void UCS_WaitStab(void);

#endif /* __PMM_ */

