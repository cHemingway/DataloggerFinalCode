#include "config.h"

#include "netprot.h"
#include "netprot_command.h"
#include "netprot_command_list.h"
#include "netprot_setget_params.h"
#include "bcast.h"
#include "UID.h"
#include "sevenseg.h"
#include "capture.h"

#include "fnet.h"
#include <stdio.h>
#include <string.h>


#ifdef NETPROT_DEBUG
/* Macro to return -1 on error */
#define NETPROT_SOCKET_ERROR_CHECK(err) do {\
	if(err==SOCKET_ERROR) {\
		fnet_printf("SOCKET_ERROR in %s:%d: FNET returned %d \n",__func__,__LINE__,fnet_error_get());\
		return NETPROT_ERR_LOCAL;\
	}\
} while(0)
#else
/* Macro to return -1 on error */
#define NETPROT_SOCKET_ERROR_CHECK(err) do {\
	if(err==SOCKET_ERROR) return NETPROT_ERR_LOCAL;\
} while(0)
#endif

/* Checks if a socket is connected */
static int netprot_sock_connected(SOCKET s) {
	fnet_socket_state_t state = SS_UNCONNECTED;
	int statelen = sizeof(fnet_socket_state_t);
	getsockopt(s, SOL_SOCKET, SO_STATE, (char *)&state, &statelen);
	if (state == SS_CONNECTED) {
		return 1;
	}
	else {
		return 0;
	}
}

/* Function to check a response for an error condition. */
static int netprot_response_check(char resp[], int resplen) {
	if (resp) {
		return (resp[0]=='+') ? (0):(-1);
	} else {
		return -1;
	}
}

/* NAME: netprot_readline()
 * DESCRIPTION: Reads (and consume) only one line from a socket.
 * 				Will not read less than a whole line, even if data available.
 * PARAMS:
 * 		SOCKET s:	The socket to read from
 * 		*line:		A buffer to read into
 * 		linemaxsize:	The size of the buffer to read into, will not read if too short.
 * RETURNS:
 * 		n:		 				Size of the line
 * 		0:		 				No line to read or linesize < maxlinesize
 * 		NETPROT_ERR_LOCAL:		On error
 *  */
static int netprot_readline(SOCKET s, char *outline, int linemaxsize) {
	char linebuf[NETPROT_COMMANDSIZE], *newline_pos;
	int  linebuf_len = (sizeof linebuf) / (sizeof linebuf[0]); 
	int  n, line_len;
	
	/* Peek the whole line to get line position */
	n = recv(s, linebuf, linebuf_len, MSG_PEEK);
	NETPROT_SOCKET_ERROR_CHECK(n);
	
	/* Find the "/n" character */
	newline_pos = memchr(linebuf, '\n', n);
	if (newline_pos == NULL) {
		return 0;
	}
	
	/* Now properly read the line */
	line_len = newline_pos - linebuf;
	n = recv(s, outline, line_len, 0);
	NETPROT_SOCKET_ERROR_CHECK(n);
	
	/* Success */
	return n;
}

int netprot_hello(SOCKET *s, struct sockaddr *server_sockaddr, int timeout) {
	int err;
	char tosend[NETPROT_HELLO_SIZE];
	char uid_str[UID_STR_LEN];
	char torecv[NETPROT_RESPONSE_SIZE];
	int starttime, timeout_ticks;
	int connected = 0, recvcount=0;
	
	/* Get UID string */
	UID_tostr(uid_str, (sizeof(uid_str)/sizeof(char)));
	
	/* Build command string */
	snprintf(tosend, NETPROT_HELLO_SIZE-1, NETPROT_HELLO_FORMAT, uid_str);
	
	/* Close socket if it already exists */
	netprot_goodbye(s);
	
	/* Create Socket */
	*s = socket(AF_INET, SOCK_STREAM, 0);

	/* Connect */
	err = connect(*s, server_sockaddr, sizeof(struct sockaddr_in));
	NETPROT_SOCKET_ERROR_CHECK(err);
	
	/* Wait for connection */
	starttime = fnet_timer_ticks();
	timeout_ticks = fnet_timer_ms2ticks(timeout); /*TODO: Remove magic number */
	/* Poll until connected or timed out */
	connected = 0;
	while(!connected) {
		connected = netprot_sock_connected(*s);
		if (fnet_timer_get_interval(starttime, fnet_timer_ticks()) > timeout_ticks) {
			break;
		}
	}
	
	/* Check if we managed to connect */
	if (!connected) {
		return NETPROT_ERR_REMOTE;
	}
	
	/* SEND HELLO */
	err = send(*s, tosend, strlen(tosend), 0); /*No flags */
	NETPROT_SOCKET_ERROR_CHECK(err);
	
	/* Get "+OK" back */
	starttime = fnet_timer_ticks();
	timeout_ticks = fnet_timer_ms2ticks(timeout); /*TODO: Remove magic number */
	/* Poll until timeout or OK */
	while (recvcount==0) { 
		/* Ensure we only read OK and no more */
		recvcount = recv(*s, torecv, sizeof("+OK \r\n") -1, 0);/* Poll Socket */
		NETPROT_SOCKET_ERROR_CHECK(recvcount);
		if (fnet_timer_get_interval(starttime, fnet_timer_ticks()) > timeout_ticks) {
					break;
		}
	}
	
	/* Check if +OK was received */
	if (recvcount>0) {
		err = netprot_response_check(torecv, recvcount);
		if (err) { 
			return NETPROT_ERR_REMOTE; /* Invalid Response */
		}
	}
	else { /* No Response */
		return NETPROT_ERR_REMOTE;
	}
	
	/* Success */
	return NETPROT_OK;
}

