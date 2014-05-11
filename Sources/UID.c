#include "UID.h"
#include "derivative.h"
#include <stdio.h>
#include <string.h>
#include <stddef.h>

int UID_tostr(char *out, int outlen) {
	return snprintf(out,outlen,"0x%X%X%X%X",
			(unsigned int)SIM_UIDH,	/* High int */
			(unsigned int)SIM_UIDMH,/* Med-High int */
			(unsigned int)SIM_UIDML,/* Med-Low int */
			(unsigned int)SIM_UIDL);  /* Low int */
}

int UID_tobuf(char *out, int outlen) {
	uint32_t *outptr;
	/* Check output size is correct */
	/* Size = 16, as 4 32 bit words */
	if (outlen < 16) {
		return -1;
	}
	
	/* To avoid weird behaviour, read 32 bits at a time
	 * instead of using memcpy().
	 */
	outptr = (uint32_t *) out;
	*outptr = SIM_UIDH;
	outptr++;
	*outptr = SIM_UIDMH;
	outptr++;
	*outptr = SIM_UIDML;
	outptr++;
	*outptr = SIM_UIDL;
	
	/* Success */
	return 0;
}
