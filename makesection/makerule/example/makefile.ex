######################################################################## 
# $Id:: makefile.ex 1388 2008-12-01 19:20:06Z pdurgesh                 $
# 
# Project: Example makefile
#
# Notes:
#     This makefile generates an image that will load in SDRAM at
#     address 0x80000000. The image can be loaded and run with the
#     stage 1 loader or with a debugger.
#
# Description: 
#  Makefile
# 
######################################################################## 
# Software that is described herein is for illustrative purposes only  
# which provides customers with programming information regarding the  
# products. This software is supplied "AS IS" without any warranties.  
# NXP Semiconductors assumes no responsibility or liability for the 
# use of the software, conveys no license or title under any patent, 
# copyright, or mask work right to the product. NXP Semiconductors 
# reserves the right to make changes in the software without 
# notification. NXP Semiconductors also make no representation or 
# warranty that such application will be suitable for the specified 
# use without further testing or modification. 
########################################################################

########################################################################
#
# Pick up the configuration file in make section
#
########################################################################
#include ../../makeconfig

########################################################################
#
# Pick up the default build rules 
#
########################################################################

include $(PROJ_ROOT)/makesection/makerule/$(DEVICE)/make.$(DEVICE).$(TOOL)

########################################################################
#
# Pick up the assembler and C source files in the directory
# Included Cortex-Mx core files  
#
########################################################################
include $(PROJ_ROOT)\makesection\makerule\common\make.rules.ftypes
ADDOBJS     += $(PROJ_ROOT)/Core/$(CMCORE_TYPE)/CoreSupport/core_cm3.o 
ADDOBJS     += $(PROJ_ROOT)/Core/$(CMCORE_TYPE)/DeviceSupport/$(MANUFACTURE)/$(DEVICE)/system_$(DEVICE).o 

########################################################################
#
# For some Flag defined in header file in Example directory  
#
########################################################################
CFLAGS += -I$(EXDIRINC) $(TMPPATH) 

# Define __BUILD_WITH_EXAMPLE__ symbol in this case (build example mode)
CFLAGS += -D__BUILD_WITH_EXAMPLE__=1 


