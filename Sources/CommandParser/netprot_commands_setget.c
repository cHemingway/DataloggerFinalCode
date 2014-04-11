#include "netprot_commands_setget.h"
#include "parse_assignment.h"
#include "netprot_setget_params.h"
#include "netprot_setget_params_list.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>


static int find_object(const char *name, netprot_object **out) {
	int namelen, found = 0, i = 0;
	/* Check if name is present */
	if (*name=='\0') {
		return -1;
	}
	/* Search for object */
	while (found==0) {
		char *objname = netprot_objects[i].name;
		/* Check for end of list */
		if (*objname == '\0') {
			break;
		}
		/* Check name */
		namelen = strlen(objname);
		if(!strncmp(objname, name, namelen)) {
			*out = &netprot_objects[i];
			found = 1;
		}
		i++;
	}

	/* Return success or failure */
	return !found;
}

static int find_attr(const char *name, netprot_object *object, netprot_param **out) {
	int namelen, found = 0, i = 0;
	/* Check if name is present */
	if (*name=='\0') {
		return -1;
	}
	/* Search for parameter */
	while (found==0) {
		char *paramname = object->attrs[i].name;
		/* Check for end of list */
		if (*paramname == '\0') {
			break;
		}
		/* Check name */
		namelen = strlen(paramname);
		if(!strncmp(paramname, name, namelen)) {
			*out = &(object->attrs[i]);
			found = 1;
		}
		i++;
	}

	/* Return success or failure */
	return !found;

}

int netprot_cmd_set(const char *in, char *out, int outlen) {
	char success[] = "+OK \n";
	char *ptr, *attrname;
	netprot_object *object;
	netprot_param *attr;
	struct valout_t valout;
	int err;

	/* Skip over whitespace */
	ptr = (char *)in;
	while(isspace(*ptr++));
	ptr--;

	/* Search for object */
	err = find_object(ptr, &object);
	if (err) {
		strcpy(out, "-ERR OBJECT NOT FOUND \r\n");
		return -1;
	}

	/* Skip to assignment */
	ptr += strlen(object->name);

	/* Skip any whitespace */
	while(isspace(*ptr)) ptr++;

	/* Parse assignment */
	err = parse_assignment(ptr, ptr+strlen(ptr), &attrname, &valout);
	if (err) {
		strcpy(out, "-ERR INVALID ASSIGNMENT \r\n");
		return -1;
	}

	/* Search for attribute name */
	err = find_attr(attrname, object, &attr);
	if (err) {
		strcpy(out, "-ERR ATTR NOT FOUND \r\n");
		return -1;
	}

	/* Check read only */
	if(attr->flags & FLAG_RO) {
		strcpy(out, "-ERR ATTR READ ONLY \r\n");
		return -1;
	} 

	/* Check type and assign */
	if (valout.type == VAL_INT && attr->type == PARAM_INT) {
		attr->intval = valout.integer;
	}
	else if(valout.type == VAL_STR && attr->type == PARAM_STR) {
		/* Check string length */
		if (strlen(valout.string)+1 >= NETPROT_PARAMS_STRVALSIZE) {
			strcpy(out, "-ERR ATTR TOO LONG \r\n");
			return -1;
		}
		/* Copy it */
		strcpy(attr->strval, valout.string);
	}
	else {  /* Not a valid string */
		strcpy(out, "-ERR ATTR UNKNOWN TYPE \r\n");
		return -1;
	}

	/* Success */
	strcpy(out, success);
	return 0;	
}

int netprot_cmd_get(const char *in, char *out, int outlen) {
	char *ptr;
	netprot_object *object;
	netprot_param *attr;
	int err;

	/* Skip over whitespace */
	ptr = (char *)in;
	while(isspace(*ptr)) ptr++;

	/* Search for object */
	err = find_object(ptr, &object);
	if (err) {
		strncpy(out, "-ERR OBJECT NOT FOUND \r\n", outlen);
		out[outlen] = '\0'; /* Null Terminate */
		return -1;
	}

	/* Skip to attribute */
	ptr += strlen(object->name);
	while(isspace(*ptr)) ptr++;

	/* Check if attribute name was given */
	if (*ptr=='\0') { /* No attribute given, print them all */
		int i = 0;
		char *paramname, *outptr=out;
		int out_remaining = outlen;
		int paramlen;
		/* Print + sign */
		snprintf(outptr, out_remaining, "+");
		out_remaining -= strlen("+");
		outptr += strlen("+");
		/* Loop over each param until empty string */
		while ('\0' != *(paramname = object->attrs[i++].name) && (out_remaining)) {
			snprintf(outptr, out_remaining, "%s, ", paramname);
			paramlen = strlen(paramname) + sizeof(" ");
			out_remaining -= paramlen;
			outptr += paramlen;
		}
		/* Null teminate output */
		out[outlen] = '\0';
	}
	else {	/* Attribute name given */
		/* Search for attribute */
		err = find_attr(ptr, object, &attr);
		if (err) {
			strncpy(out, "-ERR ATTR NOT FOUND \r\n", outlen);
			out[outlen] = '\0'; /* Null Terminate */
			return -1;
		}

		/* Output Value */
		if (attr->type == PARAM_INT) {
			snprintf(out,outlen,"+%d\r\n",attr->intval);
			out[outlen] = '\0'; /* Null Terminate */
		} 
		else if (attr->type == PARAM_STR) {
			snprintf(out,outlen,"+%s\r\n",attr->strval);
			out[outlen] = '\0'; /* Null Terminate */
		}
	}


	/* Success */
	return 0;	
}

int netprot_cmd_reset(const char *in, char *out, int outlen) {
	int i = 0, j = 0;

	/* Loop over each object until empty name*/
	while (strcmp(netprot_objects[i].name,"")) {
		/* Loop over each parameter */
		while (strcmp(netprot_objects[i].attrs[j].name,"")) {
			/* Reset values */
			netprot_objects[i].attrs[j].intval = netprot_objects[i].attrs[j].default_intval;
			strcpy(netprot_objects[i].attrs[j].strval, netprot_objects[i].attrs[j].default_strval);
			j++;
		}
		i++;
	}

	/* Success */
	strncpy(out,"+OK \r\n",outlen);
	return 0;
}
