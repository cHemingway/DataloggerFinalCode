#include "netprot_header.h"
#include "netprot_header_prv.h"

#include <stdint.h>
#include <stddef.h>

/* Include correct hton, ntohl */
#include "fnet.h"
#define htonl fnet_htonl
#define ntohl fnet_ntohl

int netprot_header_append(struct netstruct *toappend, int count,
 						  int datasize, long long dt_ns, char flags) 
{
	int dt_s;
	/* Check Pointers */
	if (toappend==NULL) {
		return -1;
	}

	/* Set default fields */
	toappend->version 	 = NETPROT_VERSION;
	toappend->header_size = offsetof(struct netstruct, data);
	toappend->channel_no = 0; /* Hardcoded for now */
	toappend->ts_s		 = 0; /* Timestamp is not used yet */
	toappend->ts_ns		 = 0;  

	/* Set flags */
	flags |= TS_INVALID; /* No timstamp support yet */
	toappend->flags = (uint8_t)(flags & 0xff);

	/* Set count */
	toappend->count = htonl(count);

	/* Calculate and set dt */
	dt_s = dt_ns / 1000000000u;
	toappend->dt_s = htonl(dt_s);
	toappend->dt_ns = htonl(dt_ns % 1000000000u);

	/* Set length */
	toappend->size = htonl(datasize);

	return 0; //Success
}


int netprot_header_decode(const void *decode, int decodelen,
							char **dataout, int *datasize,
							int *count, long long *dt_ns, char *flags)
{
	char *ptr; /* General pointer */
	uint8_t version, header_size;
	uint32_t dt_s;


	/* Check pointers */
	if ((decode==NULL) || (datasize==NULL ) || (count==NULL) || (dt_ns==NULL) ||
		(flags==NULL) ){
		return NP_ERR_PARAM;
	}

	/* Check length is enough for a header */
	if (decodelen < NETSTRUCT_LEN) {
		return NP_ERR_PARAM;
	}

	/* Extract version and check */
	version = extract_uint8(decode);
	if (version != NETPROT_VERSION) {
		return NP_ERR_CORRUPT; /* Invalid protocol version */
	}

	/* Check header size, could be corrupted by padding not working */
	ptr = ptroffset(decode, HEADER_SIZE_OFFSET);
	header_size = extract_uint8(ptr);
	if (header_size != NETSTRUCT_LEN) {
		return NP_ERR_CORRUPT; /* Invalid protocol version */
	}

	/* Extract flags */
	*flags = extract_uint8(ptroffset(decode, FLAGS_OFFSET));
	*count = extract_uint32(ptroffset(decode, COUNT_OFFSET));

	/* Extract dt_ns and convert */
	*dt_ns = extract_uint32(ptroffset(decode, DT_NS_OFFSET));
	if (*dt_ns >= 1000000000) { /* dt_ns is corrupted */
		return NP_ERR_CORRUPT;
	}
	dt_s = extract_uint32(ptroffset(decode, DT_NS_OFFSET));
	*dt_ns += 1000000000 * (dt_s); /* Append seconds as well */

	/* Extract data size and check if valid */
	*datasize = extract_uint32(ptroffset(decode, SIZE_OFFSET));
	if( (*datasize + NETSTRUCT_LEN) > decodelen ) {
		return NP_ERR_CORRUPT; /* Datasize is larger than available data */
	}

	/* Finally, extract the data */
	*dataout  = ptroffset(decode, DATA_OFFSET);

	/* Success */
	return 0;
}

/* Local only function for getting size of a netstruct and data */
int netprot_header_getsize(struct netstruct *in) {
	return NETSTRUCT_LEN + ntohl(in->size);
}
