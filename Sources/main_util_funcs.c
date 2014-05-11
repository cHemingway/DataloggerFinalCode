/*
 * Miscellaneous utility functions for using fnet.
 * In separate file to keep main.c tidy.
 */

#include <stdint.h>
#include <string.h>
#include "fnet.h" /* FNET stack */
#include "UID.h"


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


/* Function to set MAC address from uuid connected */
int set_mac_address(void) {
	char mac_address[6];
	char uid_buf[16];
	/* djb2 hash variables from http://www.cse.yorku.ca/~oz/hash.html*/
	uint32_t hash = 5381;
	int i;
	/* We only have 1 network, so get_default is fine */
	fnet_netif_desc_t cur_netif = fnet_netif_get_default();
	
	/* Get bits from UUID */
	UID_tobuf(uid_buf, sizeof(uid_buf));
	
	/* First 3 bytes are from FNET "fnet_cpu_config.h"
	 * Should hopefully not conflict with any other OUIDs.
	 */
	mac_address[0] = 0x00;
	mac_address[1] = 0x04;
	mac_address[2] = 0x8f;
	
	/* Use hash function on UUID as we don't know which bits are random */
	/* This hash function is "djb2" from http://www.cse.yorku.ca/~oz/hash.html
	 * hash is uint32_t instead of long to make size explicit.
	 */
	for (i = 0; i<(sizeof(uid_buf)/sizeof(uid_buf[0])); i++) {
		hash = ((hash << 5) + hash) + uid_buf[i]; /* hash * 33 + uid_buf */
	}
	
	/* Add the hash onto the end of the mac address */
	memcpy(mac_address+3, &hash, 3 * sizeof(char));
	
	/* Now set the created MAC address */
	return fnet_netif_set_hw_addr(cur_netif, (unsigned char*)mac_address, sizeof(mac_address));
}
