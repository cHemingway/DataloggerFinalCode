#include "netprot_setget_params_list.h"
#include "config.h"
#include <stddef.h>

/* List of "CHANNEL" object attributes */
netprot_param channel_attrs[] = {
	/* CHANNEL NAME */
	{
		"NAME",		/* Name = "NAME" */
		PARAM_STR,	/* Type = String */
		0,			/* Flags = 0 */
		0,			/* Integer Value = 0 */
		"",			/* String Value is empty */
		0,			/* Default integer = 0 */
		"",			/* Default value = empty */
	},
	/* TERMINATOR */
	{"",0,0,0,"",0,""}
};

/* List of "BOARD" object attributes */
netprot_param board_attrs[] = {
	/* BOARD TYPE */
	{
		"TYPE",						/* Name = "TYPE" */
		PARAM_STR,					/* Type = String */
		FLAG_RO,					/* Read Only */
		0,							/* Integer Value = 0 */
		CONFIG_BOARD_TYPE_STRING,	/* String value from config file */
		0,							/* Default integer = 0 */
		CONFIG_BOARD_TYPE_STRING,	/* Default value from config filey */
	},
	/* TERMINATOR */
	{"",0,0,0,"",0,""}
};

#if CONFIG_BOARD == CONFIG_BOARD_PWM
/* List of "PWM" object attributes */
netprot_param pwm_attrs[] = {
	/* PWM FREQ */
	{
		"FREQ",				/* Name = "FREQ" */
		PARAM_INT,			/* Type = Integer */
		0,					/* Flags = 0 */
		100000,				/* 100Khz */
		"",					/* No string value */
		100000,				/* 100Khz */
		"",					/* No string value */
	},
	/* PWM WIDTH */
	{
		"WIDTH",			/* Name = "WIDTH" */
		PARAM_INT,			/* Type = Integer */
		0,					/* Flags = 0 */
		3000,				/* 3us */
		"",					/* No string value */
		3000,				/* 3us */
		"",					/* No string value */
	},
	/* PWM DELAY */
	{
		"DELAY",			/* Name = "DELAY" */
		PARAM_INT,			/* Type = Integer */
		0,					/* Flags = 0 */
		CONFIG_PWM_DELAY,	/* From config file */
		"",					/* No string value */
		CONFIG_PWM_DELAY,	/* From config file */
		"",					/* No string value */
	},
	
	/* TERMINATOR */
	{"",0,0,0,"",0,""}
		
};
#endif /* CONFIG_BOARD == CONFIG_BOARD_PWM */


/* List of all the "objects" (collections of properties) */
netprot_object netprot_objects[] = {
	/* CHANNEL OBJECT */
	{
		"CHANNEL",	/* Name */
		channel_attrs
	},
	
	/* BOARD OBJECT */
	{
		"BOARD",
		board_attrs
	},
	
	#if CONFIG_BOARD == CONFIG_BOARD_PWM
	/* PWM OBJECT */
	{
		"PWM",
		pwm_attrs
	},
	#endif

	/* TERMINATOR */
	{ "",	NULL}
};