########################################################################
#
# Object file in FW library directory  
#
########################################################################
FWSRC 	= $(wildcard $(PROJ_ROOT)/$(FWLIB)/source/*.c)
FWSRCC  = $(wildcard $(PROJ_ROOT)/$(FWLIB)/source/*.cpp)
FWASM	= $(wildcard $(PROJ_ROOT)/$(FWLIB)/source/*.$(ASM_EXT))
FWOBJS  = $(FWSRC:%.c=%.o) $(FWSRCC:%.c=%.o) $(FWASM:%.$(ASM_EXT)=%.o)


########################################################################
#
# GNU compiler/linker specific stuff
#
########################################################################

ifeq ($(TOOL), gnu)

MEXT        =.map
MAPFILE     =$(EXECNAME)
#CFLAGS      +=-gdwarf-2
AFLAGS 		+=-gdwarf-2
ADDOBJSS    += $(PROJ_ROOT)/Core/$(CMCORE_TYPE)/DeviceSupport/$(MANUFACTURE)/$(DEVICE)/startup/gcc/startup_$(DEVICE).o


# Define Required Linker script file in each build mode
LDSCRIPTIROM = $(PROJ_ROOT)/makesection/makerule/example/ldscript_rom_$(TOOL).ld
LDSCRIPTIRAM = $(PROJ_ROOT)/makesection/makerule/example/ldscript_ram_$(TOOL).ld

# Following used for Chip Revision V00 only ------------------------------------
LDSCRIPTIROM_V00 = $(PROJ_ROOT)/makesection/makerule/example/ldscript_rom_v00_$(TOOL).ld
LDSCRIPTIRAM_V00 = $(PROJ_ROOT)/makesection/makerule/example/ldscript_ram_v00_$(TOOL).ld

endif

########################################################################
#
# Arm compiler/linker specific stuff
#
# ARM examples enter via __main and are linked at address 0x80000000
#
########################################################################

ifeq ($(TOOL), ads)
MEXT        =.map
MAPFILE     =$(EXECNAME)
CFLAGS      +=-g
AFLAGS      +=-g
LDSCRIPT    =../common/ldscript_iram_ads.ld
ADDOBJS     += pad.o
endif
ifeq ($(TOOL), rvw)
MEXT        =.map
MAPFILE     =$(EXECNAME)
CFLAGS      +=-g
AFLAGS      +=-g
LDSCRIPT    =../common/ldscript_iram_ads.ld
ADDOBJS     += pad.o
endif
ifeq ($(TOOL), keil)
MEXT        =.map
MAPFILE     =$(EXECNAME)
CFLAGS      +=-g
AFLAGS      +=-g
LDSCRIPT    =../common/ldscript_iram_ads.ld
ADDOBJS     += pad.o
endif

########################################################################
#
# IAR compiler/linker specific stuff
#
########################################################################

ifeq ($(TOOL), iar)
MEXT        =.map
MAPFILE     =$(EXECNAME)
CFLAGS      +=
AFLAGS      +=
LDESC       = --config
LDSCRIPT    =../common/ldscript_iram_$(TOOL).ld
ADDOBJS     += 
endif

########################################################################
#
# Rules to build the executable 
#
########################################################################

default: rom

ifeq ($(TOOL), gnu)

rom: LDSCRIPT=$(LDSCRIPTIROM)
rom: AFLAGS += --defsym RAM_MODE=0 
rom: CFLAGS += -D__RAM_MODE__=0
rom: cleanall debug_status  $(OBJS) $(ADDOBJS) $(ADDOBJSS) $(FWOBJS) 
	$(LD) $(OBJS) $(ADDOBJS) $(ADDOBJSS) $(FWOBJS) $(LDFLAGS) $(LK) $(SCAN) $(MAP) \
	$(MAPFILE)$(MEXT) $(LDESC) $(LDSCRIPT) -o $(EXECNAME)$(EXT)
	$(ELFTOHEX) $(EXECNAME)$(EXT) $(EXECNAME)$(HEX)
	$(ELFTOREC) $(EXECNAME)$(EXT) $(EXECNAME)$(REC)
	$(CODESIZE) $(EXECNAME)$(EXT)
#	$(ELFTOBIN) $(EXECNAME)$(EXT) $(EFLTBINOPT) $(EXECNAME).bin

ram: LDSCRIPT=$(LDSCRIPTIRAM) 
ram: AFLAGS += --defsym RAM_MODE=1
ram: CFLAGS += -D__RAM_MODE__=1 
ram: cleanall debug_status $(OBJS) $(ADDOBJS) $(ADDOBJSS) $(FWOBJS) 
	$(LD) $(OBJS) $(ADDOBJS) $(ADDOBJSS) $(FWOBJS) $(LDFLAGS) $(LK) $(SCAN) $(MAP) \
	$(MAPFILE)$(MEXT) $(LDESC) $(LDSCRIPT) -o $(EXECNAME)$(EXT)
	$(ELFTOHEX) $(EXECNAME)$(EXT) $(EXECNAME)$(HEX)
	$(ELFTOREC) $(EXECNAME)$(EXT) $(EXECNAME)$(REC)
	$(CODESIZE) $(EXECNAME)$(EXT)
#	$(ELFTOBIN) $(EXECNAME)$(EXT) $(EFLTBINOPT) $(EXECNAME).bin


# Following used for Chip Revision V00 only! --------------------------------------
ram_v00: LDSCRIPT=$(LDSCRIPTIRAM_V00) 
ram_v00: AFLAGS += --defsym RAM_MODE=1
ram_v00: CFLAGS += -D__RAM_MODE__=1 -D__LPC17XX_REV00=1 
ram_v00: debug_status $(OBJS) $(ADDOBJS) $(ADDOBJSS) $(FWOBJS) 
	$(LD) $(OBJS) $(ADDOBJS) $(ADDOBJSS) $(FWOBJS) $(LDFLAGS) $(LK) $(SCAN) $(MAP) \
	$(MAPFILE)$(MEXT) $(LDESC) $(LDSCRIPT) -o $(EXECNAME)$(EXT)
	$(ELFTOHEX) $(EXECNAME)$(EXT) $(EXECNAME)$(HEX)
	$(ELFTOREC) $(EXECNAME)$(EXT) $(EXECNAME)$(REC)
	$(CODESIZE) $(EXECNAME)$(EXT)
	
rom_v00: LDSCRIPT=$(LDSCRIPTIROM_V00)
rom_v00: AFLAGS += --defsym RAM_MODE=0  
rom_v00: CFLAGS += -D__RAM_MODE__=0 -D__LPC17XX_REV00=1 
rom_v00: debug_status $(OBJS) $(ADDOBJS) $(ADDOBJSS) $(FWOBJS) 
	$(LD) $(OBJS) $(ADDOBJS) $(ADDOBJSS) $(FWOBJS) $(LDFLAGS) $(LK) $(SCAN) $(MAP) \
	$(MAPFILE)$(MEXT) $(LDESC) $(LDSCRIPT) -o $(EXECNAME)$(EXT)
	$(ELFTOHEX) $(EXECNAME)$(EXT) $(EXECNAME)$(HEX)
	$(ELFTOREC) $(EXECNAME)$(EXT) $(EXECNAME)$(REC)
	$(CODESIZE) $(EXECNAME)$(EXT)
	
endif


# Print DEBUG MODE Status
debug_status:
	$(ECHO) "DEBUG MODE Status -->" $(DEBUG_MODE)

	
	
cleanall: realclean lpc_clean
	-@$(RM) $(ADDOBJS)
	-@$(RM) $(ADDOBJSS)

########################################################################
#
# Pick up the compiler and assembler rules
#
########################################################################

include $(PROJ_ROOT)/makesection/makerule/common/make.rules.build

.PHONY: debug_status 