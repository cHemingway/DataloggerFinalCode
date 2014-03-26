#include "netprot.h"
#include "fnet.h"
#include "UID.h"
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
		recvcount = recv(*s, torecv, NETPROT_RESPONSE_SIZE, 0);/* Poll Socket */
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
