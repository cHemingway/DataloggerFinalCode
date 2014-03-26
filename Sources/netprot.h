#ifndef _NETPROT_H_
#define _NETPROT_H_

#include "fnet.h"


#define NETPROT_HELLO_FORMAT "HELLO %s \r\n"
#define NETPROT_HELLO_SIZE ( sizeof("HELLO  \r\n") + UID_STR_LEN) 

#define NETPROT_RESPONSE_SIZE ( sizeof("+ERROR: ") + 20)


/* Return values of NETPROT functions */
enum netprot_err {
	NETPROT_ERR_REMOTE = (-2), /* Remote server error */
	NETPROT_ERR_LOCAL  = (-1), /* Local error, can also be caused by server disconnecting */
	NETPROT_OK		   = (0)   /* No Error */
};

/*NAME: netprot_hello
 *DESCRIPTION: connect()s to a server and sends the "hello" command
 *PARAMS:
 *		SOCKET *s:					A pointer to the socket to set up (out).
 *		sockaddr server_sockaddr:	The address and port of the server
 *RETURNS:
 *			NETPROT_ERR_REMOTE 	on invalid server response or timeout
 *			NETPROT_ERR_LOCAL 	on local failure
 *			NETPROT_OK 			on success
 */
int netprot_hello(SOCKET *s, struct sockaddr *server_sockaddr, int timeout);

/*NAME: netprot_goodbye
 *DESCRIPTION: shutdown() a socket, blocks until disconnected. 
 	 	 	   note, all unsent data will be deleted.
 *PARAMS:
 *		SOCKET *s:	The socket to disconnect
 *RETURNS:
 *		NETPROT_ERR_LOCAL 		on error 
 *		NETPROT_OK 				on success
 */
int netprot_goodbye(SOCKET *s);

#endif /* _NETPROT_H_ */
