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

#endif /* CAPTURE_LOOP_H_ */
