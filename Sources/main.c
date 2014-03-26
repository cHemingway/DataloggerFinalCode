#include "derivative.h" /* include peripheral declarations */
#include "mcg.h"
#include "fnet.h"
#include "bcast.h"
#include "mcg.h"
#include "dhcp.h"
#include "netprot.h"

#define BCAST_PORT 4950

/* Enable OSCERCLK to PHY */
void enable_phyclk(void) {
	/* Enable OSCERCLK0 */
	OSC_CR |= OSC_CR_ERCLKEN_MASK;	/* 1 = enable */
	/* Set CLKOUT to OSCERCLK0 */
	SIM_SOPT2 |= SIM_SOPT2_CLKOUTSEL(6); /* 6 = OSCERCLK0 */
	/* Set high drive strength */
	PORTC_PCR3 |= PORT_PCR_DSE_MASK;  /* 1 = high speed */
	/* Set fast slew rate */
	PORTC_PCR3 &= ~PORT_PCR_SRE_MASK; /* 0 = fast speed */
	/* Enable pin */
	PORTC_PCR3 &= ~PORT_PCR_MUX_MASK; /* Clear */
	PORTC_PCR3 |= PORT_PCR_MUX(5); /* Set to 5=CLKOUT */	
}

void init_hw(void) {
	/* Enable port clocks */
	SIM_SCGC5 |= (SIM_SCGC5_PORTA_MASK
				   | SIM_SCGC5_PORTB_MASK 
				   | SIM_SCGC5_PORTC_MASK 
				   | SIM_SCGC5_PORTD_MASK 
				   | SIM_SCGC5_PORTE_MASK );
	
	/* Set system dividers BEFORE setting clocks */
    SIM_CLKDIV1 = ( 0
                    | SIM_CLKDIV1_OUTDIV1(0)		/* Core at 100Mhz */
                    | SIM_CLKDIV1_OUTDIV2(1)		/* Bus at 50Mhz */
                    | SIM_CLKDIV1_OUTDIV3(1)		/* Flexbus at 50Mhz */
                    | SIM_CLKDIV1_OUTDIV4(3) );		/* Flash at 25Mhz */
	
	/* Setup clocks */
	pll_init(
			50000000,		/* 50MHz external oscillator */
			LOW_POWER,		/* No meaning as external osc */
			CANNED_OSC,		/* External reference clock */
			25,				/* Clock freq = ((50/25)*50)/2 = 100MHz */
			50,
			MCGOUT			/* use as mcgclock */
			);
	
	/* Output clock to PHY */
	enable_phyclk();
}

void init_fnet(void) {
	static unsigned char stack_heap[FNET_CFG_HEAP_SIZE]; //TODO: Fix so does not cross boundaries!
	struct fnet_init_params init_params;

	/* Input parameters for FNET stack initialisation */
	init_params.netheap_ptr = stack_heap;
	init_params.netheap_size = FNET_CFG_HEAP_SIZE;

	/* Init FNET stack */
	if (fnet_init(&init_params) != FNET_ERR) {
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

	//Wait a bit
	fnet_timer_delay(FNET_TIMER_TICK_IN_SEC * 2); //2 Sec
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
	int connected = 0; 
	struct sockaddr server_sockaddr;
	int bcast_status;
	
	/* Setup Hardware */
	init_hw();
	
	/* Initialise UART */
	fnet_cpu_serial_init(FNET_CFG_CPU_SERIAL_PORT_DEFAULT, 115200);

	/* Clear some screen */
	fnet_printf("\n\n\n");

	/* Enables interrupts.*/
	fnet_cpu_irq_enable(0);

	/* Initialise FNET */
	init_fnet();
	
	/* Wait for Ethernet connection */
	fnet_printf("Waiting for connection \n");
	while (!connected ) {
		connected = check_connected();
		fnet_timer_delay(FNET_TIMER_TICK_IN_SEC * 1); /* 1 Sec */
		fnet_printf("."); /* Print some errors */
	}
	
	/* Wait for DHCP server */
	//wait_dhcp(5); 
	
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
		char server_ipstr[FNET_IP6_ADDR_STR_SIZE];
		int server_ipstr_len = (sizeof(server_ipstr)/sizeof(server_ipstr[0]));
		
		/* Check for broadcast */
		bcast_status = check_broadcast(bcast_s, &server_sockaddr, 4951);
		if (bcast_status == 1) {
			int connect_error;
			fnet_printf("BCAST: Server bcast received \n");
			fnet_inet_ntop(server_sockaddr.sa_family, server_sockaddr.sa_data, server_ipstr, server_ipstr_len);
			fnet_printf("BCAST: Server IP: %s \n",server_ipstr);
			fnet_printf("BCAST: Server Port: %d \n",FNET_NTOHS(server_sockaddr.sa_port));
			
			/* Send a hello */
			connect_error = netprot_hello(&server_s, &server_sockaddr, 2000);
			if(connect_error == NETPROT_OK) {
				fnet_printf("HELL0: Server connection established \n");
			}
			else if(connect_error == NETPROT_ERR_REMOTE){
				fnet_printf("HELLO: Server did not respond or responded incorrectly \n");
			}
			else {
				fnet_printf("HELLO: Socket error \n");
			}
			/* Close the socket */
			netprot_goodbye(&server_s); /* Send */
		}
		else if (bcast_status == -1) {
			fnet_printf("BCAST: Error listening for server bcast \n");
		}
		
		/* Polling services.*/
		fnet_poll_services();
	}
	
	/* Should never end up here */
	return 0;
}
