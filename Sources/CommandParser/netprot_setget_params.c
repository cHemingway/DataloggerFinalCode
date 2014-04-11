#include "netprot_setget_params.h"
#include "netprot_setget_params_list.h"
#include <string.h>

int netprot_find_object(const char *name, netprot_object **out) {
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

int netprot_find_attr(const char *name, netprot_object *object, netprot_param **out) {
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

int netprot_find_object_attr(const char *objname, const char *paramname, netprot_param **out) {
	int err;
	netprot_object *object;
	netprot_param *attr;
	
	/* Check params */
	if ( (!objname) || (!paramname) || (!out) ) {
		return -1;
	}
	/* Search for object */
	err = netprot_find_object(objname, &object);
	if (err) {
		return -1;
	}
	/* Search for attribute */
	err = netprot_find_attr(paramname, object, &attr);
	if (err) {
		return -1;
	}
	/* Success */
	*out = attr;
	return 0;
}
