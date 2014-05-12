#ifndef __BCAST_H__
#define __BCAST_H__

#include <fnet.h>


/* NAME: bcast_setup_listener
 * DESCRIPTION: sets up a UDP broadcast listener on port port
 *
 * PARAMS:
 *		short port:				The port to listen on
 * RETURNS:
 *			-1	on failure
 *			 s  Socket handle on success
 */
SOCKET bcast_setup_listener(short port);


/* NAME: bcast_parse_broadcast
 * DESCRIPTION: parses a null-terminated broadcast message into a sock_addr struct.
 * 				note: does not set local port, only remote
 * 				sets port to 0 if not specified.
 * PARAMS:
 * 		char bcast_buf[]:	Broadcast to parse, null terminated
 * 		bcast_buf_len:		Length of broadcast buffer to parse
 * 		sockaddr out:		Output sockaddr of server.
 * RETURNS:
 * 			-1 on failure
 * 			 0 on success
 */
int bcast_parse_broadcast(const char bcast_buf[], int bcast_buf_len, struct sockaddr *out);


/* NAME: bcast_check_broadcast 
 * DESCRIPTION: checks for a broadcast message on s (setup by setup_listener).
 * PARAMS:
 * 		SOCKET s:				The socket to listen on (from setup_listener)
 * 		struct sock_addr *out:	The struct to populate with the received data
 * 		int default_port:		The default port to use if one is not specified
 * RETURNS:
 * 		   -1 on error
 * 			0  on no data recieved
 * 			1  on success
 */
int bcast_check_broadcast(SOCKET s, struct sockaddr *out, int default_port);

/* __BCAST_H__ */
#endif
