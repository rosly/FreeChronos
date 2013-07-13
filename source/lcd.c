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
#include "lcd.h"

#include <stdlib.h>
#include <string.h>

/** \brief More convinient definitions for LCDBLKPRE which are writen to LCDBBLKCTL */
typedef enum {
   LCD_BLINKPRE_512 = 0,
   LCD_BLINKPRE_1024,
   LCD_BLINKPRE_2048,
   LCD_BLINKPRE_4096,
   LCD_BLINKPRE_8192,
   LCD_BLINKPRE_16384,
   LCD_BLINKPRE_32768,
   LCD_BLINKPRE_65536
} LCD_BlinkPre_T;

/* Definitions of LCD memory assigment peer each LCD symbol, for instance:
     LCD_SEG_L1_0 means first 7digit segment from right on first line - as we see it is driven by bits 5th memory cell
     LCD_SYMB_AM_MEM is AM/PM symbol, as we see it is driven by fibits in rst memory cell (0 offet) */
#define LCD_SEG_LX_INVALID         ((uint8_t)0xff)
#define LCD_SEG_L1_0_MEM           ((uint8_t)5)
#define LCD_SEG_L1_1_MEM           ((uint8_t)3)
#define LCD_SEG_L1_2_MEM           ((uint8_t)2)
#define LCD_SEG_L1_3_MEM           ((uint8_t)1)
#define LCD_SEG_L1_COL_MEM         ((uint8_t)0)
#define LCD_SEG_L1_DP1_MEM         ((uint8_t)0)
#define LCD_SEG_L1_DP0_MEM         ((uint8_t)4)
#define LCD_SEG_L2_0_MEM           ((uint8_t)7)
#define LCD_SEG_L2_1_MEM           ((uint8_t)8)
#define LCD_SEG_L2_2_MEM           ((uint8_t)9)
#define LCD_SEG_L2_3_MEM           ((uint8_t)10)
#define LCD_SEG_L2_4_MEM           ((uint8_t)11)
#define LCD_SEG_L2_5_MEM           ((uint8_t)11)
#define LCD_SEG_L2_COL1_MEM        ((uint8_t)0)
#define LCD_SEG_L2_COL0_MEM        ((uint8_t)4)
#define LCD_SEG_L2_DP_MEM          ((uint8_t)8)
#define LCD_SYMB_AM_MEM            ((uint8_t)0)
#define LCD_SYMB_PM_MEM            ((uint8_t)0)
#define LCD_SYMB_ARROW_UP_MEM      ((uint8_t)0)
#define LCD_SYMB_ARROW_DOWN_MEM    ((uint8_t)0)
#define LCD_SYMB_PERCENT_MEM       ((uint8_t)4)
#define LCD_SYMB_TOTAL_MEM         ((uint8_t)10)
#define LCD_SYMB_AVERAGE_MEM       ((uint8_t)9)
#define LCD_SYMB_MAX_MEM           ((uint8_t)7)
#define LCD_SYMB_BATTERY_MEM       ((uint8_t)6)
#define LCD_UNIT_L1_FT_MEM         ((uint8_t)4)
#define LCD_UNIT_L1_K_MEM          ((uint8_t)4)
#define LCD_UNIT_L1_M_MEM          ((uint8_t)6)
#define LCD_UNIT_L1_I_MEM          ((uint8_t)6)
#define LCD_UNIT_L1_PER_S_MEM      ((uint8_t)4)
#define LCD_UNIT_L1_PER_H_MEM      ((uint8_t)6)
#define LCD_UNIT_L1_DEGREE_MEM     ((uint8_t)4)
#define LCD_UNIT_L2_KCAL_MEM       ((uint8_t)6)
#define LCD_UNIT_L2_KM_MEM         ((uint8_t)6)
#define LCD_UNIT_L2_MI_MEM         ((uint8_t)6)
#define LCD_ICON_HEART_MEM         ((uint8_t)1)
#define LCD_ICON_STOPWATCH_MEM     ((uint8_t)2)
#define LCD_ICON_RECORD_MEM        ((uint8_t)0)
#define LCD_ICON_ALARM_MEM         ((uint8_t)3)
#define LCD_ICON_BEEPER1_MEM       ((uint8_t)4)
#define LCD_ICON_BEEPER2_MEM       ((uint8_t)5)
#define LCD_ICON_BEEPER3_MEM       ((uint8_t)6)

