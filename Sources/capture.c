#include "capture.h"
#include "dspi.h"

void capture_setup() {
	
}

void capture_isr(void) {
	dspi_no_fifo_3(1);
}

