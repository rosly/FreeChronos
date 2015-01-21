# 
# This file is a part of RadOs project
# Copyright (c) 2013, Radoslaw Biernaki <radoslaw.biernacki@gmail.com>
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1) Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
#
# 2) Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# 3) No personal names or organizations' names associated with the 'RadOs' project
#    may be used to endorse or promote products derived from this software without
#    specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE RADOS PROJET AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

TARGET     = freechronos
MCU        = cc430f6137

SOURCES = \
	main.c \
	pmm.c \
	wtdg.c \
	ucs.c \
	lcd.c \
	rtc.c \
	btt.c

KERNEL = \
	os_port.c \
	os_sched.c \
	os_timer.c \
	os_sem.c

INCLUDES = kernel source

LIBS =

SOURCEDIR = source
KERNELDIR = kernel
BUILDDIR = build

CFLAGS   = -mmcu=$(MCU) -g -O2 -std=gnu99 -Wall -Wextra
LDFLAGS  = -mmcu=$(MCU) -mdisable-watchdog

CC       = msp430-gcc
LD       = msp430-ld
AR       = msp430-ar
AS       = msp430-gcc
GASP     = msp430-gasp
NM       = msp430-nm
OBJCOPY  = msp430-objcopy
OBJDUMP  = msp430-objdump
RANLIB   = msp430-ranlib
STRIP    = msp430-strip
SIZE     = msp430-size
READELF  = msp430-readelf
#MAKETXT  = srec_cat
CP       = cp -p
RM       = rm -f
MV       = mv

vpath %.c $(SOURCEDIR) $(KERNELDIR)
DEPEND = $(addprefix $(BUILDDIR)/, $(SOURCES:.c=.d))
OBJECTS = $(addprefix $(BUILDDIR)/, $(SOURCES:.c=.o)) $(addprefix $(BUILDDIR)/, $(KERNEL:.c=.o))
INCLUDES := $(addprefix -I, $(INCLUDES))
ELF = $(BUILDDIR)/$(TARGET).elf
HEX = $(BUILDDIR)/$(TARGET).hex
TXT = $(BUILDDIR)/$(TARGET).txt

all: $(BUILDDIR)/$(TARGET).elf $(BUILDDIR)/$(TARGET).hex $(BUILDDIR)/$(TARGET).lst
$(ELF): $(OBJECTS)
	echo "Linking $@"
	$(CC) $(OBJECTS) $(LDFLAGS) $(LIBS) -o $@
	echo
	echo ">>>> Size of Firmware <<<<"
	$(SIZE) $(ELF)
	echo
$(BUILDDIR)/%.o: %.c
	echo "Compiling $<"
	$(CC) -c $(CFLAGS) -o $@ $(INCLUDES) $<
$(BUILDDIR)/%.hex: $(BUILDDIR)/%.elf
	$(OBJCOPY) -O ihex $< $@
$(BUILDDIR)/%.txt: $(BUILDDIR)/%.hex
	$(MAKETXT) -O $@ -TITXT $< -I
$(BUILDDIR)/%.lst: $(BUILDDIR)/%.elf
	$(OBJDUMP) -dStw $< > $@

# include the dependencies unless we're going to clean, then forget about them.
ifneq ($(MAKECMDGOALS), clean)
-include $(DEPEND)
endif
# dependencies file
$(BUILDDIR)/%.d: %.c
	echo "Generating dependencies $@ from $<"
	$(CC) -MM -MT $(@:.d=.o) ${CFLAGS} $(INCLUDES) $< >$@
.SILENT:
.PHONY: clean
clean:
	-$(RM) $(OBJECTS)
	-$(RM) $(TARGET).*
	-$(RM) $(SOURCES:.c=.lst)
	-$(RM) $(DEPEND)