int netprot_goodbye(SOCKET *s) {
	int err;
	struct linger linger_val = {1, 0}; /* Hard close */
	
	/* Turn off linger */
	err = setsockopt(*s, SOL_SOCKET, SO_LINGER, (char *)&linger_val, sizeof(struct linger));
	NETPROT_SOCKET_ERROR_CHECK(err);
	
	/* Close the socket */
	err = closesocket(*s);
	NETPROT_SOCKET_ERROR_CHECK(err);
	/* Success */
	return NETPROT_OK;
}

int netprot_get_commands(SOCKET s) {
	int recvcount;
	char commandstr[100];
	const int commandstr_len = (sizeof commandstr) / (sizeof commandstr[0]); 
	char outstr[100]; /* Return value to server */
	const int outstr_len = (sizeof outstr) / (sizeof outstr[0]);
	
	/* PARSE COMMANDS IF CONNECTED */
	recvcount = recv(s, commandstr, commandstr_len, 0);/* Poll Socket */
	/* Parse command: TODO, ONLY READ ONE LINE */
	if (recvcount>0) {
			
			int err;
			
			/* Null terminate */
			commandstr[recvcount] = '\0';
			/* Process Command */
			err = netprot_process_command(netprot_default_command_list, commandstr, outstr, outstr_len);
			if (err) {
				fnet_printf("CMD: Error processing command %s", commandstr);
			}
			/* Send reply */
			err = send(s, outstr, strlen(outstr), 0);
			NETPROT_SOCKET_ERROR_CHECK(err);
			
			/* Update Display if ADC board */
			#ifdef CONFIG_BOARD_ADC
			{
				netprot_param *segname;
				netprot_find_object_attr("CHANNEL","NAME", &segname);
				sevenseg_set(segname->strval,0);
			}
			#endif
	}
	else if (recvcount==SOCKET_ERROR) {
		fnet_printf("Server Disconnected \n");
		netprot_goodbye(&s);
		return -1; /* Disconnected */
	}
	
	/* Success */
	return 0;
}

int netprot_connect(SOCKET bcast_s, SOCKET *server_s) {
	int bcast_status, connected = 0;
	struct sockaddr server_sockaddr;
	
	bcast_status = bcast_check_broadcast(bcast_s, &server_sockaddr, 4951);
	if (bcast_status == 1) {
		int connect_error;
		char server_ipstr[FNET_IP6_ADDR_STR_SIZE];
		int server_ipstr_len = (sizeof(server_ipstr)/sizeof(server_ipstr[0]));
		
		fnet_printf("BCAST: Server bcast received \n");
		fnet_inet_ntop(server_sockaddr.sa_family, server_sockaddr.sa_data, server_ipstr, server_ipstr_len);
		fnet_printf("BCAST: Server IP: %s \n",server_ipstr);
		fnet_printf("BCAST: Server Port: %d \n",FNET_NTOHS(server_sockaddr.sa_port));
		
		/* Send a hello */
		connect_error = netprot_hello(server_s, &server_sockaddr, CONFIG_TIMEOUT);
		if(connect_error == NETPROT_OK) {
			fnet_printf("HELL0: Server connection established \n");
			connected = 1; /* Connected */
		}
		else if(connect_error == NETPROT_ERR_REMOTE){
			fnet_printf("HELLO: Server did not respond or responded incorrectly \n");
		}
		else {
			fnet_printf("HELLO: Socket error \n");
		}
	}
	else if (bcast_status == -1) {
		fnet_printf("BCAST: Error listening for server bcast \n");
	}
	return connected;
}

int netprot_send_capture(SOCKET s) {
	static int count = 0;
	struct netstruct *buf;
	int nsamples = capture_read(&buf);
	int n, sent, tosend;
	
	if (nsamples) { /* We have data to send */
		/* Append the header: TODO: ADD dt_ns */
		netprot_header_append(buf, count++, nsamples * sizeof(uint16_t), 0, 0);
		/* Send the data */
		tosend = netprot_header_getsize(buf);
		while (tosend>0) { /* Loop until all data is sent */
			n = send(s, (char*)buf+sent, tosend, 0);
			if (n == SOCKET_ERROR) {
				return -1;
			}
			sent += n;
			tosend -= sent;
		}
	}
	/* Success */
	return 0;
}
