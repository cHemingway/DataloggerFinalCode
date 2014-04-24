#include "netprot_commands_startstop.h"
#include "trigger.h"
#include "capture.h"
#include <string.h>
#include "stdio.h"

int netprot_cmd_start(const char *in, char *out, int outlen) {
	/* Setup capture */
	capture_setup(500);
	
	/* Start capture isr */
	trigger_isr_start(capture_isr);
	
	/* Don't output anything */
	*out = '\0';
	/* Success */
	return 0;
}


int netprot_cmd_stop(const char *in, char *out, int outlen) {
	char retval[] = "+OK \r\n";
	
	/* Stop capture isr */
	trigger_isr_stop();
	
	/* Output OK */
	if (outlen < sizeof(retval))
	{
		return -1;
	} else {
		strcpy(out,retval);
		return 0; /* Success */
	}
}
