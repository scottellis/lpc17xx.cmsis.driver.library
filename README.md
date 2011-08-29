  lpc17xx.cmsis.driver.library
=======

A shortcut for setting up new machines to do Gumstix Robovero development.

This is the NXP sample driver library for LPC17xx Cortex-M3 boards downloaded
from here

[http://ics.nxp.com/support/documents/microcontrollers/zip/lpc17xx.cmsis.driver.library.zip]

The modifications in this repository are for building the library using the 
Crosstool-NG tools configured as described on the Gumstix Robovero Wiki

[http://wiki.gumstix.org/index.php?title=RoboVero#crosstool-NG]

If you setup the Crosstool-NG for arm-bare_newlib_cortex-m3_nommu-eabi with 
a destination of $(HOME)/x-tools and you added this to your PATH

	${HOME}/x-tools/arm-bare_newlib_cortex_m3_nommu-eabi/bin

then you should be able to use build the library from this repo as follows

	$ cd ${HOME}/cortex-m3
	$ git clone git@github.com/scottellis/lpc17xx.cmsis.driver.library.git
	$ cd lpc17xx.cmsis.driver.library
	$ make
 
