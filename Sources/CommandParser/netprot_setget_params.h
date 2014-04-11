#ifndef NETPROT_SETGET_PARAMS_H
#define NETPROT_SETGET_PARAMS_H

#define NETPROT_PARAM_NAMESIZE 16
#define NETPROT_OBJECT_MAXATTRS 32

#define NETPROT_PARAMS_STRVALSIZE 32

/* Enum for possible types of parameter*/
enum netprot_param_type {
	PARAM_INT,  /* Integer */
	PARAM_STR,  /* String */
};

/* Flags for parameters */
enum netprot_param_flags {
	FLAG_RO	= (1),	/* Read Only */
};

/* Structure to describe a parameter */
typedef struct netprot_param_t {
	char name[NETPROT_PARAM_NAMESIZE];
	enum netprot_param_type type;			/* Type of the parameter */
	char flags;								/* Flags from netprot_param_flags*/
	int intval;								/* Integer value, if given */
	char strval[NETPROT_PARAMS_STRVALSIZE];	/*String value, if given */
	/* Default Values */
	int default_intval;
	char default_strval[NETPROT_PARAMS_STRVALSIZE];
} netprot_param;

/* Structure to describe an object containing a list of parameters */
typedef struct netprot_object_t {
	char name[NETPROT_PARAM_NAMESIZE];
	netprot_param *attrs;
} netprot_object;



#endif