/* Definitions of LCD memory masks for each LCD symbol
    Those mask will be applied on memory cell, for instane if we whant to enable paticular symbol we will make OR operation, if we whant to disable we will habe to make AND with negation mask */
#define LCD_SEG_L1_MASK            (BIT2 + BIT1 + BIT0 + BIT7 + BIT6 + BIT5 + BIT4)
#define LCD_SEG_L1_0_MASK          (BIT2 + BIT1 + BIT0 + BIT7 + BIT6 + BIT5 + BIT4)
#define LCD_SEG_L1_1_MASK          (BIT2 + BIT1 + BIT0 + BIT7 + BIT6 + BIT5 + BIT4)
#define LCD_SEG_L1_2_MASK          (BIT2 + BIT1 + BIT0 + BIT7 + BIT6 + BIT5 + BIT4)
#define LCD_SEG_L1_3_MASK          (BIT2 + BIT1 + BIT0 + BIT7 + BIT6 + BIT5 + BIT4)
#define LCD_SEG_L1_COL_MASK        (BIT5)
#define LCD_SEG_L1_DP1_MASK        (BIT6)
#define LCD_SEG_L1_DP0_MASK        (BIT2)
#define LCD_SEG_L2_MASK            (BIT3 + BIT2 + BIT1 + BIT0 + BIT6 + BIT5 + BIT4)
#define LCD_SEG_L2_0_MASK          (BIT3 + BIT2 + BIT1 + BIT0 + BIT6 + BIT5 + BIT4)
#define LCD_SEG_L2_1_MASK          (BIT3 + BIT2 + BIT1 + BIT0 + BIT6 + BIT5 + BIT4)
#define LCD_SEG_L2_2_MASK          (BIT3 + BIT2 + BIT1 + BIT0 + BIT6 + BIT5 + BIT4)
#define LCD_SEG_L2_3_MASK          (BIT3 + BIT2 + BIT1 + BIT0 + BIT6 + BIT5 + BIT4)
#define LCD_SEG_L2_4_MASK          (BIT3 + BIT2 + BIT1 + BIT0 + BIT6 + BIT5 + BIT4)
#define LCD_SEG_L2_5_MASK          (BIT7)
#define LCD_SEG_L2_COL1_MASK       (BIT4)
#define LCD_SEG_L2_COL0_MASK       (BIT0)
#define LCD_SEG_L2_DP_MASK         (BIT7)
#define LCD_SYMB_AM_MASK           (BIT1 + BIT0)
#define LCD_SYMB_PM_MASK           (BIT0)
#define LCD_SYMB_ARROW_UP_MASK     (BIT2)
#define LCD_SYMB_ARROW_DOWN_MASK   (BIT3)
#define LCD_SYMB_PERCENT_MASK      (BIT4)
#define LCD_SYMB_TOTAL_MASK        (BIT7)
#define LCD_SYMB_AVERAGE_MASK      (BIT7)
#define LCD_SYMB_MAX_MASK          (BIT7)
#define LCD_SYMB_BATTERY_MASK      (BIT7)
#define LCD_UNIT_L1_FT_MASK        (BIT5)
#define LCD_UNIT_L1_K_MASK         (BIT6)
#define LCD_UNIT_L1_M_MASK         (BIT1)
#define LCD_UNIT_L1_I_MASK         (BIT0)
#define LCD_UNIT_L1_PER_S_MASK     (BIT7)
#define LCD_UNIT_L1_PER_H_MASK     (BIT2)
#define LCD_UNIT_L1_DEGREE_MASK    (BIT1)
#define LCD_UNIT_L2_KCAL_MASK      (BIT4)
#define LCD_UNIT_L2_KM_MASK        (BIT5)
#define LCD_UNIT_L2_MI_MASK        (BIT6)
#define LCD_ICON_HEART_MASK        (BIT3)
#define LCD_ICON_STOPWATCH_MASK    (BIT3)
#define LCD_ICON_RECORD_MASK       (BIT7)
#define LCD_ICON_ALARM_MASK        (BIT3)
#define LCD_ICON_BEEPER1_MASK      (BIT3)
#define LCD_ICON_BEEPER2_MASK      (BIT3)
#define LCD_ICON_BEEPER3_MASK      (BIT3)

