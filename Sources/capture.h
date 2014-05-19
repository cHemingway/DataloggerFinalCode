#ifndef CAPTURE_LOOP_H_
#define CAPTURE_LOOP_H_

#include "fnet.h"
#include "netprot_header.h"

/* Function to setup capture parameters */
void capture_setup(int n);

/* ISR to capture data */
void capture_isr(void);

/* Function to read capture buffer if full */
int capture_read(struct netstruct **buf);

/* Function sets the capture buffer to empty */
void capture_set_empty(void);

/* Function sets the decimation factor */
/* NOTE: Any value but 1 is treated as 1000 now */
void capture_set_decimation(int m);

/* Function gets the decimation factor */
int capture_get_decimation(void);

/* Function continuously reads capture buffer and decimates until it has enough values to send */
int capture_read_decimate(struct netstruct **buf);

#endif /* CAPTURE_LOOP_H_ */
