DataloggerFinalCode
===================

Final code for the Kinetis K60 powered ethernet datalogger, built for TD4, a 4th year uni team design project.

Probably won't be much use unless you have the specific hardware needed. 


Acknowledgements
----------------

Most of the heavy lifting is done by the excellent [FNET](http://fnet.sourceforge.net/) ethernet stack (GNU GPL Licensed) by Andrey Butok, FNET Community.

Startup code etc by Freescale.

Clock driver (in mcg.c, mcg.h) from " [Kinetis 100MHz Rev 2 Example Projects](http://www.freescale.com/webapp/sps/site/prod_summary.jsp?code=K60_100&nodeId=01624698C9DE2DDDAF&fpsp=1&tab=Design_Tools_Tab) ", copyright Freescale.

SPI code modified by Aaron Duffy from Freescale's [Kinetis 100MHz Rev 2 Example Projects. ](http://www.freescale.com/webapp/sps/site/prod_summary.jsp?code=K60_100&nodeId=01624698C9DE2DDDAF&fpsp=1&tab=Design_Tools_Tab) 

Function set7seg() ( ffba6e2c8f8dfca221203cd8e837fea799479a8d ) written by Aaron Duffy.