/* Map of common 7segment mapping for every digit on LCD
    Those defines will be used in font definitions for each char */
#define LCD_SEG_A                  (BIT4)
#define LCD_SEG_B                  (BIT5)
#define LCD_SEG_C                  (BIT6)
#define LCD_SEG_D                  (BIT7)
#define LCD_SEG_E                  (BIT2)
#define LCD_SEG_F                  (BIT0)
#define LCD_SEG_G                  (BIT1)
/** \brief Definition of the font for 7segment symbols
 * \note we can use this kind of aproatch becose we have quite consistent bits mapping for 7segments in both lines, the only issue is that for line 2 we will have to swab the nibbles in byte
 *   A
 * F   B
 *   G
 * E   C
 *   D
 */
static uint8_t const lcd_font[] = {
   0,                                                                                 //Display Space
   LCD_SEG_B | LCD_SEG_C,                                                             //Display !
   0, //"
   0, //#
   0, //$
   0, //%
   0, //&
   0, //'
   LCD_SEG_E | LCD_SEG_F, //(
   LCD_SEG_B | LCD_SEG_C, //)
   0, //*
   0, //+
   0, //,
   LCD_SEG_G, //-
   0, //.
   0, // /
   LCD_SEG_A | LCD_SEG_B | LCD_SEG_C | LCD_SEG_D | LCD_SEG_E | LCD_SEG_F,             // Displays "0"
   LCD_SEG_B | LCD_SEG_C,                                                             // Displays "1"
   LCD_SEG_A | LCD_SEG_B | LCD_SEG_D | LCD_SEG_E | LCD_SEG_G,                         // Displays "2"
   LCD_SEG_A | LCD_SEG_B | LCD_SEG_C | LCD_SEG_D | LCD_SEG_G,                         // Displays "3"
   LCD_SEG_B | LCD_SEG_C | LCD_SEG_F | LCD_SEG_G,                                     // Displays "4"
   LCD_SEG_A | LCD_SEG_C | LCD_SEG_D | LCD_SEG_F | LCD_SEG_G,                         // Displays "5"
   LCD_SEG_A | LCD_SEG_C | LCD_SEG_D | LCD_SEG_E | LCD_SEG_F | LCD_SEG_G,             // Displays "6"
   LCD_SEG_A | LCD_SEG_B | LCD_SEG_C,                                                 // Displays "7"
   LCD_SEG_A | LCD_SEG_B | LCD_SEG_C | LCD_SEG_D | LCD_SEG_E | LCD_SEG_F | LCD_SEG_G, // Displays "8"
   LCD_SEG_A | LCD_SEG_B | LCD_SEG_C | LCD_SEG_D | LCD_SEG_F | LCD_SEG_G,             // Displays "9"
   0, //:
   0, //;
   0, //<
   LCD_SEG_D | LCD_SEG_G, // =
   0, //>
   0, //?
   0, //@
   LCD_SEG_A + LCD_SEG_B + LCD_SEG_C + LCD_SEG_E + LCD_SEG_F + LCD_SEG_G,                                                                  // Displays "A"
   LCD_SEG_C + LCD_SEG_D + LCD_SEG_E + LCD_SEG_F + LCD_SEG_G,                 // Displays "b"
   LCD_SEG_A + LCD_SEG_D + LCD_SEG_E + LCD_SEG_F,                         // Displays "C"
   LCD_SEG_B + LCD_SEG_C + LCD_SEG_D + LCD_SEG_E + LCD_SEG_G,                 // Displays "d"
   LCD_SEG_A + +LCD_SEG_D + LCD_SEG_E + LCD_SEG_F + LCD_SEG_G,                // Displays "E"
   LCD_SEG_A + LCD_SEG_E + LCD_SEG_F + LCD_SEG_G,                         // Displays "F"
   //  LCD_SEG_A+      LCD_SEG_C+LCD_SEG_D+LCD_SEG_E+LCD_SEG_F+LCD_SEG_G,         // Displays "G"
   LCD_SEG_A + LCD_SEG_B + LCD_SEG_C + LCD_SEG_D + LCD_SEG_F + LCD_SEG_G,         // Displays "g"
   LCD_SEG_B + LCD_SEG_C + LCD_SEG_E + LCD_SEG_F + LCD_SEG_G,                 // Displays "H"
   LCD_SEG_E + LCD_SEG_F,                                         // Displays "I"
   LCD_SEG_A + LCD_SEG_B + LCD_SEG_C + LCD_SEG_D,                         // Displays "J"
   //              LCD_SEG_B+LCD_SEG_C+      LCD_SEG_E+LCD_SEG_F+LCD_SEG_G,   // Displays "k"
   LCD_SEG_D + LCD_SEG_E + LCD_SEG_F + LCD_SEG_G,                         // Displays "k"
   LCD_SEG_D + LCD_SEG_E + LCD_SEG_F,                                 // Displays "L"
   LCD_SEG_A + LCD_SEG_B + LCD_SEG_C + LCD_SEG_E + LCD_SEG_F,                 // Displays "M"
   LCD_SEG_C + LCD_SEG_E + LCD_SEG_G,                                 // Displays "n"
   LCD_SEG_C + LCD_SEG_D + LCD_SEG_E + LCD_SEG_G,                         // Displays "o"
   LCD_SEG_A + LCD_SEG_B + LCD_SEG_E + LCD_SEG_F + LCD_SEG_G,                 // Displays "P"
   LCD_SEG_A + LCD_SEG_B + LCD_SEG_C + LCD_SEG_D + LCD_SEG_E + LCD_SEG_F,         // Displays "Q"
   LCD_SEG_E + LCD_SEG_G,                                         // Displays "r"
   LCD_SEG_A + LCD_SEG_C + LCD_SEG_D + LCD_SEG_F + LCD_SEG_G,                 // Displays "S"
   LCD_SEG_D + LCD_SEG_E + LCD_SEG_F + LCD_SEG_G,                         // Displays "t"
   LCD_SEG_C + LCD_SEG_D + LCD_SEG_E,                                 // Displays "u"
   LCD_SEG_C + LCD_SEG_D + LCD_SEG_E,                                 // Displays "u"
   LCD_SEG_G,                                                 // Displays "-"
   LCD_SEG_B + LCD_SEG_C + +LCD_SEG_E + LCD_SEG_F + LCD_SEG_G,                // Displays "X"
   LCD_SEG_B + LCD_SEG_C + LCD_SEG_D + LCD_SEG_F + LCD_SEG_G,                 // Displays "Y"
   LCD_SEG_A + LCD_SEG_B + LCD_SEG_D + LCD_SEG_E + LCD_SEG_G,                 // Displays "Z"
   0, // Last symbol ... must be empty = space
};

