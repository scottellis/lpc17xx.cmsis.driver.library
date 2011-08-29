/******************************************************************************/
/* RETARGET.C: 'Retarget' layer for target-dependent low level functions      */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2005-2006 Keil Software. All rights reserved.                */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

// This used for ARMCC compiler only
#if defined ( __CC_ARM   )

#include <rt_misc.h>

#pragma import(__use_no_semihosting_swi)

void _ttywrch(int ch) {
  // Not used (No Output)
}

void _sys_exit(int return_code) {
label:  goto label;  /* endless loop */
}
#endif /* __CC_ARM */
