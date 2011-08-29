######################################################################## 
# $Id:: makefile 1516 2008-12-17 00:28:46Z pdurgesh                    $
# 
# Project: Debugger loadable example makefile
#
# Notes:
#     This type of image is meant to be loaded and executed through a
#     debugger and will not run standalone and cannot be FLASHed into
#     the board.
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

EXECNAME    =uIPport
EXDIR		=EMAC/uIP

########################################################################
#
# Pick up the configuration file in make section
#
########################################################################
include ../../../makesection/makeconfig 

EXDIRINC	=$(PROJ_ROOT)/Examples/$(EXDIR)

# Include directory
TMPPATH 		+= -I"$(EXDIRINC)/apps/webserver" \
				-I"$(EXDIRINC)/common" \
				-I"$(EXDIRINC)/lpc17xx_port" \
				-I"$(EXDIRINC)/uip" 
				
# Create a list of aditional files will be compiled
# Retarget object files
ADDOBJS		+=  $(EXDIRINC)/common/Serial.o \
				$(EXDIRINC)/common/printf-stdarg.o \
				$(EXDIRINC)/common/syscalls.o 
# uIP 'apps' files
ADDOBJS		+=  $(EXDIRINC)/apps/webserver/httpd.o \
				$(EXDIRINC)/apps/webserver/httpd-cgi.o \
				$(EXDIRINC)/apps/webserver/httpd-fs.o \
				$(EXDIRINC)/apps/webserver/http-strings.o 
# uIP 'uip' files
ADDOBJS		+=  $(EXDIRINC)/uip/psock.o \
				$(EXDIRINC)/uip/timer.o \
				$(EXDIRINC)/uip/uip.o \
				$(EXDIRINC)/uip/uip_arp.o 
# 'lpc17xx_port' files 								
ADDOBJS		+=  $(EXDIRINC)/lpc17xx_port/emac.o \
				$(EXDIRINC)/lpc17xx_port/clock-arch.o \
				$(EXDIRINC)/lpc17xx_port/main.o 
				
				
include $(PROJ_ROOT)/makesection/makerule/example/makefile.ex
