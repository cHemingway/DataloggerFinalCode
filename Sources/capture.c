#include "capture.h"
#include "dspi.h"
#include "netprot_header.h"

#include <stdint.h>
#include <stddef.h>

/* Maximum number of samples to store */
#define MAX_SAMPLES (1000)
/* Size of each sample */
#define BUFFER_SIZE (MAX_SAMPLES * sizeof(uint16_t))
/* Alignment boundary */
#define ALIGNMENT_BOUNDARY (sizeof(uint32_t));

/* Global double buffers for data storage including struct header */
volatile char __attribute__ ((aligned(8))) data0[BUFFER_SIZE];
volatile char __attribute__ ((aligned(8))) data1[BUFFER_SIZE];

/* Actual buffers */
struct netstruct *buf0 = (struct netstruct *) data0;
struct netstruct *buf1 = (struct netstruct *) data1;
struct netstruct *cur_buf = NULL;  

/* Global for current sample number */
volatile int nsample = 0;

/* Global for buffer full */
volatile char buf_full = 0;

/* Global to indicate buffer was not emptied in time */
volatile char buf_overflow = 0;

/* Global for samples per buffer */
int samples_per_buffer;

/* Function to setup capture parameters */
void capture_setup(int n) {
	samples_per_buffer = n; /* Set samples per buffer */
	/* Reset values */
	nsample = 0;
	buf_full = 0;
	cur_buf  = buf0;
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

