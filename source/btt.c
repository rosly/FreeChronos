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
#include <isr_compat.h>
#include "os.h"
#include "btt.h"

#include "lcd.h" /* remove later */

#define BTT_STAR_BIT         (BIT2)
#define BTT_NUM_BIT           (BIT1)
#define BTT_UP_BIT             (BIT4)
#define BTT_DOWN_BIT        (BIT0)
#define BTT_BKLT_BIT         (BIT3)
#define BTT_ALL_BIT            (BTT_STAR_BIT | BTT_NUM_BIT | BTT_UP_BIT | BTT_DOWN_BIT | BTT_BKLT_BIT)

void BTT_Init(void)
{
    // Set button ports to input
    P2DIR &= ~BTT_ALL_BIT;

    // Enable internal pull-downs
    P2OUT &= ~BTT_ALL_BIT;
    P2REN |= BTT_ALL_BIT;

    // IRQ triggers on rising edge
    P2IES &= ~BTT_ALL_BIT;

    // Reset IRQ flags
    P2IFG &= ~BTT_ALL_BIT;

    // Enable button interrupts
    P2IE |= BTT_ALL_BIT;
}

void BTT_Get(BTT_Act_T *act)
{
    *act = 0;
}

/* PORT2_VECTOR */
OS_ISR(PORT2, BTT_Interrupt)
{
   arch_contextstore_i(RTC); /* place the ISR entering code - we need to do that because this ISR calls the OS functions */

   P2IFG &= ~BTT_ALL_BIT;  // Reset IRQ flags
   eint(); /* allow for nested interrups */
   LCD_SwitchHeart(false);

   arch_contextrestore_i(RTC); /* place the ISR exiting code - this will eventualy pop dierent task then which was on ISR enter */
}

