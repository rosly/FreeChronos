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
#include "ucs.h"

#define UCS_XT1_PINS (uint8_t)(BIT0 | BIT1)
#define UCS_XT1_PORT P5SEL

void UCS_XT1Set(bool enable, UCS_X1_DrvStrght_T drvstrght, UCS_X1Cap_T cap)
{
   uint16_t tmp; //temporary for one shot register write

   if( enable ) {
      UCS_XT1_PORT |= UCS_XT1_PINS;           // Switch port to peripheral mode
      tmp = UCSCTL6 & (~XT1OFF);                   // XT1 On
   } else {
      UCS_XT1_PORT &= (~UCS_XT1_PINS);      // Switch port to IO mode
      tmp = UCSCTL6 | XT1OFF;                        // XT1 Off
   }
   tmp &= (~(XCAP0_L | XCAP1_L | XT1DRIVE0_L | XT1DRIVE1_L)); //mask out the XCAP and XDRIVE bits
   tmp |= (XT1DRIVE_1 * drvstrght) | (XCAP_1 * cap);                     //set the XCAP and XDRIVE bits
   UCSCTL6 = tmp;                                                                         //write to register in one shot
}

#if 0
void UCS_SourceGet(UCS_ClockSource_T *src)
{
   static const uint8_t ucs_fllref_conv1[] = {
      [SELREF__XT1CLK >> 4]   = UCS_CLOCK_XT1CLK,
      [SELREF_1 >> 4]              = UCS_CLOCK_XT1CLK,
      [SELREF__REFOCLK >> 4] = UCS_CLOCK_REFOCLK,
      [SELREF_3 >> 4]              = UCS_CLOCK_REFOCLK,
      [SELREF_4 >> 4]              = UCS_CLOCK_REFOCLK,
      [SELREF__XT2CLK >> 4]   = UCS_CLOCK_XT2CLK,
      [SELREF_6 >> 4]              = UCS_CLOCK_XT2CLK,
      [SELREF_7 >> 4]              = UCS_CLOCK_NONE, /* UCS_CLOCK_XT2CLK in case 'F543x and 'F541x non-A versions */
   };
   uint16_t tmp;

   src->fllref= ucs_fllref_conv1[(UCSCTL3 & (SELREF0 | SELREF1 | SELREF2)) >> 4];
   tmp = UCSCTL4;
   src->aclk    = tmp & (SELA0 | SELA1 | SELA2);
   src->smclk  = tmp & (SELS0 | SELS1 | SELS2);
   src->mclk   = tmp & (SELM0 | SELM1 | SELM2);
}
#endif

void UCS_SourceSet(const UCS_ClockSource_T *src)
{
   static const uint8_t ucs_fllref_conv2[] = {
      [UCS_CLOCK_XT1CLK]        = SELREF__XT1CLK,
      [UCS_CLOCK_VLOCLK]        = SELREF_7, /* invalid setting */
      [UCS_CLOCK_REFOCLK]      = SELREF__REFOCLK,
      [UCS_CLOCK_DCOCLK]       = SELREF_7, /* invalid setting */
      [UCS_CLOCK_DCOCLKDIV] = SELREF_7, /* invalid setting */
      [UCS_CLOCK_XT2CLK]        = SELREF__XT2CLK,
      [UCS_CLOCK_NONE]           = SELREF_7, /* SELREF__XT2CLK in case 'F543x and 'F541x non-A versions */
   };
   uint16_t tmp;

   tmp = UCSCTL3 & (~(SELREF0 | SELREF1 | SELREF2)); //mask out the SELREF bits
   tmp |= ucs_fllref_conv2[src->fllref];                            //set SELREF setting bits
   UCSCTL3 = tmp;                                                         //store FLLREF soure in register

   UCSCTL4 = (src->aclk * SELA_1) | (src->smclk * SELS_1) | (src->mclk * SELM_1);
}

void UCS_FllSet(const UCS_FllSetting_T *set)
{
   uint16_t tmp;

   _BIS_SR(SCG0);                                                                      // Disable the FLL control loop

   tmp = UCSCTL3 & (~(FLLREFDIV0 | FLLREFDIV1 | FLLREFDIV2)); //mask out the FLLREFDIV bits
   tmp |= set->fllrefdiv * FLLREFDIV__2;                                       //set FLLREFDIV setting bits
   UCSCTL3 = tmp;                                                                      //store FLLREFDIV in register

   UCSCTL0 = 0x0000;                                                                 // Set lowest possible DCOx, MODx
   UCSCTL1 = (set->dcorsel * DCORSEL_1) | (set->dismod * DISMOD); // Select suitable DCO range and enable/disable modulator
   UCSCTL2 = (set->flld * FLLD_1) + set->flln;                             // Set DCO Multipliers

   _BIC_SR(SCG0);                                                                     // Enable the FLL control loop

   // Worst-case settling time for the DCO when the DCO range bits have been
   // changed is n x 32 x 32 x f_MCLK / f_FLL_reference. See UCS chapter in 5xx
   // UG for optimization.
   // 32 x 32 x 12 MHz / 32,768 Hz = 375000 = MCLK cycles for DCO to settle
   //__delay_cycles(375000);
   __delay_cycles(100000);
   __delay_cycles(100000);
   __delay_cycles(100000);
   __delay_cycles(75000);
}

void UCS_WaitStab(void)
{
   // Loop until XT1 & DCO stabilizes, use do-while to insure that
   // body is executed at least once
   do
   {
      UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + XT1HFOFFG + DCOFFG);
      SFRIFG1 &= ~OFIFG;      // Clear fault flags
   } while ((SFRIFG1 & OFIFG));
}