/** \brief Map of two lines mempry offsets and their 7segment masks
  * This map will be used to display string in lines */
static const uint8_t lcd_linemap[2][5]= {
   /* mapping of first line, it has only 4 digits */
   { LCD_SEG_LX_INVALID, /* this will allow us to use mapping for artifical 5 digit of line 1, offset 0xff will be checked before write to memory */
      LCD_SEG_L1_3_MEM,
      LCD_SEG_L1_2_MEM,
      LCD_SEG_L1_1_MEM,
      LCD_SEG_L1_0_MEM, },
   /* mapping of first line, it has 6 digits */
   { LCD_SEG_L2_4_MEM,
      LCD_SEG_L2_3_MEM,
      LCD_SEG_L2_2_MEM,
      LCD_SEG_L2_1_MEM,
      LCD_SEG_L2_0_MEM, }
};


void LCD_Init()
{
   //Enabbling the LCD module with MUX4 sourced from ACLCK
   //LCD_FREQ = ACLK/16/8 = 256Hz so for Mux4 fFrame will be 256Hz/4 = 64Hz
   LCDBCTL0 = LCDDIV_15 | LCDPRE_3 | LCD4MUX | LCDON | LCDSON; /* LCDDIV_15 will be +1 to 16, LCDSSEL not set means ACLK */
   LCDBBLKCTL = LCDBLKMOD_0; /* initialy disabling the blinking so the blinking frequency will be also reseted */

   // I/O to COM outputs
   P5SEL |= (BIT5 | BIT6 | BIT7);
   P5DIR |= (BIT5 | BIT6 | BIT7);

   // Activate LCD outputs
   LCDBPCTL0 = 0xFFFF;         // Select LCD segments S0-S15
   LCDBPCTL1 = 0x00FF;         // Select LCD segments S16-S22

#ifdef USE_LCD_CHARGE_PUMP
   // Charge pump voltage generated internally, internal bias (V2-V4) generation
   LCDBVCTL = LCDCPEN | VLCD_2_72;
#endif

   LCDBMEMCTL |= LCDCLRBM + LCDCLRM; // Clear display memory and blinking memory, LCDDISP cannot be changed so we make a OR op */
}

