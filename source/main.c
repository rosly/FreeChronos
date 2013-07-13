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
#include "wtdg.h"
#include "pmm.h"
#include "ucs.h"
#include "lcd.h"
#include "rtc.h"
#include "btt.h"
#include "os.h"

#include <stdio.h>

static os_task_t task1;
static os_task_t task2;
static uint16_t task1_stack[100];
static uint16_t task2_stack[100];
static os_sem_t sem1;
static os_sem_t sem2;

__attribute__((naked, section(".init3"))) void __low_level_init()
{
   WTDG_Init();
   PMM_SetVCore(PMM_LEVEL_24);
   PMM_SetHiPower(true);
   UCS_XT1Set(true, UCS_X1_DRVSTRGHT_MAX, UCS_X1CAP_12PF);
   static const UCS_ClockSource_T src = {
      .fllref =UCS_CLOCK_XT1CLK,
      .aclk = UCS_CLOCK_XT1CLK, // SET ACLK to 32,768kHz from XT1
      .smclk = UCS_CLOCK_DCOCLKDIV,
      .mclk = UCS_CLOCK_DCOCLKDIV };
   UCS_SourceSet(&src);
   static const UCS_FllSetting_T fllset = {
      .fllrefdiv = UCS_FLLREFDIV_1,
      .dcorsel = UCS_DCORANGE_5,
      .flld = UCS_FLLD_2,
      .flln = 366, /* from orginal code as 0x16E as (12MHz / 32768) - 1, *D=2) is not included because we calculate the DCODIV freq*/
      .dismod = false };
    UCS_FllSet(&fllset); // Configure CPU clock for 12MHz
    UCS_WaitStab(); //Wait until XT1 & DCO stabilize
}

void idle(void)
{
   PMM_LowPower(PMM_LPM3);
}

int task1_proc(void* param)
{
   os_retcode_t ret;
   uint16_t msec;
   uint16_t diff;
   RTC_DayTime_T daytime;
   char tmp[17];

   param = param;
   while(1)
   {
      //os_sem_up(&sem2);
      ret = os_sem_down(&sem1, OS_SEMTIMEOUT_INFINITE);
      assert(OS_OK != ret);

      msec = daytime.msec;
      RTC_Get(&daytime);
      diff = daytime.msec - msec;

      snprintf(tmp, sizeof(tmp), "%02u%03u", daytime.sec, diff);
      LCD_PrintS(1, tmp, LCD_OPT_NONE);
   }
}

int task2_proc(void* param)
{
   os_retcode_t ret;
   RTC_DayTime_T daytime;
   char tmp[17];

   param = param;
   while(1)
   {
      //os_sem_up(&sem1);
      ret = os_sem_down(&sem2, OS_SEMTIMEOUT_INFINITE);
      assert(OS_OK != ret);
      RTC_Get(&daytime);
      snprintf(tmp, sizeof(tmp), "%02u%02u", daytime.min, daytime.sec);
      LCD_PrintS(0, tmp, LCD_OPT_NONE);
   }
}

static void rtc_callback(void)
{
   os_sem_up(&sem1);
   os_sem_up(&sem2);
}

void init(void)
{
   static const RTC_DayTime_T daytime = {
      .msec = 0,
      .sec = 0,
      .min = 30,
      .hour = 0,
      .dow = 1,
      .day = 1,
      .month = 1,
      .year = 2011,
   };

   RTC_Init(0);
   RTC_Set(&daytime);
   BTT_Init();
   LCD_Init();
   //LCD_BlinkSet(LCD_BLINKMODE_ALL, LCS_BLINKFREQ_4HZ);
   RTC_SetMsMode(RTC_MSMODE_16); //RTC_MSMODE_256 ??
   RTC_SetClbck(rtc_callback);

   os_sem_create(&sem1, 0);
   os_sem_create(&sem2, 0);

   os_task_create(&task1, 1, task1_stack, sizeof(task1_stack), task1_proc, NULL);
   os_task_create(&task2, 1, task2_stack, sizeof(task2_stack), task2_proc, NULL);
}

int main(void)
{
   os_start(init, idle);
   return 0;
}

void NORETURN COLD halt(void)
{
   dint();
   PMM_LowPower(PMM_LPM0);
   while(1); /* supressig the compiler warnings, also securing if in some magical way somebody wake up the CPU while the interupts are disabled (NMI?) */
}

