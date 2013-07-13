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

#ifndef __RTC_
#define __RTC_

typedef struct {
   uint16_t msec; //miliseconds
   uint8_t sec; //seconds
   uint8_t min; //minutes
   uint8_t hour; //hours
   uint8_t dow; //day of week
   uint8_t day; //day of month
   uint8_t month; //month
   uint16_t year; //year
} RTC_DayTime_T;

typedef enum {
   RTC_MSMODE_DISABLE = 0, /**< miliseond mode disabled */
   RTC_MSMODE_1024,              /**<1024 Interrupts peer second - <1ms perods */
   RTC_MSMODE_512,                /**<512 Interrupts peer second - ~2ms periods */
   RTC_MSMODE_256,              /**<256 Interrupts peer second - ~4ms perods */
   RTC_MSMODE_128,              /**<128 Interrupts peer second - ~8ms perods */
   RTC_MSMODE_64,                /**<64 Interrupts peer second - ~16ms periods */
   RTC_MSMODE_32,                /**<32 Interrupts peer second - ~32 ms periods */
   RTC_MSMODE_16                 /**<16 Interrupts peer second 0- ~64ms periods */
} RTC_MsMode_T;

typedef void (*RTC_Callback_T)(void);

void RTC_Init(int8_t callinit);

void RTC_Set(const RTC_DayTime_T *day_time);

void RTC_Get(RTC_DayTime_T *day_time);

void RTC_SetMsMode(RTC_MsMode_T mode);

void RTC_SetClbck(RTC_Callback_T clbck);

#endif /* __RTC_ */