void LCD_BlinkSet(LCD_BlinkMode_T mode, LCD_BlinkFreq_T freq)
{
   /* from slau259.pdf The blinking frequency must be smaller than the frame frequency, fFrame.
       The blinking frequency should be changed only when LCDBLKMODx = 00.
       LCB_BLK_FREQ = ACLK/(LCDBLKDIV + 1)/LCD_BlinkPre_T */

   LCDBBLKCTL = LCDBLKMOD_0; /* disabling the blinking, this will also clear the div and prescaler bits acording to spec */
   LCDBBLKCTL = (LCDBLKDIV0 * freq) | (LCD_BLINKPRE_8192 * LCDBLKPRE0) | (mode * LCDBLKMOD_1); /* setting the frequency and blinking mode */
}

void LCD_Print(uint8_t line, uint8_t left, const char *str, uint8_t len)
{
   uint8_t i;
   uint8_t symbol;
   uint8_t *mem;
   uint8_t segbits;
   uint8_t mask= ((0 == line) ? LCD_SEG_L1_MASK : LCD_SEG_L2_MASK);

   /* iterate through linemap to obtain the memory ofset where we need to write the symbol
       bits which should be enabled are obtained by reading the lcd_font for proper symbol
       then the bits are masked by memory mask, so we dont modify the remaining bits */
   for(i = 0; i < len; i++) {
      symbol = max(str[i], ' '); /* symbols with code less than space will be converted to space */
      if( symbol > 'Z' ) {
         symbol = min((symbol & (~0x20)), '['); /* symbols with code greater than 'Z' are probably the lower case letters so we make them upper case, additionaly in case of symbols grater than '[' we limit them to '[' which will be printed as space */
      }
      segbits = lcd_font[symbol - ' '];                                                                         /* obtain the symbol bits from font */
      if( 0 != line ) {
         segbits = ((segbits << 4) & 0xF0) | ((segbits >> 4) & 0x0F);                            /*in case of line2 swap the nibles!!! */
      }
      mem = (uint8_t*)LCDMEM + lcd_linemap[line][i + left];                                      /* get the memory addres where the symbol need to be writen */
      *mem = ((*mem) & (~mask)) | (segbits & mask);
   }
}

