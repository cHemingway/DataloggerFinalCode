#include "netprot_commands_misc.h"
#include <string.h>

/* Command returns "PING" when "PONG" is sent */
int netprot_cmd_ping(const char *in, char *out, int outlen) {
	char retval[] = "+PONG \r\n";

	if (outlen < sizeof(retval))
	{
		return -1;
	} else {
		strcpy(out,retval);
		return 0;
	}
}

/* Command returns nothing when "PONG" is sent */
int netprot_cmd_ok(const char *in, char *out, int outlen) {
	*out = '\0'; /* Empty String */
	return 0;
}
