#include "netprot_command.h"
#include <string.h>
#include <stddef.h>


int netprot_process_command(const netprot_command commands[], const char *in, char *out, int outsize)
{
	netprot_command cmd;
	int cmdlen;
	int diff = 0;
	int retval = 0;
	const char *cmdin;

	/* Check Input Parameters */
	if ((commands==NULL) || (in==NULL) || (out==NULL)) {
		return -1;
	}

	/* Loop over each command, commands is null terminated*/
	for(cmd=*commands++;cmd.func!=NULL;cmd=*commands++) {
		/* Get length */
		cmdlen = strlen(cmd.name);
		/* Match command */
		diff = strncmp(in, cmd.name, cmdlen); 
		if (!diff){ /*No difference, found */
			cmdin = in + cmdlen;
			retval = cmd.func(cmdin,out,outsize);
			break;
		}
	}

	if (diff!=0) {
		return -1;
	}
	else {
		return retval;
	}
}
