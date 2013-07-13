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

#include "project.h"
#include "pmm.h"

static void pmm_setvcoreup (uint8_t level);
static void pmm_setvcoredown (uint8_t level);

void PMM_SetVCore(PMM_Level_T level)
{
   uint8_t actLevel = PMMCTL0_L & (PMMCOREV0 | PMMCOREV1); //consider only PMMCOREV bits

   //increase VCore step by step acording to slau259b.pdf
   while(actLevel < level) {
      pmm_setvcoreup(++actLevel);
   }

   //decrease VCore step by step acording to slau259b.pdf
   while(actLevel > level) {
      pmm_setvcoredown(--actLevel);
   }
}

void PMM_SetHiPower(bool hipower)
{
   PMMCTL0_H = PMMPW_H;                                                                          // Open PMM registers for write access (password)
   if( hipower )
      PMMCTL0_L |= PMMHPMRE;                                                                    //set hipower bit
   else
      PMMCTL0_L &= (~PMMHPMRE);                                                              //clear hpower bit
   PMMCTL0_H = 0x00;                                                                                // Lock PMM registers for write access
}

void PMM_LowPower(Pmm_LowPower_T state)
{
   switch(state) {
      case PMM_LPM0:
         LPM0;
          __no_operation();
         break;

      case PMM_LPM1:
         LPM1;
          __no_operation();
         break;

      case PMM_LPM2:
         LPM2;
          __no_operation();
         break;

      case PMM_LPM3:
         LPM3;
          __no_operation();
         break;

      case PMM_LPM4:
         LPM4;
          __no_operation();
         break;
   }
}

static void pmm_setvcoreup (uint8_t level)
{
   PMMCTL0_H = PMMPW_H;                                                                          // Open PMM registers for write access (password)

   SVSMHCTL = SVSHE + SVSHRVL0 * level + SVMHE + SVSMHRRL0 * level;    // Set SVS/SVM high side new level
   SVSMLCTL = SVSLE + SVMLE + SVSMLRRL0 * level;                                    // Set SVM low side to new level
   while ((PMMIFG & SVSMLDLYIFG) == 0);                                                     // Wait till SVM is settled

   PMMIFG &= ~(SVMLVLRIFG + SVMLIFG);                                                    // Clear already set flags
   PMMCTL0_L = PMMCOREV0 * level;                                                            // Set VCore to new level
   if ((PMMIFG & SVMLIFG)) {
      while ((PMMIFG & SVMLVLRIFG) == 0);                                                   // Wait till new level reached
   }

   SVSMLCTL = SVSLE + SVSLRVL0 * level + SVMLE + SVSMLRRL0 * level;     // Set SVS/SVM low side to new level
   PMMCTL0_H = 0x00;                                                                                // Lock PMM registers for write access
}

static void pmm_setvcoredown (uint8_t level)
{
   PMMCTL0_H = PMMPW_H;                                                                          // Open PMM registers for write access (password)

   SVSMLCTL = SVSLE + SVSLRVL0 * level + SVMLE + SVSMLRRL0 * level;     // Set SVS/SVM low side to new level
   while ((PMMIFG & SVSMLDLYIFG) == 0);                                                     // Wait till SVM is settled

   PMMCTL0_L = PMMCOREV0 * level;                                                            // Set VCore to new level
   PMMIFG &= ~(SVMLVLRIFG + SVMLIFG);                                                    // Clear already set flags
   if ((PMMIFG & SVMLIFG)) {
      while ((PMMIFG & SVMLVLRIFG) == 0);                                                   // Wait till new level reached
   }

   PMMCTL0_H = 0x00;                                                                                // Lock PMM registers for write access
}

