#include "derivative.h" /* include peripheral declarations */

#include "fnet.h" /* FNET stack */

#include <string.h> /* for STRLEN */

/* Our own headers */
#include "init.h"
#include "bcast.h"
#include "dhcp.h"
#include "netprot.h"
#include "netprot_header.h"
#include "sevenseg.h"
#include "capture.h"

/* Setup functions for networking */
#include "main_util_funcs.h"


#define BCAST_PORT 4950	
#define DHCP_TRIES 10	/* No of attempts to try and get address by DHCP before giving up */


int main(void) {
	SOCKET bcast_s, server_s;
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
		
		/* CONNECT IF NEEDED */
		if(!connected) {
			connected = netprot_connect(bcast_s, &server_s);
		}
		
		/* WHEN CONNECTED */
		if (connected) {
			int err;
			/* Send data */
			err = netprot_send_capture(server_s);
			if (err) {
				netprot_goodbye(&server_s);
				connected = 0;
				continue;
			}
			
			/* Get commands */
			err = netprot_get_commands(server_s);
			if (err) {
				netprot_goodbye(&server_s);
				connected = 0;
				continue;
			}
		}
		
		/* Polling services.*/
		fnet_poll_services();
	}
	
	/* Should never end up here */
	return 0;
}
