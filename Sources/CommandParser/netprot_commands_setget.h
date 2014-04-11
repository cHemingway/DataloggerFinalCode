#ifndef NETPROT_COMMANDS_SETGET
#define NETPROT_COMMANDS_SETGET

/* Command to set a value */
int netprot_cmd_set(const char *in, char *out, int outlen);

/* Command to get a set value */
int netprot_cmd_get(const char *in, char *out, int outlen);

/* Command to reset all values */
int netprot_cmd_reset(const char *in, char *out, int outlen);


#endif