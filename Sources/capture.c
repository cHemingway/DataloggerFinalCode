#include "capture.h"
#include "dspi.h"
#include "netprot_header.h"
#include "netprot_header_prv.h"

#include "cic.h" /* For decimation factor */

#include <stdint.h>
#include <stddef.h>

/* Maximum number of samples to store */
#define MAX_SAMPLES (1000)
/* Size of the buffers to store data in */
#define BUFFER_SIZE (MAX_SAMPLES * sizeof(uint16_t)) + NETSTRUCT_LEN
/* Alignment boundary */
#define ALIGNMENT_BOUNDARY (sizeof(uint32_t));

/* Global double buffers for data storage including struct header */
volatile char __attribute__ ((aligned(8))) data0[BUFFER_SIZE];
volatile char __attribute__ ((aligned(8))) data1[BUFFER_SIZE];
volatile char __attribute__ ((aligned(8))) data2[BUFFER_SIZE];

/* ISR buffers */
struct netstruct *buf0 = (struct netstruct *) data0;
struct netstruct *buf1 = (struct netstruct *) data1;
struct netstruct *cur_buf = NULL;  

/* Decimate output buffer */
struct netstruct *decimate_buf = (struct netstruct *) data2;

/* Global for current sample number */
volatile int nsample = 0;

/* Global for buffer full */
volatile char buf_full = 0;

/* Global to indicate buffer was not emptied in time */
volatile char buf_overflow = 0;

/* Global for samples per buffer */
volatile int samples_per_buffer;

/* Global for decimation factor*/
volatile int decimation_factor = 1;

/* Decimation filters */
cic_decimate_instance_q32 filter1;
cic_decimate_instance_q32 filter2;

/* Decimate intermediate buffers */
q32_t filter1_input[MAX_SAMPLES];
q32_t filter2_input[MAX_SAMPLES/125];
q32_t filter2_output[MAX_SAMPLES/8];

/* Function to setup capture parameters */
void capture_setup(int n) {
	samples_per_buffer = n; /* Set samples per buffer */
	/* Reset values */
	nsample = 0;
	buf_full = 0;
	cur_buf  = buf0;
	buf_overflow = 0;
}

/* Interrupt service routine for capture */
void capture_isr(void) {
	/* Read input value and save in structure */
	cur_buf->data[nsample] = dspi_no_fifo_3(1);
	
	/* Increment n samples*/
	nsample++;
	/* Swap buffers on overflow */
	if (nsample >= samples_per_buffer-1) {
		if (cur_buf == buf0) { /* buf0 -> buf1 */
			cur_buf = buf1;
		}
		else { 					/* buf1 -> buf0 */
			cur_buf = buf0;
		}
		/* clear nsample */
		nsample = 0;
		/* Check if buffer was emptied in time (buf_full == 0) */
		if (buf_full) buf_overflow = 1;
		/* raise flag */
		buf_full = 1;
	}
}

/* Get current hardware buffer to send */
int capture_read(struct netstruct **buf) {
	if (buf_full) {
		*buf = cur_buf;
		buf_full = 0;
		return samples_per_buffer;
	}
	else {
		return 0;
	}
}

/* Set buf_full to 0 */
void capture_set_empty(void) {
	buf_full = 0;
}

void capture_set_decimation(int m) {
	if (m != 1) {
		decimation_factor = 1000; /* Hardcoded for now */
	}
	
	/* Setup filters, hardcoded for now. See block diagram for values */
	cic_decimate_init_q32(&filter1, 125, 2, 2, 62500, samples_per_buffer);
	cic_decimate_init_q32(&filter2, 8, 3, 2, 4096, samples_per_buffer/125);
}

int capture_get_decimation(void) {
	return decimation_factor;
}

int capture_read_decimate(struct netstruct **buf) {
	int i, samples_read = 0, total_read = 0, total_output = 0;
	struct netstruct *read_buf = NULL;
	uint16_t *output = (*buf)->data;
	
	while(total_output < samples_per_buffer) {
		/* Get samples from input buffer */
		while(samples_read==0) {
			samples_read = capture_read(&read_buf);
		}
		
		/* Expand to 32 bit */
		for (i=0; i<samples_per_buffer; i++) {
			filter1_input[i] = read_buf->data[i];
		}
		
		/* Decimate with filter 1 */
		cic_decimate_q32(&filter1, filter1_input, filter2_input, samples_read);
		
		/* Decimate with filter 2 */
		cic_decimate_q32(&filter2, filter1_input, filter2_output, samples_read / 125);
		
		/* Contract and add to output, NOTE: presumes samples_per_buffer < M */
		total_read += samples_read;
		if (total_read > decimation_factor) {
			*output++ = filter2_output[0]; /* Very hacky, should be a loop */
			total_read = 0;
			total_output++;
		}
	}
	
	return total_output;
}
