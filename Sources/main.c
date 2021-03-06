#include "derivative.h" /* include peripheral declarations */
#include "fnet.h" /* FNET stack */
#include <string.h> /* for STRLEN */

/* Include configuration file */
#include "config.h"
/* Our own headers */
#include "init.h"
#include "bcast.h"
#include "dhcp.h"
#include "netprot.h"
#include "netprot_header.h"
#include "sevenseg.h"
#include "capture.h"
#include "trigger.h"


#if CONFIG_TEST_HW
void dspi_read(void) {
	dspi_no_fifo_3(1);
}
#endif

/* Setup functions for networking */
#include "main_util_funcs.h"
int main(void) {
	SOCKET bcast_s, server_s;
	int connected = 0, disconnect = 0;
	
	/* Setup Hardware */
	init_hw();
	
/* Test SPI and PWM */
#if CONFIG_TEST_HW
#if CONFIG_BOARD == CONFIG_BOARD_ADC
	trigger_isr_start(dspi_read);
#endif
#if CONFIG_BOARD == CONFIG_BOARD_PWM
	pwm_start();
#endif
	while(1);
#endif
	
	/* Initialise 7 Seg */
	sevenseg_set(CONFIG_7SEG_DEFAULT,DP_0);
	sevenseg_init();
	
	/* Initialise UART */
	fnet_cpu_serial_init(FNET_CFG_CPU_SERIAL_PORT_DEFAULT, 115200);

	/* Clear some screen */
	fnet_printf("\n\n\n");
	
	/* Print board type and firmware version */
	fnet_printf("Board Type: %s Firmware compiled %s %s \n",
			CONFIG_BOARD_TYPE_STRING, __TIME__, __DATE__);
	
	/* Initialise FNET stack */
	init_fnet();
	
	/* Set MAC address based on K60 UID*/
	set_mac_address();
	
	/* Wait for Ethernet connection */
	fnet_printf("Waiting for Connection \n");
	sevenseg_set(CONFIG_7SEG_DEFAULT,DP_1); /* First dot = waiting for connection */
	while (!check_connected()) {
		fnet_timer_delay(FNET_TIMER_TICK_IN_SEC * 1); /* 1 Sec */
		fnet_printf("."); /* Print some errors */
	}
	
	/* Wait for DHCP server, if it fails use link local */
	wait_dhcp(CONFIG_DHCP_TRIES);  
	
	/* Print current IP address */
	fnet_printf("Current IP Address:");
	print_cur_ip();
	fnet_printf("\n");
	
	sevenseg_set(CONFIG_7SEG_DEFAULT,DP_2); /* Second dot = have IP Address */
	
	/* Start UDP receiver */
	bcast_s = bcast_setup_listener(CONFIG_BCAST_PORT);
	if (bcast_s == -1) {
		fnet_printf("BCAST: Error, could not initialise port \n");
		while (1);
	}
	
	sevenseg_set(CONFIG_7SEG_DEFAULT,DP_3); /* Third dot = waiting for UDP */

	/* MAIN LOOP */
	while (1) {
		/* CONNECT IF NEEDED */
		if(!connected) {
			connected = netprot_connect(bcast_s, &server_s);
			#if CONFIG_BOARD == CONFIG_BOARD_PWM
				if(connected) {
					sevenseg_set("8888",DP_0); /* Change the screen, this will do for now */
				}
			#endif	
			fnet_poll_services(); /* Poll DHCP while unconnected */
		}
		
		/* WHEN CONNECTED */
		if (connected) {
			int sent = 0, err;
			
			/* Check for disconnection */
			if (disconnect) {
				disconnect = 0; /* Reset */
				netprot_goodbye(&server_s);
				/* Flush UDP port to prevent reconnecting to dead server */
				bcast_flush(bcast_s);
				sevenseg_set(CONFIG_7SEG_DEFAULT,DP_3); /* Third dot = waiting for UDP */
				/* Stop what you are doing */
				#if CONFIG_BOARD == CONFIG_BOARD_ADC
					trigger_isr_stop();
					capture_set_empty();
				#endif
				fnet_printf("Server Disconnected \n");
				connected = 0;
				continue;
			}
			
			/* Send data first if we are a capture type board */
			#if CONFIG_CAPTURE_SUPPORT
				sent = netprot_send_capture(server_s);
				if (sent<0) { /* Error - Disconnect */
					disconnect = 1;
					continue;
				}
			#endif
			
			/* Get commands */
			if (sent <= 0) { /* We have sent nothing or error */
				err = netprot_get_commands(server_s);
				if (err) { /* Error - Disconnect */
					disconnect = 1;
					continue;
				}
			}
		}
	}
	/* END OF MAIN LOOP */
	
	/* Should never end up here */
	return 0;
}
