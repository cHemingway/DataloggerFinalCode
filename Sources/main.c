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
#include "sevenseg.h"

#define BCAST_PORT 4950	
#define DHCP_TRIES 10	/* No of attempts to try and get address by DHCP before giving up */


void init_fnet(void) {
	/* Enable FNET interrupts.*/
	fnet_cpu_irq_enable(0);
	
	/* Init FNET stack */
	if (fnet_init_static() != FNET_ERR) {
		if (fnet_netif_get_default() == FNET_NULL) {
			fnet_printf("Network interface is not configured \n");
			while (1)
				;
		}
	} else {
		fnet_printf("Error FNET Initialisation is failed");
		while (1)
			;
	}

	fnet_printf("Stack Ready \n");
}

/* Function to print the current IPv4 address */
void print_cur_ip(void) {
	fnet_netif_desc_t cur_netif = fnet_netif_get_default();
	fnet_ip4_addr_t cur_ip = fnet_netif_get_ip4_addr(cur_netif);
	char cur_ip_str[FNET_IP4_ADDR_STR_SIZE];
	/* Print address */
	fnet_printf("%s", 
			(fnet_inet_ntop(AF_INET, &cur_ip, cur_ip_str, sizeof cur_ip_str)) 
			? (cur_ip_str) : ("Error IP")); /* Print "Error IP" if no connection */
	/* Print if from DHCP or not */
	if(fnet_netif_get_ip4_addr_automatic(cur_netif)) {
		fnet_printf(" (automatic)");
	}
	else {
		fnet_printf(" (manual)");
	}
}

/* Function to check if network cable is currently connected */
int check_connected(void) {
	fnet_netif_desc_t cur_netif = fnet_netif_get_default();
	int connected = fnet_netif_connected(cur_netif);
	if (connected) {
		return 1; /* Connected */
	}
	else {
		return 0; /* No Connection */
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
	sevenseg_init();
	
	/* Initialise UART */
	fnet_cpu_serial_init(FNET_CFG_CPU_SERIAL_PORT_DEFAULT, 115200);

	/* Clear some screen */
	fnet_printf("\n\n\n");

	/* Initialise FNET */
	init_fnet();

	
	/* Wait for Ethernet connection */
	fnet_printf("Waiting for connection \n");
	while (!check_connected()) {
		fnet_timer_delay(FNET_TIMER_TICK_IN_SEC * 1); /* 1 Sec */
		fnet_printf("."); /* Print some errors */
	}
	
	/* Wait for DHCP server */
	//wait_dhcp(DHCP_TRIES);  
	
	/* Print current IP address */
	fnet_printf("Current IP Address:");
	print_cur_ip();
	fnet_printf("\n");
	
	/* Start UDP receiver */
	bcast_s = setup_listener(BCAST_PORT);
	if (bcast_s == -1) {
		fnet_printf("BCAST: Error, could not initialise port \n");
		while (1);
	}

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
		
		/* PARSE COMMANDS IF CONNECTED */
		if (connected) {
			/* Get command if found */
			recvcount = recv(server_s, commandstr, commandstr_len, 0);/* Poll Socket */
			
			/* Parse command: TODO, ONLY READ ONE LINE */
			if (recvcount>0) {
					/* Null terminate */
					commandstr[recvcount] = '\0';
					/* Process Command */
					netprot_process_command(netprot_default_command_list, commandstr, outstr, outstr_len);
					/* Send reply */
					send(server_s, outstr, strlen(outstr), 0);
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
