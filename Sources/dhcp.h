#ifndef __DHCP_H_
#define __DHCP_H_


/* NAME: wait_dhcp
 * DESCRIPTION: Attempts to contact the DHCP server
 * PARAMS:
 * 		tries:	The number of attempts to make
 * 				Time between attempts is set by FNET_CFG_DHCP_RESPONSE_TIMEOUT
 * 				After n tries have failed, stops the DHCP client service.
 * RETURNS:
 * 				-1 on failure
 * 				0  on success
 * 		
 */
int wait_dhcp(int tries);



#endif /* __DHCP_H_ */
