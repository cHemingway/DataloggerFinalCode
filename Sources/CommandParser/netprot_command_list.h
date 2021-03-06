#ifndef NETPROT_COMMAND_LIST
#define NETPROT_COMMAND_LIST

#include "netprot_command.h"
#include "netprot_commands_misc.h"
#include "netprot_commands_setget.h"
#include "netprot_commands_startstop.h"
#include <stddef.h>

netprot_command netprot_default_command_list[] = {
	{"+OK", netprot_cmd_ok},
	{"PING", netprot_cmd_ping},
	{"SET", netprot_cmd_set},
	{"GET", netprot_cmd_get},
	{"START", netprot_cmd_start},
	{"STOP", netprot_cmd_stop},
	{"RESET", netprot_cmd_reset},
	{"",NULL} /* Terminate with null values */
};

/* NETPROT_COMMAND_LIST */
#endif
