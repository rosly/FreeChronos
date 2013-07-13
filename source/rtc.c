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
#include "rtc.h"

//#include "pmm.h"
#include "lcd.h" /* remove later used for RtC_SwitchHeart */

/** Design of this module base on following usage scenario of rtC HW:
 * - ech second the interupt from RTCRDYIFG will triger the copy of the date and time from RTC HW into SW buffer
 * - this copy will be consistent because the rising edge of RTCRDY is trigered right after seond update. The interupt wil have whole second to copy the data (of course this will take several uS)
 * - seconds minutes hours and etc are read from RTC registers, miliseconds are calculated basing on value read from RT1PS
 * - in case of milisecond resolution is needed, for instance to triget the milisecond updates for sport time, we will use the interupt from RT1SIFG
 * - correction for cristal frequency offset and temperature compensation ill be done with calibration logic HW with +4ppm and -2pmm steps
 * - it is planed to store thecalibration data outside this moule and then pass them in time of init. Calibration params will remove the static frequency offset
 * -each 10s we wll read the temperature (on some other interpt) and pass this temp into RtC to compensate the frequency of the cristal
 */

typedef struct {
   RTC_DayTime_T daytime;
   int8_t callinit; //zero callibration offset  in 25 degres, positive values steps are in 4ppm, negative are -2ppm steps
} RTC_T;

static RTC_T rtc;
static RTC_Callback_T rtc_clbck = NULL;

void RTC_Init(int8_t callinit)
{
   //init the rtc struct
   //memset(&rtc, 0, sizeof(rtc));
   rtc.callinit = callinit;

   RTCCTL01 = RTCMODE; // set the RTC mode this will also reset the time and date registers
   RTCCTL23 = ((rtc.callinit > 0 ? 1 : 0) * RTCCALS) | (rtc.callinit & (RTCCAL0 | RTCCAL1 | RTCCAL2 | RTCCAL3 | RTCCAL4 | RTCCAL5)) | RTCCALF_0; //set calibration offset and disable callibration output on RTCCLK pin, may be changed latter if external callibration need to be done
   RTCPS0CTL &= (~RT0PSIE); //to make sure disble prescaler0 interupts
   RTCPS1CTL &= (~RT1PSIE); //to make sure disble prescaler1 interupts
   RTCCTL01 |= RTCRDYIE; //enable RTC Ready Interrupt
}

void RTC_Set(const RTC_DayTime_T *day_time)
{
   rtc.daytime = *day_time; //copy the data into rtc struct

   //set the time and date in HW
   RTCCTL01 |= RTCHOLD; //hold the whole RTC //RTCCTL01_H |=RTCHOLD_H;
   RTCPS = (uint16_t)(((uint32_t)(rtc.daytime.msec) * 0x8000u) / 1000u); /* miliseconds, need to be writen into prescaler registers, there is no special register for miliseconds so we need to calculate the value, prescalers generate te signal to RTU each 0x8000u value (period for 1 second) */
   RTCSEC = rtc.daytime.sec; //seconds
   RTCMIN = rtc.daytime.min; //minutes
   RTCHOUR = rtc.daytime.hour; //hours
   RTCDOW = rtc.daytime.dow; //day of week
   RTCDAY = rtc.daytime.day; //day of month
   RTCMON = rtc.daytime.month; //month
   RTCYEAR = rtc.daytime.year; //year
   RTCCTL01 &= (~RTCHOLD); //start RTC //RTCCTL01_H &= (~RTCHOLD_H);
}

void RTC_Get(RTC_DayTime_T *day_time)
{
   //in this function we copy the modt recent day_time, it was updated durring the RTC_Interrupt
   *day_time = rtc.daytime; //struct copy
}

void RTC_SetMsMode(RTC_MsMode_T mode)
{
   if( RTC_MSMODE_DISABLE == mode ) {
      RTCPS1CTL &= (~RT1PSIE); //disble RTC prescaler1 interupts
      RTCPS0CTL &= (~RT0PSIE); //disble RTC prescaler0 interupts
      RTCCTL01 |= RTCRDYIE; //enable RTC Ready Interrupt
   } else if( mode >= RTC_MSMODE_64 ) {
      RTCCTL01 &= (~RTCRDYIE); //disable RTC Ready Interrupt
      RTCPS0CTL &= (~RT0PSIE); //disble RTC prescaler0 interupts
      RTCPS1CTL = ((mode - RTC_MSMODE_64) * RT1IP0) | RT1PSIE; //set the prescaler1 interupt interval and enable flag
   } else {
      RTCCTL01 &= (~RTCRDYIE); //disable RTC Ready Interrupt
      RTCPS1CTL &= (~RT1PSIE); //disble RTC prescaler1 interupts
      RTCPS0CTL |= ((mode - RTC_MSMODE_1024) * RT0IP0) | RT0IP2 | RT0PSIE; //set the prescaler0 interupt interval and enable flag, RT0IP2 must be set for proper interval calculations
   }
}

void RTC_SetClbck(RTC_Callback_T clbck)
{
   rtc_clbck = clbck;
}

OS_ISR(RTC, RTC_Interrupt) //uses RTC_VECTOR define, RTCIV; here may have the values (0x02 RTC ready) (0x08 prescaler0) (0x0A prescaler1)
{
   arch_contextstore_i(RTC); /* place the ISR entering code - we need to do that because this ISR calls the OS functions */

   rtc.daytime.msec = (uint16_t)((((uint32_t)(RTCPS & 0x7fff)) * 1000u) / 0x8000u);
   rtc.daytime.sec = RTCSEC; //seconds
   rtc.daytime.min = RTCMIN; //minutes
   rtc.daytime.hour = RTCHOUR; //hours
   rtc.daytime.dow = RTCDOW; //day of week
   rtc.daytime.day = RTCDAY; //day of month
   rtc.daytime.month = RTCMON; //month
   rtc.daytime.year = RTCYEAR; //year

   RTCPS0CTL_L &= (~RT0PSIFG_L); // disable the prescaler 0 interupt flag
   RTCPS1CTL_L &= (~RT1PSIFG_L); // disable the prescaler 1 interupt flag
   RTCCTL01_L &= (~RTCRDYIFG_L); //disable the RTCRDYIFG

   os_tick();
   LCD_SwitchHeart(true);
   if( NULL != rtc_clbck ) {
      rtc_clbck();
   }

   arch_contextrestore_i(RTC); /* place the ISR exiting code - this will eventualy pop dierent task then which was on ISR enter */
}