void LCD_PrintS(uint8_t line, const char *str, LCD_PrintOpt_T opt)
{
   uint8_t len;
   uint8_t left;
   uint8_t i;
   uint8_t linelen = ((0 == line) ? 4u : 5u);
   uint8_t linebeg = ((0 == line) ? 1u : 0u);
   uint8_t *mem;
   uint8_t mask= ((0 == line) ? LCD_SEG_L1_MASK : LCD_SEG_L2_MASK);

   /* first calculate the position of the text depending on passed opt */
   len = min(strlen(str), linelen); /* calculate the string len and limit it to 5 or 4 chars */
   switch( opt & (LCD_OPT_ALLIGNLEFT |LCD_OPT_ALLIGNCENTER | LCD_OPT_ALLIGNRIGHT) ) {
      case LCD_OPT_ALLIGNLEFT:
         left = linebeg;
         break;
      case LCD_OPT_ALLIGNCENTER:
         left = ((linelen - len) / 2) + linebeg;
         break;
      case LCD_OPT_ALLIGNRIGHT:
      default:
         left = 5u - len;
         break;
   }

   /* clear the line if not forbiden */
   if(0 == (opt & LCD_OPT_DONTCLEAR)) {
      for(i = linebeg; i < 5u; i++) {
         mem = (uint8_t*)LCDMEM + lcd_linemap[line][i];
         *mem &= (~mask);
      }
   }

   LCD_Print(line, left, str, len);
}

void LCD_SwitchHeart(bool heart)
{
    uint8_t *mem;
    uint8_t offset;
    uint8_t mask;

    mask = (heart) ? LCD_ICON_HEART_MASK : LCD_ICON_STOPWATCH_MASK;
    offset = (heart) ? LCD_ICON_HEART_MEM : LCD_ICON_STOPWATCH_MEM;

    mem = (uint8_t*)LCDMEM + offset;
    if( *mem & mask )
        *mem &= (~mask);
    else
        *mem |= mask;

    //*((uint8_t*)LCDMEM + LCD_ICON_RECORD_MEM) = LCD_ICON_RECORD_MASK;
}

#if 0 /* simplified number printing function, comparing to snprintf it get 592 bytes of code less, but the benefit is not worth the confucion made by strange looking API */
void LCD_PrintU(uint8_t line, uint8_t left, uint16_t val, uint8_t radix, int8_t fields)
{
   char tmp[17]; /* for utoa 17 is the maximal string length if radix is 2, see the stdlib.c from msp430-libc */
   uint8_t len;
   uint8_t off;

   fields = max(min(fields, (int8_t)sizeof(tmp) - 1), -(int8_t)sizeof(tmp) + 1); /* normalizing the param */

   utoa(val, tmp, radix);
   len = min(strlen(tmp), sizeof(tmp)); // workaround for msp43-gcc bug https://sourceforge.net/tracker/index.php?func=detail&aid=3433730&group_id=42303&atid=432701
    if( 0 != fields ) {
      if(len > abs(fields) ) {
         /* if length of the string is greater than the fields param, string will be truncated */
         tmp[abs(fields)] = 0;
         len = abs(fields);
      }

      /* if fields != 0 means that we need to add the spaces or '0' before the printed number */
      off = abs(fields) - len;
      if( 0 != off ) { /* chek if we need to do anything */
         memmove(&tmp[off], tmp, len + 1); /* len + 1 because coping also the ending 0 */
         memset(tmp, (fields < 0) ? ' ' : '0', off);
         len += off;
      }
   }
   LCD_Print(line, left, tmp, len);
}
#endif

