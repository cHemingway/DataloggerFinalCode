#ifndef NETPROT_PARAMS_LIST_H
#define NETPROT_PARAMS_LIST_H

#include "netprot_setget_params.h"
#include <stddef.h>


/* List of "CHANNEL" object attributes */
netprot_param channel_attrs[] = {
	/* CHANNEL NAME */
	{
		"NAME",		/* Name = "NAME" */
		PARAM_STR,	/* Type = String */
		0,			/* Flags = 0 */
		0,			/* Integer Value = 0 */
		"",	/* String Value is empty */
		0,			/* Default integer = 0 */
		"",			/* Default value = empty */
	},

	/* TERMINATOR */
	{"",0,0,0,"",0,""}
};

netprot_object netprot_objects[] = {

	/* CHANNEL OBJECT */
	{
		"CHANNEL",	/* Name */
		channel_attrs
	},

	/* TERMINATOR */
	{ "",	NULL}

};

/* NETPROT_PARAMS_LIST_H */
#endif