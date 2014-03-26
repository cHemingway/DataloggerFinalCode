#ifndef _UID_H_
#define _UID_H_


#define UID_STR_LEN sizeof("0xabcdabcdabcdabcdabcdabcdabcdabcd")

/* NAME: UID_tostr
 * DESCRIPTION: gets device 128 bit UID register as a string hex literal.
 * PARAMS:
 * 	   *out:	A pointer to a string to output to
 * 		outlen:	The maximum size of the output (including null termination).
 * RETURNS:
 * 		 n		The length of the string written (excluding null)
 * 		-1		On failure
 */
int UID_tostr(char *out, int outlen);

#endif /* _UID_H_ */
