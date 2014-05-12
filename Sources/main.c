#include "derivative.h" /* include peripheral declarations */

#include "fnet.h" /* FNET stack */

#include <string.h> /* for STRLEN */

/* Our own headers */
#include "init.h"
#include "bcast.h"
#include "dhcp.h"
#include "netprot.h"
#include "netprot_command.h"
#include "netprot_command_list.h"
#include "netprot_setget_params.h"
#include "netprot_header.h"
#include "sevenseg.h"
#include "capture.h"

/* Setup functions for networking */
#include "main_util_funcs.h"


#define BCAST_PORT 4950	
#define DHCP_TRIES 10	/* No of attempts to try and get address by DHCP before giving up */


/* NAME: netprot_send_capture
 * DESCRIPTION: Sends captured data to the server
 * PARAMS:
 * 		SOCKET s:	A connected socket to the server
 * RETURNS:
 * 		-1:			On socket error (disconnect)
 * 		0:			On success
 */
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
	}
}

int main(void) {
	SOCKET bcast_s, server_s;
	struct sockaddr server_sockaddr;
	int bcast_status;
	int connected = 0;
	
	/* Setup Hardware */
	init_hw();
	
	/* Initialise 7 Seg */
	sevenseg_set("0000",DP_0);
	sevenseg_init();
	
	/* Initialise UART */
	fnet_cpu_serial_init(FNET_CFG_CPU_SERIAL_PORT_DEFAULT, 115200);

	/* Clear some screen */
	fnet_printf("\n\n\n");

	/* Initialise FNET stack */
	init_fnet();
	
	/* Set MAC address based on K60 UID*/
	set_mac_address();
	
	/* Wait for Ethernet connection */
	fnet_printf("Waiting for connection \n");
	sevenseg_set("0000",DP_1); /* First dot = waiting for connection */
	while (!check_connected()) {
		fnet_timer_delay(FNET_TIMER_TICK_IN_SEC * 1); /* 1 Sec */
		fnet_printf("."); /* Print some errors */
	}
	
	/* Wait for DHCP server, if it fails use link local */
	wait_dhcp(DHCP_TRIES);  
	
	/* Print current IP address */
	fnet_printf("Current IP Address:");
	print_cur_ip();
	fnet_printf("\n");
	
	sevenseg_set("0000",DP_2); /* Second dot = have IP Address */
	
	/* Start UDP receiver */
	bcast_s = setup_listener(BCAST_PORT);
	if (bcast_s == -1) {
		fnet_printf("BCAST: Error, could not initialise port \n");
		while (1);
	}
	
	sevenseg_set("0000",DP_3); /* Third dot = waiting for UDP */

	/* Main Loop */
	while (1) {
		
		int recvcount;
		char commandstr[100];
		const int commandstr_len = (sizeof commandstr) / (sizeof commandstr[0]); 
		char outstr[100]; /* Return value to server */
		const int outstr_len = (sizeof outstr) / (sizeof outstr[0]);
		
		
		/* CONNECT IF NEEDED */
		if(!connected) {
			bcast_status = check_broadcast(bcast_s, &server_sockaddr, 4951);
			if (bcast_status == 1) {
				int connect_error;
				char server_ipstr[FNET_IP6_ADDR_STR_SIZE];
				int server_ipstr_len = (sizeof(server_ipstr)/sizeof(server_ipstr[0]));
				
				fnet_printf("BCAST: Server bcast received \n");
				fnet_inet_ntop(server_sockaddr.sa_family, server_sockaddr.sa_data, server_ipstr, server_ipstr_len);
				fnet_printf("BCAST: Server IP: %s \n",server_ipstr);
				fnet_printf("BCAST: Server Port: %d \n",FNET_NTOHS(server_sockaddr.sa_port));
				
				/* Send a hello */
				connect_error = netprot_hello(&server_s, &server_sockaddr, 2000);
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
		}
		
		
		if (connected) {
			/* Send data */
			netprot_send_capture(server_s);
			
			/* PARSE COMMANDS IF CONNECTED */
			recvcount = recv(server_s, commandstr, commandstr_len, 0);/* Poll Socket */
			/* Parse command: TODO, ONLY READ ONE LINE */
			if (recvcount>0) {
					netprot_param *segname;
					int err;
					/* Null terminate */
					commandstr[recvcount] = '\0';
					/* Process Command */
					err = netprot_process_command(netprot_default_command_list, commandstr, outstr, outstr_len);
					if (err) {
						fnet_printf("CMD: Error processing command %s", commandstr);
					}
					/* Send reply */
					send(server_s, outstr, strlen(outstr), 0);
					
					/* Temporary: Update Display */
					netprot_find_object_attr("CHANNEL","NAME", &segname);
					sevenseg_set(segname->strval,0);
			}
			else if (recvcount==SOCKET_ERROR) {
				fnet_printf("Server Disconnected \n");
				netprot_goodbye(&server_s);
				connected = 0; /* Disconnected */
			}
		}
		
		/* Polling services.*/
		fnet_poll_services();
	}
	
	/* Should never end up here */
	return 0;
}
