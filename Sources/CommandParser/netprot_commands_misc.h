#ifndef NETPROT_COMMANDS_MISC
#define NETPROT_COMMANDS_MISC

/* Ping Command, returns "+PONG \r\n" */
int netprot_cmd_ping(const char *in, char *out, int outlen);

/* +OK Command, returns nothing */
int netprot_cmd_ok(const char *in, char *out, int outlen);

/* NETPROT_COMMANDS_MISC */
#endif
