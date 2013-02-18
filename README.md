bt8x8cap
========

Capture raw frame data from Brooktree 848 and compatible capture chips like Conexant Fusion 878A, as used on the Hauppage ImpactVCB 00166 and similar cards.

Based on nxtvepg (http://nxtvepg.sourceforge.net/) 2.8.1. Both that and this use the driver from DScaler 4 (http://deinterlace.sourceforge.net/) to access the bt8x8 card directly rather than going through WDM like btwdmdrv does.

bt8x8cap uses the card's VBI frame output mode to capture raw samples at 8 times the NTSC color carrier frequency (28.636MHz). The samples are placed in output.dat which is formatted as blocks of 2048x480 1-byte-per-sample data. The samples are in the top left 1024x450 part of each block. bt8x8cap waits for 5 seconds after starting the card to allow the AGC to adjust, then captures for 0.5s. Though for some reason it only captures 10 fields rather than the expected 30.

TODO:
* Split the program into a daemon which keeps the AGC warmed up and a program which can be run to actually capture frames quickly and process them for XTServer.
* Fix the capture format.
* Use the captured data for composite CGA calibration.
