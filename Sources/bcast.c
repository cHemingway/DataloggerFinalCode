#include <string.h>
#include <stdlib.h>
#include "bcast.h"
#include "fnet.h"

#define MCAST_MAXSIZE sizeof("IP=abcd:abcd:abcd:abcd:abcd:abcd:abcd:abcd PORT=xxxxx\r\n") 

SOCKET bcast_setup_listener(short port) {
	int err;
	SOCKET s;
	struct sockaddr_in socket_addr;
	struct sockaddr_in socket_out_addr;
	int sockaddr_len = sizeof(socket_addr);

	/* Clear socket_addr */
	memset(&socket_addr, 0, sizeof socket_addr);
	memset(&socket_out_addr, 0, sizeof socket_addr);

	/* Create Socket */
	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s == SOCKET_INVALID) {
		return -1;
	}

	/* Setup address for default network, specified port*/
	socket_addr.sin_family = AF_INET;
	socket_addr.sin_port = FNET_HTONS(port); //Convert port to network order
	socket_addr.sin_addr.s_addr = INADDR_ANY;

	/* Connect to port, see how it goes */
	err = bind(s, (struct sockaddr *) (&socket_addr),
			sizeof(struct sockaddr_in));
	if (err != FNET_OK) {
		fnet_printf("%d \n", fnet_error_get()); /* BUG: Breaks with error 6: INVAL. Blame address. */
		closesocket(s);
		return -1;
	}

	/* Read the port back again to check*/
	err = getsockname(s, (struct sockaddr *) (&socket_out_addr), &sockaddr_len);
	if (err != FNET_OK) {
		fnet_printf("%d \n", fnet_error_get()); /* BUG: Breaks with error 6: INVAL. Blame address. */
		closesocket(s);
		return -1;
	}
	fnet_printf("Listening on port %d \n",
			FNET_NTOHS(socket_out_addr.sin_port));

	return s;
}

/* NAME: extract_string
 * DESCRIPTION: extracts a single quoted string with a given prefix
 * 				e.g. port='6355', would extract 6355.
 * PARAMS:  
 * 			in: 	input string, null terminated.
 * 			prefix: includes starting quote, null terminated.
 * 			out:	the buffer to extract the string into, adds null termination
 * 			outlen:	the max size of the output buffer, including /0
 * RETURNS:
 * 			0: on failure
 * 			n: length of extracted string on success
 */
static int extract_string(const char in[], const char prefix[], char out[], int out_len) {
	char *str_ptr, *str_end_ptr;
	int str_len;
	
	str_ptr = strstr(in, prefix);
	if (str_ptr) {
		/* Strip prefix */
		str_ptr = str_ptr+strlen(prefix);  
		/* Find end single quote */
		str_end_ptr = strchr(str_ptr, '\'');
		if (!str_end_ptr) { 
			return 0; /* No end found */
		}
		/* Get length */
		str_len = str_end_ptr - str_ptr;
		/* Check if length is less than out_len */
		if (out_len < str_len+1) {
			return 0; /* String is too long */
		}
		/* Copy to new string  */
		strncpy(out, str_ptr, str_len);
		/* Add null termination */
		out[str_len] = '\0';
		
	}
	else { 
		return 0; /* No prefix found */
	}
	
	return str_len;
}


int bcast_parse_broadcast(const char bcast_buf[], int bcast_buf_len, struct sockaddr *out)
{
	char ipstr[FNET_IP6_ADDR_STR_SIZE];
	char portstr[sizeof("xxxxx")]; /* Max value of port is 65536 (2^16) */
	int ipstr_len, portstr_len;

	/* Sanity check, is string null terminated? */
	if ((bcast_buf[bcast_buf_len] != '\0') || (bcast_buf == NULL)) {
		return -1;
	}
	
	/* Search for IP address */
	ipstr_len = extract_string(bcast_buf, "IP='", ipstr, sizeof(ipstr)/sizeof(ipstr[0]) );
	if (ipstr_len == 0) {
		return -1;
	}
	
	/* Now turn into IP address */ //HACK: ADD 1 sometimes
	if(FNET_OK != fnet_inet_ptos(ipstr, out) ) {
		return -1;
	}
	
	/* Search for port */
	portstr_len = extract_string(bcast_buf, "PORT='", portstr, sizeof(portstr)/sizeof(portstr[0]) );
	if (portstr_len == 0) {
		out->sa_port = 0; /* Non set */
	}
	else {
		out->sa_port = FNET_HTONS(atoi(portstr)); /* Convert */
	}
	
	/* Success! */
	return 0;
}


int bcast_check_broadcast(SOCKET s, struct sockaddr *out, int default_port) {
	char recv_buf[(MCAST_MAXSIZE+1)]; /* Received data, extra space for null termination */
	int recv_buf_len = sizeof(recv_buf)/sizeof(recv_buf[0]);
	int recv_len;
	int err;

	/* First, try and receive a packet, add space for null termination */
	recv_len = recv(s, recv_buf, recv_buf_len-1, 0);
	
	/* Check if data */ 
	if (recv_len == 0) {
		return 0;
	}
	
	/* Check if error */
	if (recv_len == SOCKET_ERROR) {
		/* TODO: parse error code? */
		return -1;
	}
	
	/* Null terminate data for safety */
	recv_buf[recv_len] = '\0';
	
	/* Now parse */
	err = bcast_parse_broadcast(recv_buf, recv_len, out);
	if (err) {
		return -1;
	}
	
	/* Add port if not specified */
	if (out->sa_port == 0) {
		out->sa_port = FNET_HTONS(default_port);
	}
	
	/*Success! Return 1 */
	return 1;
}

void bcast_flush(SOCKET s) {
	char buf[100];
	while(recv(s,buf,100,0) > 0);
}
