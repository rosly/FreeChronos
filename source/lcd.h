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

#ifndef __LCD_
#define __LCD_

typedef enum {
   LCD_BLINKMODE_OFF = 0,        /* LCDBLKMOD_0 */
   LCD_BLINKMODE_INDIVIDUAL,  /* LCDBLKMOD_1 */
   LCD_BLINKMODE_ALL,              /* LCDBLKMOD_2 */
   LCD_BLINKMODE_SWITCHING   /* LCDBLKMOD_3 */
} LCD_BlinkMode_T;

typedef enum {
   LCS_BLINKFREQ_4HZ   = 0, /* LCDBLKDIV = 0 + 1 */
   LCS_BLINKFREQ_2HZ   = 1, /* LCDBLKDIV = 1 + 1 */
   LCS_BLINKFREQ_1HZ   = 3, /* LCDBLKDIV = 3 + 1 */
   LCS_BLINKFREQ_05HZ = 7  /* LCDBLKDIV = 7 + 1 */
} LCD_BlinkFreq_T;

typedef enum {
   LCD_OPT_NONE     = 0,

   LCD_OPT_ALLIGNLEFT       = 1 << 1,
   LCD_OPT_ALLIGNCENTER  = 1 << 2,
   LCD_OPT_ALLIGNRIGHT    = 1 << 3,

   LCD_OPT_DONTCLEAR      = 1 << 4,
} LCD_PrintOpt_T;

void LCD_Init();

void LCD_BlinkSet(LCD_BlinkMode_T mode, LCD_BlinkFreq_T freq);

void LCD_Print(uint8_t line, uint8_t left, const char *str, uint8_t len);

void LCD_PrintS(uint8_t line, const char *str, LCD_PrintOpt_T opt);

void LCD_SwitchHeart(bool heart);

//void LCD_PrintU(uint8_t line, uint8_t left, uint16_t val, uint8_t radix, int8_t fields);

#endif /* __LCD_ */

