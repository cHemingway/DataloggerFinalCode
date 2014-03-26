#include "UID.h"
#include "derivative.h"
#include <stdio.h>

int UID_tostr(char *out, int outlen) {
	return snprintf(out,outlen,"0x%X%X%X%X",
			(unsigned int)SIM_UIDH,	/* High int */
			(unsigned int)SIM_UIDMH,/* Med-High int */
			(unsigned int)SIM_UIDML,/* Med-Low int */
			(unsigned int)SIM_UIDL);  /* Low int */
}
