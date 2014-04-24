#ifndef __NETPROT_HEADER_PRV__
#define __NETPROT_HEADER_PRV__

#include <stdint.h>

#define getoffsetptr(val, offset) (((char*)&(val)) + (offset))
#define ptroffset(ptr, offset) (((char*)(ptr)) + (offset))
#define extract_uint8(ptr)  ((int) (*((uint8_t *)(ptr)) & 0xff))
#define extract_uint32(ptr) (ntohl((int)*((uint32_t *)(ptr))) & 0xffffffff)

#define NETSTRUCT_LEN 28

/* Enum for offsets in header, values in octets */
enum header_offsets {
	VERSION_OFFSET 			= 0,	/* 8 bits  */
	HEADER_SIZE_OFFSET 		= 1,	/* 8 bits  */
	FLAGS_OFFSET			= 2,	/* 8 bits  */
	CHANNEL_OFFSET			= 3,	/* 8 bits  */
	COUNT_OFFSET			= 4,	/* 32 bits */
	TS_S_OFFSET				= 8,	/* 32 bits */
	TS_NS_OFFSET			= 12,	/* 32 bits */
	DT_S_OFFSET				= 16,	/* 32 bits */
	DT_NS_OFFSET			= 20,	/* 32 bits */
	SIZE_OFFSET 			= 24,	/* 32 bits */
	DATA_OFFSET				= 28,	/* char[]  */
};

/* __NETPROT_HEADER_PRV__ */
#endif 