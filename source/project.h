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

#ifndef __PROJECT_
#define __PROJECT_

//#define __CC430F6137__ this deffine should be automaticly added by compiler when it will read the -mcu=cc430f6137, this define is required for legacymsp430.h

#include <legacymsp430.h>
#include <stdint.h> /* for patform specific types definitions like uint16_t */
#include <limits.h> /* for UINT_MAX etc */
#include <stddef.h>

typedef uint8_t bool;
#define false ((uint8_t)0)
#define true  ((uint8_t)1) /* considered also !false but finaly decided to be more strict */

#define COLD __attribute__ ((cold))
#define NORETURN __attribute__ ((noreturn))

void NORETURN COLD halt(void);

#define assert(_cond) \
   do \
   { \
      if( __builtin_expect((_cond), 0) ) \
         halt(); \
   }while(0)

#define min(_a, _b) (((_a) < (_b)) ? (_a) : (_b))
#define max(_a, _b) (((_a) > (_b)) ? (_a) : (_b))

#define container_of(ptr, type, member) ({ \
    const typeof( ((type *)0)->member ) *__mptr = (ptr); \
    (type *)( (char *)__mptr - offsetof(type,member) );})

#define ele_cnt(_table) (sizeof(_table) / sizeof((_table)[0]))

//#define USE_WATCHDOG

#endif /* __PROJECT_ */
