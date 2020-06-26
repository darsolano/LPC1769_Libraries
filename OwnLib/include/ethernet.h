/*
 * ethernet.h
 *
 *  Created on: 20/1/2015
 *      Author: dsolano
 */

#ifndef INCLUDE_ETHERNET_H_
#define INCLUDE_ETHERNET_H_

#include <chip.h>
#include <lpc_types.h>

#define UIP_ETHTYPE_ARP 0x0806
#define UIP_ETHTYPE_IP  0x0800
#define UIP_ETHTYPE_IP6 0x86dd


// mac dest addr
typedef struct
{
	uint8_t eth_dest_addr[6];
}eth_dest_addr_Typedef;

// mac Source, my own Mac
typedef struct
{
	uint8_t eth_src_addr[6];
}eth_src_addr_Typedef;

typedef struct
{
	eth_dest_addr_Typedef *dst;
	eth_src_addr_Typedef *src;
	uint32_t vlan_tag;
	uint16_t type_length; 	//If the value in this field of the frame is numerically greater than or equal to 1536 decimal (0x600	hex),
							//then the field is being used as a type field.
	uint8_t *data_field;	// must point to a buffer of len < 1536 bytes

}eth_hdr_typedef;


#endif /* INCLUDE_ETHERNET_H_ */
