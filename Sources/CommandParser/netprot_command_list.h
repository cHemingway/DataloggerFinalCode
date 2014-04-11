#ifndef NETPROT_COMMAND_LIST
#define NETPROT_COMMAND_LIST

#include "netprot_command.h"
#include "netprot_commands_misc.h"
#include "netprot_commands_setget.h"
#include <stddef.h>

netprot_command netprot_default_command_list[] = {
	{"PING", netprot_cmd_ping},
	{"SET", netprot_cmd_set},
	{"GET", netprot_cmd_get},
	{"RESET", netprot_cmd_reset},
	{"",NULL} /* Terminate with null values */
};

/* NETPROT_COMMAND_LIST */
#endif