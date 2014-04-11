#ifndef PARSE_ASSIGNMENT_H
#define PARSE_ASSIGNMENT_H

/* Enum for possible types of returned value */
enum valout_type {
	VAL_NONE, /* No value found */
	VAL_INT,  /* Integer, converted from a hex or decimal string */
	VAL_STR,  /* String, pointer to start */
};

/* Output structure */
struct valout_t {
	enum valout_type type;	/* Type of returned value */
	char *string; 			/* If return value is a string */
	int  integer; 			/* If return value is an integer */
};


/* NAME: parse_assignment
 * DESCRIPTION: given a assignment between *start and *end of the form:
 * "val=12" or "val=0xfe" or "val='string'"
 * extracts the string "val", and parse the assignment value into *valout.
 * note: does not duplicate the input, so do not free input if you still want valout_t or name.
 * BUGS:
 *		Does not correctly handle "input = +-22" or similar to keep things simple.
 * PARAMS:
 *		*start, *end: Start and end of a char array containing only a valid assignment
 * 		**name:		  The output name, NULL on failure
 *		*valout:	  A structure containing the type and value of the assigned value
 * RETURNS:
 *		0:	On Success
 *	   -1:	On Error
 */
int parse_assignment(char *start, char *end, char **name, struct valout_t *valout);

/* PARSE_ASSIGNMENT_H */
#endif