#include "dhcp.h"
#include "fnet.h"


enum callback_state  {
	WAITING,	/* Waiting for server response */
	TIMEOUT,	/* Number of tries exceeded */
	SUCCESS		/* Success, IP address found */
};

/* Struct passed to each counter */
struct callback_params_t {
	int max_tries;				/* Max number of tries, 0 for infinate */
	int cur_try;				/* Current attempt no */
	enum callback_state state; 	/* Current state */
};

/* Callback for discover message sent (in progress) */
static void handler_discover(fnet_netif_desc_t netif, void *param) {
	struct callback_params_t *callback_param;
	callback_param = (struct callback_params_t *)param;
	
	/* Print Status */
	fnet_printf("DHCP: attempt %d/%d \n",callback_param->cur_try, callback_param->max_tries);
	
	/* Check if number of tries exceeded */
	if (callback_param->max_tries == callback_param->cur_try) {
			callback_param->state = TIMEOUT;
	}
	else { /* Increment number of tries */
			callback_param->cur_try += 1;
	}
}

/* Callback for address updated (done) */
static void handler_updated(fnet_netif_desc_t netif, void *param) {
	struct callback_params_t *callback_param;
	callback_param = (struct callback_params_t *)param;
	callback_param->state = SUCCESS;
}

int wait_dhcp(int tries) {
	fnet_netif_desc_t netif;
	struct fnet_dhcp_params dhcp_params;
	struct callback_params_t callback_params;
	
	/* Get Default netif */
	netif =  fnet_netif_get_default();
	if (!netif) {
		return -1;
	}
	
	/* Clear params struct */
	fnet_memset_zero(&dhcp_params, sizeof(struct fnet_dhcp_params));
	
	/* Turn on link-local support */
	dhcp_params.retries_until_link_local = tries;
	
	/* Initialise DHCP client */
	if(fnet_dhcp_init(netif, &dhcp_params) == FNET_ERR) {
		return -1;
	}
	
	/* Setup param for callbacks */
	callback_params.max_tries = tries + 1; /* Add one so we get link local before timeout */
	callback_params.cur_try = 1;
	callback_params.state = WAITING;
	
	
	/* Print message */
	fnet_printf("\nDHCP: Waiting for server \n");
	
	/* Register call backs */
	fnet_dhcp_handler_updated_set(handler_updated, &callback_params);
	fnet_dhcp_handler_discover_set(handler_discover, &callback_params);
	
	/* Poll background services until address found or n_tries exceeded */
	while (callback_params.state == WAITING) {
		fnet_poll_services();
	}
	
	/* Now evaluate response */
	if (callback_params.state == SUCCESS) {
		fnet_printf("DHCP: Success! \n");
		return 0;  /* Success */
	}
	else {
		fnet_printf("DHCP: Failed! Using link local address \n");
		/* Keep DHCP server running in background */
		return -1; /* Failure */
	}
}
