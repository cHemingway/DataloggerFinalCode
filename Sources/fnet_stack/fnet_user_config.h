/**********************************************************************//*!
 *
 * @file fnet_user_config.h
 *
 * @brief FNET User configuration file.
 * It should be used to change any default configuration parameter.
 *
 ***************************************************************************/

#ifndef _FNET_USER_CONFIG_H_

#define _FNET_USER_CONFIG_H_


// We are running at 100Mhz
#define FNET_CFG_CPU_CLOCK_HZ		(100000000)

// Custom macro for any changes made to FNET (only UART pins so far)
#define FNET_CFG_CPU_DATALOGGER_BOARD	(1)

//Changed to use UART 0
#define FNET_CFG_CPU_SERIAL_PORT_DEFAULT    (0) 

//Increase this to try and defeat slow start
#define FNET_CFG_SOCKET_TCP_TX_BUF_SIZE 20*1024

/*****************************************************************************
 * Enable compiler support.
 ******************************************************************************/
#define FNET_CFG_COMP_GNUC         	(1)       

/*****************************************************************************
 * Processor type.
 * Selected processor definition should be only one and must be defined as 1. 
 * All others may be defined but must have 0 value.
 ******************************************************************************/
#define FNET_CFG_CPU_MK60N512       	(1)

/*****************************************************************************
 * IPv4 and/or IPv6 protocol support.
 ******************************************************************************/
#define FNET_CFG_IP4                (1)
#define FNET_CFG_IP6                (0)

/*****************************************************************************
 * IP address for the Ethernet interface. 
 * At runtime it can be changed by the fnet_netif_set_address() or 
 * by the DHCP client service.
 ******************************************************************************/
#define FNET_CFG_ETH0_IP4_ADDR      (FNET_IP4_ADDR_INIT(192, 168, 1, 22))

/*****************************************************************************
 * IP Subnet mask for the Ethernet interface. 
 * At runtime it can be changed by the fnet_netif_set_netmask() or 
 * by the DHCP client service.
 ******************************************************************************/
#define FNET_CFG_ETH0_IP4_MASK      (FNET_IP4_ADDR_INIT(255, 255, 255, 0))

/*****************************************************************************
 * Gateway IP address for the Ethernet interface.
 * At runtime it can be changed by the fnet_netif_set_gateway() or 
 * by the DHCP client service.
 ******************************************************************************/
#define FNET_CFG_ETH0_IP4_GW        (FNET_IP4_ADDR_INIT(0, 0, 0, 0))

/*****************************************************************************
 * DNS server IP address for the Ethernet interface.
 * At runtime it can be changed by the fnet_netif_set_dns() or 
 * by the DHCP client service. 
 * It is used only if FNET_CFG_DNS is set to 1.
 ******************************************************************************/
#define FNET_CFG_ETH0_IP4_DNS       (FNET_IP4_ADDR_INIT(0, 0, 0, 0)) 

/*****************************************************************************
 * Size of the internal static heap buffer. 
 * This definition is used only if the fnet_init_static() was 
 * used for the FNET initialization.
 ******************************************************************************/
#ifdef RAM_TARGET
#define FNET_CFG_HEAP_SIZE          (22 * 1024)
#else
#define FNET_CFG_HEAP_SIZE          (40 * 1024)
#endif

/*****************************************************************************
 * TCP protocol support.
 * You can disable it to save a substantial amount of code if 
 * your application only needs UDP. By default it is enabled.
 ******************************************************************************/
#define FNET_CFG_TCP                (1)

/*****************************************************************************
 * UDP protocol support.
 * You can disable it to save a some amount of code if your 
 * application only needs TCP. By default it is enabled.
 ******************************************************************************/
#define FNET_CFG_UDP                (1)

/*****************************************************************************
 * UDP checksum.
 * If enabled, the UDP checksum will be generated for transmitted 
 * datagrams and be verified on received UDP datagrams.
 * You can disable it to speedup UDP applications. 
 * By default it is enabled.
 ******************************************************************************/
#define FNET_CFG_UDP_CHECKSUM       (1)

/*****************************************************************************
 * IP fragmentation.
 * If the IP fragmentation is enabled, the IP will attempt to reassemble IP 
 * packet fragments and will able to generate fragmented IP packets.
 * If disabled, the IP will  silently discard fragmented IP packets..
 ******************************************************************************/
#define FNET_CFG_IP4_FRAGMENTATION  (1)

// Enable Multicast Support
#define FNET_CFG_MULTICAST 			(1)

/*****************************************************************************
 * DHCP Client service support.
 ******************************************************************************/
#define FNET_CFG_DHCP               (1)

/*****************************************************************************
 * HTTP Server service support.
 ******************************************************************************/
/* Customise: Disable HTTP */
#define FNET_CFG_HTTP                       (0)
#define FNET_CFG_HTTP_AUTHENTICATION_BASIC  (0) 
#define FNET_CFG_HTTP_POST                  (0) 

/*****************************************************************************
 * Telnet Server service support.
 ******************************************************************************/
/* Customise: Disable telnet */
#define FNET_CFG_TELNET                     (0)

/*****************************************************************************
 * Flash Module driver support.
 ******************************************************************************/
#define FNET_CFG_FLASH                      (0)

/*****************************************************************************
 * DNS client/resolver service support.
 ******************************************************************************/
/* Customise: Disable DNS */
#ifdef RAM_TARGET /* Just to save RAM */
#define FNET_CFG_DNS                    (0)
#define FNET_CFG_DNS_RESOLVER           (0)
#else
#define FNET_CFG_DNS                    (0)
#define FNET_CFG_DNS_RESOLVER           (0)
#endif

/*****************************************************************************
 * PING service support.
 ******************************************************************************/
#ifdef RAM_TARGET /* Just to save RAM */
#define FNET_CFG_PING                   (0)
#else
#define FNET_CFG_PING                   (1)
#endif



#endif /* _FNET_USER_CONFIG_H_ */

