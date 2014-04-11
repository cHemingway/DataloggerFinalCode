#ifndef _NETPROT_COMMAND_H_
#define _NETPROT_COMMAND_H_

#define NETPROT_CMD_NAME_MAXLEN 20 

/* Structure to define a command */
typedef struct command_t {
	/* Null terminated string giving name of command */
	char name[NETPROT_CMD_NAME_MAXLEN];
	/* Pointer to function called for command:
	 * Params:
	 *		const char* in: All data after the name of the command
	 *		char* out:		Pointer to the string for returned data
	 *						MUST BE SET or else previous value will prevail
	 *		int:			The maximum length of out
	 */
	int (*func)(const char*, char*, int);
} netprot_command;


/*NAME: process_command
 *DESCRIPTION: 	Processes a null terminated command string.
 *				Assumes no whitespace before command.
 *PARAMS:
 * commands:	An array of the commands to process, terminated with null values.
 *		in:		The command string to process
 *		out:	A pointer to a string to store the null terminated output
 *	outsize:	The maximum size of the output
 *RETURNS:
 *		-2:		On Remote Error (description returned in *out)
 *		-1:		On Local Error (e.g. *out == NULL)
 *		 0:		On Success
 */
int netprot_process_command(const netprot_command commands[], const char *in, 
							char *out, int outsize);


/* _NETPROT_COMMAND_H */
#endif 