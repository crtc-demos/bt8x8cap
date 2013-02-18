bt8x8cap
========

Capture raw frame data from Brooktree 848 and compatible capture chips like
Conexant Fusion 878A, as used on the Hauppage ImpactVCB 00166 and similar
cards.

Based on nxtvepg (http://nxtvepg.sourceforge.net/) 2.8.1. Both that and this
use the driver from DScaler 4 (http://deinterlace.sourceforge.net/) to access
the bt8x8 card directly rather than going through WDM like btwdmdrv does.

bt8x8cap uses the card's VBI frame output mode to capture raw samples at 8
times the NTSC color carrier frequency (28.636MHz). The main program, vbicap,
runs as a daemon and keeps the card in a state from which it can quickly start
capturing (without using any CPU resources). There are two additional programs,
vbicap_close (which terminates the daemon safely) and vbicap_capture (which
triggers the daemon to capture 8 fields of video to output.dat).

WARNING! Terminating the daemon by any method other than vbicap_close
(particularly if it is capturing) is very likely to lead to bluescreening and
data corruption, as the driver DMAs to user mode memory.

output.dat is formatted as blocks of 460800 samples, slightly less than one
field (starting at the top of the CGA active area, the scanlines captured are
0-~234 and ~244-262. Capture starts right before the

TODO:
* Try to get make the DMA memory owned by the driver instead of the daemon.
* Try to get the remaining 10 lines captured.
* Write a program to perform NTSC decoding on the output data and turn it into
a .png flie for the XT Server.
* Play about with capturing video.
* Use the captured data for composite CGA calibration.
* RAII Mapmemory call
* RAII ACPI status
* Make m_hFile local and pass it in

