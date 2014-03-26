/**************************************************************************
* 
* Copyright 2012-2013 by Andrey Butok. FNET Community.
*
***************************************************************************
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License Version 3 
* or later (the "LGPL").
*
* As a special exception, the copyright holders of the FNET project give you
* permission to link the FNET sources with independent modules to produce an
* executable, regardless of the license terms of these independent modules,
* and to copy and distribute the resulting executable under terms of your 
* choice, provided that you also meet, for each linked independent module,
* the terms and conditions of the license of that module.
* An independent module is a module which is not derived from or based 
* on this library. 
* If you modify the FNET sources, you may extend this exception 
* to your version of the FNET sources, but you are not obligated 
* to do so. If you do not wish to do so, delete this
* exception statement from your version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
* You should have received a copy of the GNU General Public License
* and the GNU Lesser General Public License along with this program.
* If not, see <http://www.gnu.org/licenses/>.
*
**********************************************************************/ /*!
*
* @file fnet_mld.h
*
* @author Andrey Butok
*
* @brief Private. MLD protocol function definitions, data structures, etc.
*
***************************************************************************/

#ifndef _FNET_MLD_H_

#define _FNET_MLD_H_

#include "fnet_config.h"

#if FNET_CFG_MLD

#include "fnet.h"
#include "fnet_netif_prv.h"
#include "fnet_icmp6.h"
#include "fnet_ip6_prv.h"

/************************************************************************
 * RFC3810: MLD is used by an IPv6 router to discover the presence of 
 * multicast listeners on directly attached links, and to discover which
 * multicast addresses are of interest to those neighboring nodes.
 ************************************************************************/


/**********************************************************************
* MLD messages have the following format (RFC 2710)
***********************************************************************
*
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |     Type      |     Code      |          Checksum             |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |     Maximum Response Delay    |          Reserved             |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |                                                               |
*   +                                                               +
*   |                                                               |
*   +                       Multicast Address                       +
*   |                                                               |
*   +                                                               +
*   |                                                               |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*
***********************************************************************/
FNET_COMP_PACKED_BEGIN
typedef struct fnet_mld_header
{
    fnet_icmp6_header_t icmp6_header        FNET_COMP_PACKED; 
    unsigned short      max_resp_delay      FNET_COMP_PACKED;
    unsigned char       _reserved[2]        FNET_COMP_PACKED;
    fnet_ip6_addr_t     multicast_addr      FNET_COMP_PACKED;    
} fnet_mld_header_t;
FNET_COMP_PACKED_END


/***********************************************************************
 * MLD Router Alert option, in  IPv6 Hop-by-Hop Options.
 ***********************************************************************
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  | NextHeader(58)| Hdr Ext Len(0)|Option Type (5)|Opt Data Len(2)|
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  | Value (0=MLD)                 |Option Type (1)|Opt Data Len(0)|                         |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 ***********************************************************************/
FNET_COMP_PACKED_BEGIN
typedef struct fnet_mld_ra_option
{
    unsigned char               next_header         FNET_COMP_PACKED;   /* 8-bit selector. Identifies the type of header
                                                                         * immediately following the Options
                                                                         * header. */
    unsigned char               hdr_ext_length      FNET_COMP_PACKED;   /* 8-bit unsigned integer. Length of the Hop-by-
                                                                         * Hop Options header in 8-octet units, not
                                                                         * including the first 8 octets. */
    fnet_ip6_option_header_t    ra_option_header    FNET_COMP_PACKED;   /* Router Alert Option. */
    unsigned short              ra_option_value     FNET_COMP_PACKED;   /* Router Alert Option value. */   
    fnet_ip6_option_header_t    padn_option_header  FNET_COMP_PACKED;   /* Padding. It must be multiple to 8 octets. */                                                 
} fnet_mld_ra_option_header_t;
FNET_COMP_PACKED_END


/************************************************************************
*     Function Prototypes
*************************************************************************/
void fnet_mld_join(fnet_netif_t *netif, fnet_ip6_addr_t  *group_addr);
void fnet_mld_leave(fnet_netif_t *netif, fnet_ip6_addr_t  *group_addr);
void fnet_mld_report_all(fnet_netif_t *netif);
void fnet_mld_query_receive(fnet_netif_t *netif, fnet_ip6_addr_t *src_ip, fnet_ip6_addr_t *dest_ip, fnet_netbuf_t *nb, fnet_netbuf_t *ip6_nb);


#endif /* FNET_CFG_MLD */

#endif /* _FNET_MLD_H_ */
